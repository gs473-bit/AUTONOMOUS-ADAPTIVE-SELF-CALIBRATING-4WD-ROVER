#ifndef AETHER_UART_VCP_H
#define AETHER_UART_VCP_H
#include <stdint.h>
#include <stddef.h>
int uart_vcp_init(void);
size_t uart_vcp_read(uint8_t *bytes,size_t max_count);
int uart_vcp_write(const uint8_t *data,uint16_t size);
#endif
