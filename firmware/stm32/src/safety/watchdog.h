#ifndef AETHER_WATCHDOG_H
#define AETHER_WATCHDOG_H
#include <stdint.h>
typedef struct {uint32_t last_packet_ms,last_loop_ms,timeout_ms;uint8_t seen_valid,loop_seen;} watchdog_t;
void watchdog_init(watchdog_t *w,uint32_t timeout_ms);
void watchdog_valid_packet(watchdog_t *w,uint32_t now_ms);
int watchdog_expired(const watchdog_t *w,uint32_t now_ms);
int watchdog_healthy_loop(watchdog_t *w,uint32_t now_ms);
#endif
