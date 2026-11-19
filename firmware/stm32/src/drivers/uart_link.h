#ifndef AETHER_UART_LINK_H
#define AETHER_UART_LINK_H
#include "protocol/packet.h"
#include <stdint.h>
#define AETHER_UART_RING 256u
#define AETHER_UART_TXQ 4u
typedef struct {
 packet_parser_t parser;
 uint8_t rx[AETHER_UART_RING];uint16_t rx_head,rx_tail;
 uint8_t tx[AETHER_UART_TXQ][PROTO_MAX_FRAME];uint16_t tx_size[AETHER_UART_TXQ];
 uint8_t tx_head,tx_tail,tx_count;
 uint32_t rx_overflow,tx_dropped;
} uart_link_t;
void uart_link_init(uart_link_t *l);
void uart_link_rx_isr(uart_link_t *l,uint8_t byte);
int uart_link_poll(uart_link_t *l,packet_t *packet);
int uart_link_queue(uart_link_t *l,const packet_t *packet);
int uart_link_next_tx(uart_link_t *l,const uint8_t **bytes,uint16_t *length);
void uart_link_tx_complete(uart_link_t *l);
#endif
