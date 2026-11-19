#ifndef AETHER_BOARD_H
#define AETHER_BOARD_H
#include <stdint.h>
void board_boot(void);
uint32_t board_millis(void);
void board_idle(void);
int aether_clock_72mhz(void);
#endif
