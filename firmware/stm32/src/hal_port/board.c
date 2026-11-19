#include "hal_port/board.h"
#include "stm32f3xx_hal.h"
void board_boot(void) {
 HAL_Init();
 if(!aether_clock_72mhz()){
  /* fail closed; nSLEEP GPIOs remain unconfigured and require physical pulldowns */
  for(;;){}
 }
}
uint32_t board_millis(void){return HAL_GetTick();}
void board_idle(void){HAL_Delay(1u);}
