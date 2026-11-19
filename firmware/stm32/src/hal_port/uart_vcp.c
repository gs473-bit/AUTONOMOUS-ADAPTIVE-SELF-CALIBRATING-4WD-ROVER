#include "hal_port/uart_vcp.h"
#include "stm32f3xx_hal.h"
/* STM32 USART2 PA2/PA3 maps through the NUCLEO-F303RE ST-LINK VCP.
 * DMA RX is circular; main loop polls DMA producer index, never blocks on RX.
 * These pins are RESERVED and must not be used for ADC/motors. */
static UART_HandleTypeDef host_uart;
static DMA_HandleTypeDef rx_dma;
#define RX_DMA_SIZE 256u
static uint8_t rx_dma_buffer[RX_DMA_SIZE];
static uint16_t rx_read_index=0u;
static uint8_t ready=0u;

void HAL_UART_MspInit(UART_HandleTypeDef *h){
 if(h->Instance!=USART2)return;
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_USART2_CLK_ENABLE();
 __HAL_RCC_DMA1_CLK_ENABLE();
 GPIO_InitTypeDef gpio={0};
 gpio.Pin=GPIO_PIN_2|GPIO_PIN_3;
 gpio.Mode=GPIO_MODE_AF_PP;
 gpio.Pull=GPIO_PULLUP;
 gpio.Speed=GPIO_SPEED_FREQ_HIGH;
 gpio.Alternate=GPIO_AF7_USART2;
 HAL_GPIO_Init(GPIOA,&gpio);
 rx_dma.Instance=DMA1_Channel6; /* USART2_RX: verify DMA routing in F303 RM0316 */
 rx_dma.Init.Direction=DMA_PERIPH_TO_MEMORY;
 rx_dma.Init.PeriphInc=DMA_PINC_DISABLE;
 rx_dma.Init.MemInc=DMA_MINC_ENABLE;
 rx_dma.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;
 rx_dma.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;
 rx_dma.Init.Mode=DMA_CIRCULAR;
 rx_dma.Init.Priority=DMA_PRIORITY_HIGH;
 (void)HAL_DMA_Init(&rx_dma);
 __HAL_LINKDMA(h,hdmarx,rx_dma);
 HAL_NVIC_SetPriority(DMA1_Channel6_IRQn,1u,0u);
 HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
 HAL_NVIC_SetPriority(USART2_IRQn,2u,0u);
 HAL_NVIC_EnableIRQ(USART2_IRQn);
}
void DMA1_Channel6_IRQHandler(void){HAL_DMA_IRQHandler(&rx_dma);}
void USART2_IRQHandler(void){HAL_UART_IRQHandler(&host_uart);}
int uart_vcp_init(void){
 host_uart.Instance=USART2;
 host_uart.Init.BaudRate=115200;
 host_uart.Init.WordLength=UART_WORDLENGTH_8B;
 host_uart.Init.StopBits=UART_STOPBITS_1;
 host_uart.Init.Parity=UART_PARITY_NONE;
 host_uart.Init.Mode=UART_MODE_TX_RX;
 host_uart.Init.HwFlowCtl=UART_HWCONTROL_NONE;
 host_uart.Init.OverSampling=UART_OVERSAMPLING_16;
 if(HAL_UART_Init(&host_uart)!=HAL_OK)return 0;
 rx_read_index=0u;
 if(HAL_UART_Receive_DMA(&host_uart,rx_dma_buffer,RX_DMA_SIZE)!=HAL_OK)return 0;
 ready=1u;return 1;
}
size_t uart_vcp_read(uint8_t *out,size_t cap){
 if(!ready || !out || cap==0u)return 0u;
 size_t n=0u;
 const uint16_t end=(uint16_t)(RX_DMA_SIZE-__HAL_DMA_GET_COUNTER(&rx_dma));
 while(rx_read_index!=end && n<cap){
  out[n++]=rx_dma_buffer[rx_read_index];
  rx_read_index=(uint16_t)((rx_read_index+1u)%RX_DMA_SIZE);
 }
 return n;
}
int uart_vcp_write(const uint8_t *data,uint16_t size){
 if(!ready || !data || !size)return 0;
 return HAL_UART_Transmit(&host_uart,(uint8_t *)(uintptr_t)data,size,10u)==HAL_OK;
}
