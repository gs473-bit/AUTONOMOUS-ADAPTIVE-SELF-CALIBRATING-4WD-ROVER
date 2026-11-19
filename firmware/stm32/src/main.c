#include "stm32f3xx_hal.h"
#include "config.h"
/* Bootloader-safe no-motion bootstrap while actual wiring and thresholds are TODO. */
int main(void) {
    HAL_Init();
    for (;;) HAL_Delay(1u);
}
