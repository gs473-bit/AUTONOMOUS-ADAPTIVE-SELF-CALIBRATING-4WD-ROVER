#include "drivers/ibus_rx.h"
#include "config.h"
#include <string.h>
void ibus_rx_init(ibus_rx_t *r){if(r)memset(r,0,sizeof(*r));}
int ibus_rx_byte(ibus_rx_t *r,uint8_t b,uint32_t now){
 if(!r)return 0;
 if(r->used==0u && b!=0x20u){r->framing++;return 0;}
 if(r->used==1u && b!=0x40u){r->used=0u;r->framing++;return 0;}
 r->frame[r->used++]=b;
 if(r->used<32u)return 0;
 r->used=0u;
 unsigned sum=0u;
 for(unsigned i=0;i<30u;i++)sum+=r->frame[i];
 const uint16_t checksum=(uint16_t)(0xffffu-sum);
 const uint16_t received=(uint16_t)(r->frame[30]|((uint16_t)r->frame[31]<<8u));
 if(checksum!=received){r->bad_checksum++;return 0;}
 for(unsigned i=0;i<14u;i++){
  const uint16_t value=(uint16_t)(r->frame[2u+i*2u]|((uint16_t)r->frame[3u+i*2u]<<8u));
  if(value<900u || value>2100u){r->framing++;return 0;}
  r->channel[i]=value;
 }
 r->last_frame_ms=now;r->valid=1u;return 1;
}
int ibus_rx_lost(const ibus_rx_t *r,uint32_t now){
 return !r || !r->valid || (uint32_t)(now-r->last_frame_ms)>AETHER_RC_TIMEOUT_MS;
}
