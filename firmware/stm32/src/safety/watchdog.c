#include "safety/watchdog.h"
void watchdog_init(watchdog_t *w,uint32_t timeout){if(w)*w=(watchdog_t){.timeout_ms=timeout};}
void watchdog_valid_packet(watchdog_t *w,uint32_t ms){if(w){w->last_packet_ms=ms;w->seen_valid=1u;}}
int watchdog_expired(const watchdog_t *w,uint32_t ms){
 return !w || !w->seen_valid || w->timeout_ms==0u || (uint32_t)(ms-w->last_packet_ms)>=w->timeout_ms;
}
int watchdog_healthy_loop(watchdog_t *w,uint32_t now){
 if(!w)return 0;
 if(w->loop_seen && (uint32_t)(now-w->last_loop_ms)>50u)return 0;
 w->loop_seen=1u;w->last_loop_ms=now;return 1;
}
