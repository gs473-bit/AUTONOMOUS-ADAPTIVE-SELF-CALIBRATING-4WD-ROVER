#include "protocol/packet.h"
#include <string.h>
static uint16_t u16(const uint8_t *p) {return (uint16_t)(p[0] | (uint16_t)(p[1] << 8u));}
static void put16(uint8_t *p, uint16_t v) {p[0]=(uint8_t)v; p[1]=(uint8_t)(v>>8u);}
uint16_t packet_crc16(const uint8_t *data, size_t n) {
  uint16_t crc=0xFFFFu;
  for (size_t i=0; i<n; ++i) {
    crc^=(uint16_t)data[i]<<8u;
    for (unsigned bit=0; bit<8u; ++bit)
      crc=(crc&0x8000u) ? (uint16_t)((crc<<1u)^0x1021u) : (uint16_t)(crc<<1u);
  }
  return crc;
}
int packet_encode(const packet_t *packet, uint8_t *out, size_t cap, size_t *written) {
  if (!packet || !out || !written || packet->length > PROTO_MAX_PAYLOAD) return 0;
  const size_t n = 9u + packet->length;
  if (cap<n) return 0;
  out[0]=PROTO_SOF0; out[1]=PROTO_SOF1; put16(&out[2],packet->length);
  put16(&out[4],packet->sequence);out[6]=packet->type;
  memcpy(&out[7],packet->payload,packet->length);
  put16(&out[n-2],packet_crc16(&out[2],5u+packet->length));
  *written=n; return 1;
}
void packet_parser_init(packet_parser_t *p) {if (p) memset(p,0,sizeof(*p));}
static void discard(packet_parser_t *p, size_t n) {
  if (n>p->used) n=p->used;
  memmove(p->data,p->data+n,p->used-n);p->used-=n;
}
int packet_parser_feed(packet_parser_t *p, uint8_t byte, packet_t *pkt) {
  if (!p || !pkt) return -1;
  if (p->used >= PROTO_MAX_FRAME) discard(p,1);
  p->data[p->used++]=byte;
  while (p->used>=2u) {
    if (p->data[0]!=PROTO_SOF0 || p->data[1]!=PROTO_SOF1) {
      discard(p,1); p->noise++; continue;
    }
    if (p->used<7u) return 0;
    const uint16_t len=u16(&p->data[2]);
    if (len>PROTO_MAX_PAYLOAD) {p->bad_length++;discard(p,1);return -1;}
    const size_t needed=9u+len;
    if (p->used<needed) return 0;
    const uint16_t expected_crc=packet_crc16(&p->data[2],5u+len);
    if (u16(&p->data[7u+len])!=expected_crc) {p->bad_crc++;discard(p,1);return -1;}
    pkt->length=len; pkt->sequence=u16(&p->data[4]);pkt->type=p->data[6];
    memcpy(pkt->payload,&p->data[7],len);
    discard(p,needed);
    if (p->synchronized && pkt->sequence!=p->expected) {
       p->bad_sequence++;
       /* Require resynchronisation by explicit DISARM/STOP; do not execute out-of-order DRIVE. */
       if (pkt->type!=PKT_STOP && pkt->type!=PKT_DISARM) return -2;
    }
    p->expected=(uint16_t)(pkt->sequence+1u); p->synchronized=1u;
    return 1;
  }
  return 0;
}
