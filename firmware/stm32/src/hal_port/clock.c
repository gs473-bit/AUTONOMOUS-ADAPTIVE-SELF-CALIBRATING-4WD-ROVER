#include "stm32f3xx_hal.h"
/* HSI8 PLLx9 -> 72MHz sysclk; avoids needing the HSE clock pin and permits 20kHz timer ARR3599. */
int aether_clock_72mhz(void){
 RCC_OscInitTypeDef osc={0};osc.OscillatorType=RCC_OSCILLATORTYPE_HSI;
 osc.HSIState=RCC_HSI_ON;osc.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;
 osc.PLL.PLLState=RCC_PLL_ON;osc.PLL.PLLSource=RCC_PLLSOURCE_HSI;
 osc.PLL.PLLMUL=RCC_PLL_MUL9;osc.PLL.PREDIV=RCC_PREDIV_DIV1;
 if(HAL_RCC_OscConfig(&osc)!=HAL_OK)return 0;
 RCC_ClkInitTypeDef clk={0};clk.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
 clk.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;clk.AHBCLKDivider=RCC_SYSCLK_DIV1;
 clk.APB1CLKDivider=RCC_HCLK_DIV2;clk.APB2CLKDivider=RCC_HCLK_DIV1;
 return HAL_RCC_ClockConfig(&clk,FLASH_LATENCY_2)==HAL_OK;
}
