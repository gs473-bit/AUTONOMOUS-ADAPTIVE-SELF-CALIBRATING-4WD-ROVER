#include "hal_port/actuation_hw.h"
#include "stm32f3xx_hal.h"
#include "config.h"
#include "pins.h"
#include <stddef.h>
static TIM_HandleTypeDef timers[4]; /* TIM2,TIM3,TIM4,TIM8 */
static TIM_HandleTypeDef pwm_a,pwm_b; /* TIM1 three channels, TIM17 fourth */
static uint8_t configured;
static volatile uint8_t irq_fault_mask,irq_estop_open;
static const struct {GPIO_TypeDef *port;uint16_t pin;} dirs[4]={
 {GPIOC,GPIO_PIN_8},{GPIOC,GPIO_PIN_9},{GPIOC,GPIO_PIN_10},{GPIOC,GPIO_PIN_11}
};
static const struct {GPIO_TypeDef *port;uint16_t pin;} sleeps[4]={
 {GPIOB,GPIO_PIN_3},{GPIOB,GPIO_PIN_4},{GPIOB,GPIO_PIN_5},{GPIOB,GPIO_PIN_12}
};
static const struct {GPIO_TypeDef *port;uint16_t pin;} faults[4]={
 {GPIOC,GPIO_PIN_4},{GPIOC,GPIO_PIN_5},{GPIOB,GPIO_PIN_10},{GPIOB,GPIO_PIN_8}
};
void HAL_GPIO_EXTI_Callback(uint16_t pin){
 if(pin==GPIO_PIN_4)irq_fault_mask|=1u;
 if(pin==GPIO_PIN_5)irq_fault_mask|=2u;
 if(pin==GPIO_PIN_10)irq_fault_mask|=4u;
 if(pin==GPIO_PIN_8)irq_fault_mask|=8u;
 if(pin==GPIO_PIN_13)irq_estop_open=1u;
}
void EXTI4_IRQHandler(void){HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);}
void EXTI9_5_IRQHandler(void){HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);}
void EXTI15_10_IRQHandler(void){HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);}
static void configure_gpio(GPIO_TypeDef *port,uint16_t pins,uint32_t mode,uint32_t pull,uint32_t af){
 GPIO_InitTypeDef io={0};io.Pin=pins;io.Mode=mode;io.Pull=pull;
 io.Speed=GPIO_SPEED_FREQ_HIGH;io.Alternate=af;HAL_GPIO_Init(port,&io);
}
static int encoder_setup(TIM_HandleTypeDef *h,TIM_TypeDef *inst,uint32_t period){
 h->Instance=inst;h->Init.Prescaler=0u;h->Init.CounterMode=TIM_COUNTERMODE_UP;
 h->Init.Period=period;h->Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
 TIM_Encoder_InitTypeDef e={0};e.EncoderMode=TIM_ENCODERMODE_TI12;
 e.IC1Polarity=TIM_ICPOLARITY_RISING;e.IC1Selection=TIM_ICSELECTION_DIRECTTI;
 e.IC1Prescaler=TIM_ICPSC_DIV1;e.IC1Filter=4u;
 e.IC2Polarity=TIM_ICPOLARITY_RISING;e.IC2Selection=TIM_ICSELECTION_DIRECTTI;
 e.IC2Prescaler=TIM_ICPSC_DIV1;e.IC2Filter=4u;
 return HAL_TIM_Encoder_Init(h,&e)==HAL_OK && HAL_TIM_Encoder_Start(h,TIM_CHANNEL_ALL)==HAL_OK;
}
static int pwm_setup(TIM_HandleTypeDef *h,TIM_TypeDef *inst,unsigned channels){
 h->Instance=inst;h->Init.Prescaler=0u;h->Init.Period=3599u;/* 72MHz / 20kHz */
 h->Init.CounterMode=TIM_COUNTERMODE_UP;h->Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
 if(HAL_TIM_PWM_Init(h)!=HAL_OK)return 0;
 TIM_OC_InitTypeDef cfg={0};cfg.OCMode=TIM_OCMODE_PWM1;cfg.Pulse=0u;
 cfg.OCPolarity=TIM_OCPOLARITY_HIGH;cfg.OCFastMode=TIM_OCFAST_DISABLE;
 const uint32_t ch[4]={TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};
 for(unsigned i=0;i<channels;i++){
  if(HAL_TIM_PWM_ConfigChannel(h,&cfg,ch[i])!=HAL_OK)return 0;
  if(HAL_TIM_PWM_Start(h,ch[i])!=HAL_OK)return 0;/* zero duty + nSLEEP LOW */
 }
 return 1;
}
static void output_pwm(unsigned wheel,float duty){
 if(!configured || wheel>=4u)return;
 if(duty<0.0f)duty=0.0f;
 if(duty>1.0f)duty=1.0f;
 const uint32_t pulse=(uint32_t)(3599.0f*duty);
 TIM_HandleTypeDef *h=wheel==3u?&pwm_b:&pwm_a;
 uint32_t ch=wheel==0u?TIM_CHANNEL_1:wheel==1u?TIM_CHANNEL_2:wheel==2u?TIM_CHANNEL_3:TIM_CHANNEL_1;
 __HAL_TIM_SET_COMPARE(h,ch,pulse);
}
static void output_phase(unsigned wheel,int fwd){
 if(!configured || wheel>=4u)return;
 HAL_GPIO_WritePin(dirs[wheel].port,dirs[wheel].pin,fwd?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static void output_sleep(unsigned wheel,int awake){
 if(!configured || wheel>=4u)return;
 HAL_GPIO_WritePin(sleeps[wheel].port,sleeps[wheel].pin,awake?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static const motor_output_t outputs={output_pwm,output_phase,output_sleep};
const motor_output_t *actuation_hw_outputs(void){return &outputs;}
void actuation_hw_disable_all(void){
 for(unsigned i=0;i<4u;i++){output_pwm(i,0.0f);output_sleep(i,0);}
}
int actuation_hw_init(void){
 configured=0u;irq_fault_mask=0u;irq_estop_open=0u;
 if(!AETHER_ACTUATION_ENABLED || !AETHER_PINMAP_VERIFIED)return 0;
 __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_TIM1_CLK_ENABLE();__HAL_RCC_TIM2_CLK_ENABLE();__HAL_RCC_TIM3_CLK_ENABLE();
 __HAL_RCC_TIM4_CLK_ENABLE();__HAL_RCC_TIM8_CLK_ENABLE();__HAL_RCC_TIM17_CLK_ENABLE();
 /* LOW first: any pin enabling in a later boot phase remains safe. */
 for(unsigned i=0;i<4u;i++){
  HAL_GPIO_WritePin(sleeps[i].port,sleeps[i].pin,GPIO_PIN_RESET);
  configure_gpio(sleeps[i].port,sleeps[i].pin,GPIO_MODE_OUTPUT_PP,GPIO_PULLDOWN,0u);
  HAL_GPIO_WritePin(dirs[i].port,dirs[i].pin,GPIO_PIN_RESET);
  configure_gpio(dirs[i].port,dirs[i].pin,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,0u);
  configure_gpio(faults[i].port,faults[i].pin,GPIO_MODE_IT_FALLING,GPIO_PULLUP,0u);
 }
 configure_gpio(GPIOB,GPIO_PIN_13,GPIO_MODE_IT_RISING,GPIO_PULLUP,0u);
 HAL_NVIC_SetPriority(EXTI4_IRQn,0u,0u);HAL_NVIC_EnableIRQ(EXTI4_IRQn);
 HAL_NVIC_SetPriority(EXTI9_5_IRQn,0u,0u);HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
 HAL_NVIC_SetPriority(EXTI15_10_IRQn,0u,0u);HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);/* NC to GND */
 configure_gpio(GPIOA,GPIO_PIN_0|GPIO_PIN_1,GPIO_MODE_AF_PP,GPIO_PULLUP,GPIO_AF1_TIM2);
 configure_gpio(GPIOA,GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_AF_PP,GPIO_PULLUP,GPIO_AF2_TIM3);
 configure_gpio(GPIOB,GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_AF_PP,GPIO_PULLUP,GPIO_AF2_TIM4);
 configure_gpio(GPIOC,GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_AF_PP,GPIO_PULLUP,GPIO_AF4_TIM8);
 configure_gpio(GPIOA,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10,GPIO_MODE_AF_PP,GPIO_NOPULL,GPIO_AF6_TIM1);
 configure_gpio(GPIOB,GPIO_PIN_9,GPIO_MODE_AF_PP,GPIO_NOPULL,GPIO_AF1_TIM17);
 if(!encoder_setup(&timers[0],TIM2,0xffffffffu) ||
    !encoder_setup(&timers[1],TIM3,0xffffu) ||
    !encoder_setup(&timers[2],TIM4,0xffffu) ||
    !encoder_setup(&timers[3],TIM8,0xffffu) ||
    !pwm_setup(&pwm_a,TIM1,3u) || !pwm_setup(&pwm_b,TIM17,1u))return 0;
 configured=1u;actuation_hw_disable_all();return 1;
}
uint32_t actuation_hw_encoder_counter(unsigned wheel){
 return configured && wheel<4u ? __HAL_TIM_GET_COUNTER(&timers[wheel]):0u;
}
int actuation_hw_estop_nc_closed(void){
 return configured && !irq_estop_open && HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==GPIO_PIN_RESET;
}
int actuation_hw_fault_active(unsigned wheel){
 return !configured || wheel>=4u || (irq_fault_mask & (1u<<wheel)) || HAL_GPIO_ReadPin(faults[wheel].port,faults[wheel].pin)==GPIO_PIN_RESET;
}
