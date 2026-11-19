#include "hal_port/iwdg_hw.h"
#include "stm32f3xx_hal.h"
#include "config.h"
#include "pins.h"
static IWDG_HandleTypeDef watchdog;
static int ready;
int iwdg_hw_init(void){
 ready=0;
 if(!AETHER_ACTUATION_ENABLED || !AETHER_PINMAP_VERIFIED)return 0;
 watchdog.Instance=IWDG;
 watchdog.Init.Prescaler=IWDG_PRESCALER_32;
 watchdog.Init.Reload=1250u;/* Approx 1 s on 40kHz LSI; verify LSI tolerance */
 if(HAL_IWDG_Init(&watchdog)!=HAL_OK)return 0;
 ready=1;return 1;
}
int iwdg_hw_feed(void){
 return ready && HAL_IWDG_Refresh(&watchdog)==HAL_OK;
}
