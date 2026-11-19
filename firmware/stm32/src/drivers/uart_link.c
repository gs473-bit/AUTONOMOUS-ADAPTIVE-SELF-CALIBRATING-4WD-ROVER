#include "drivers/uart_link.h"
#include <string.h>
void uart_link_init(uart_link_t *l){if(l){memset(l,0,sizeof(*l));packet_parser_init(&l->parser);}}
void uart_link_rx_isr(uart_link_t *l,uint8_t b){
 if(!l)return;
 const uint16_t next=(uint16_t)((l->rx_head+1u)%AETHER_UART_RING);
 if(next==l->rx_tail){l->rx_overflow++;return;}
 l->rx[l->rx_head]=b;l->rx_head=next;
}
int uart_link_poll(uart_link_t *l,packet_t *packet){
 if(!l || !packet)return -1;
 while(l->rx_tail!=l->rx_head){
  const uint8_t b=l->rx[l->rx_tail];l->rx_tail=(uint16_t)((l->rx_tail+1u)%AETHER_UART_RING);
  const int rc=packet_parser_feed(&l->parser,b,packet);
  if(rc!=0)return rc;
 }
 return 0;
}
int uart_link_queue(uart_link_t *l,const packet_t *p){
 if(!l || !p || l->tx_count>=AETHER_UART_TXQ){if(l)l->tx_dropped++;return 0;}
 size_t n=0;const uint8_t slot=l->tx_head;
 if(!packet_encode(p,l->tx[slot],PROTO_MAX_FRAME,&n))return 0;
 l->tx_size[slot]=(uint16_t)n;l->tx_head=(uint8_t)((slot+1u)%AETHER_UART_TXQ);
 l->tx_count++;return 1;
}
int uart_link_next_tx(uart_link_t *l,const uint8_t **b,uint16_t *n){
 if(!l || !b || !n || l->tx_count==0u)return 0;
 *b=l->tx[l->tx_tail];*n=l->tx_size[l->tx_tail];return 1;
}
void uart_link_tx_complete(uart_link_t *l){
 if(!l || !l->tx_count)return;
 l->tx_tail=(uint8_t)((l->tx_tail+1u)%AETHER_UART_TXQ);l->tx_count--;
}
