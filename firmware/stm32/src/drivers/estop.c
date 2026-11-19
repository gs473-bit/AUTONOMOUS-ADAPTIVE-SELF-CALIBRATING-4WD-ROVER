#include "drivers/estop.h"
void estop_init(estop_t *e){if(e)*e=(estop_t){.latched=1u};}
void estop_sample(estop_t *e,int closed,uint32_t now){
 if(!e)return;
 if(!closed){e->closed=0u;e->latched=1u;e->closed_since_ms=now;return;}
 if(!e->closed){e->closed=1u;e->closed_since_ms=now;}
}
int estop_rearm(estop_t *e,uint32_t now){
 if(!e || !e->closed || (uint32_t)(now-e->closed_since_ms)<20u)return 0;
 e->latched=0u;return 1;
}
