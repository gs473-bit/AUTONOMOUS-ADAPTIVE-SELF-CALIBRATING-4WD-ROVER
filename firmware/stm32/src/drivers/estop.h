#ifndef AETHER_ESTOP_H
#define AETHER_ESTOP_H
#include <stdint.h>
typedef struct {uint32_t closed_since_ms;uint8_t closed,latched;} estop_t;
void estop_init(estop_t *e);
void estop_sample(estop_t *e,int contact_closed,uint32_t now_ms);
int estop_rearm(estop_t *e,uint32_t now_ms);
#endif
