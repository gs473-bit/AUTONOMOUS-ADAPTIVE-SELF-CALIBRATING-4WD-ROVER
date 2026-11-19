#include "hal_port/adc_hw.h"
#include "config.h"
#include "pins.h"
#include "stm32f3xx_hal.h"
static ADC_HandleTypeDef adc1;
static DMA_HandleTypeDef adc_dma;
static volatile uint16_t samples[5];
static uint8_t active;
void HAL_ADC_MspInit(ADC_HandleTypeDef *h){
 if(h->Instance!=ADC1)return;
 __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_ADC12_CLK_ENABLE();__HAL_RCC_DMA1_CLK_ENABLE();
 GPIO_InitTypeDef io={0};
 io.Mode=GPIO_MODE_ANALOG;io.Pull=GPIO_NOPULL;
 io.Pin=GPIO_PIN_0|GPIO_PIN_1;HAL_GPIO_Init(GPIOB,&io);
 io.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;HAL_GPIO_Init(GPIOC,&io);
 adc_dma.Instance=DMA1_Channel1;
 adc_dma.Init.Direction=DMA_PERIPH_TO_MEMORY;
 adc_dma.Init.PeriphInc=DMA_PINC_DISABLE;
 adc_dma.Init.MemInc=DMA_MINC_ENABLE;
 adc_dma.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;
 adc_dma.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;
 adc_dma.Init.Mode=DMA_CIRCULAR;
 adc_dma.Init.Priority=DMA_PRIORITY_HIGH;
 (void)HAL_DMA_Init(&adc_dma);
 __HAL_LINKDMA(h,DMA_Handle,adc_dma);
 HAL_NVIC_SetPriority(DMA1_Channel1_IRQn,1u,0u);HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}
void DMA1_Channel1_IRQHandler(void){HAL_DMA_IRQHandler(&adc_dma);}
int adc_hw_init(void){
 active=0u;
 if(!AETHER_ACTUATION_ENABLED || !AETHER_PINMAP_VERIFIED)return 0;
 adc1.Instance=ADC1;
 adc1.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;
 adc1.Init.Resolution=ADC_RESOLUTION_12B;
 adc1.Init.ScanConvMode=ADC_SCAN_ENABLE;
 adc1.Init.ContinuousConvMode=ENABLE;
 adc1.Init.DiscontinuousConvMode=DISABLE;
 adc1.Init.ExternalTrigConv=ADC_SOFTWARE_START;
 adc1.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;
 adc1.Init.DataAlign=ADC_DATAALIGN_RIGHT;
 adc1.Init.NbrOfConversion=5u;
 adc1.Init.DMAContinuousRequests=ENABLE;
 adc1.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;
 if(HAL_ADC_Init(&adc1)!=HAL_OK)return 0;
 const uint32_t channels[5]={ADC_CHANNEL_11,ADC_CHANNEL_12,ADC_CHANNEL_6,ADC_CHANNEL_7,ADC_CHANNEL_8};
 for(unsigned i=0;i<5u;i++){
  ADC_ChannelConfTypeDef ch={0};
  ch.Channel=channels[i];ch.Rank=i+1u;
  ch.SamplingTime=ADC_SAMPLETIME_61CYCLES_5;
  ch.SingleDiff=ADC_SINGLE_ENDED;ch.OffsetNumber=ADC_OFFSET_NONE;
  if(HAL_ADC_ConfigChannel(&adc1,&ch)!=HAL_OK)return 0;
 }
 if(HAL_ADCEx_Calibration_Start(&adc1,ADC_SINGLE_ENDED)!=HAL_OK)return 0;
 if(HAL_ADC_Start_DMA(&adc1,(uint32_t *)(void *)samples,5u)!=HAL_OK)return 0;
 active=1u;return 1;
}
int adc_hw_volts(float volts[5]){
 if(!active || !volts)return 0;
 for(unsigned i=0;i<5u;i++)volts[i]=(float)samples[i]*3.3f/4095.0f;
 return 1;
}
