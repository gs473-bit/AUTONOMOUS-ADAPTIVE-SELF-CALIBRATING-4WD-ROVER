#include "stm32f4xx_hal.h"
#include "config.h"
/* Safe bootstrap: no user motor GPIO initialization or PWM enabled. */
int main(void) {
    HAL_Init();
    while (1) {
        HAL_Delay(100);
    }
}
