#ifndef AETHER_IBUS_RX_H
#define AETHER_IBUS_RX_H
#include <stdint.h>
typedef struct {uint8_t frame[32],used,valid;uint16_t channel[14];uint32_t last_frame_ms,bad_checksum,framing;} ibus_rx_t;
void ibus_rx_init(ibus_rx_t *r);
int ibus_rx_byte(ibus_rx_t *r,uint8_t byte,uint32_t now_ms);
int ibus_rx_lost(const ibus_rx_t *r,uint32_t now_ms);
#endif
