#include "unity.h"
#include "../../src/protocol/packet.c"
#include "../../src/drivers/uart_link.c"
void setUp(void){}
void tearDown(void){}
void test_rx_partial_noise(void){
 uart_link_t l;uart_link_init(&l);packet_t in={0},out={0};in.type=PKT_HEARTBEAT;in.sequence=9;
 uint8_t b[PROTO_MAX_FRAME];size_t n=0;TEST_ASSERT_TRUE(packet_encode(&in,b,sizeof(b),&n));
 uart_link_rx_isr(&l,0x70u);for(size_t i=0;i<n/2;i++)uart_link_rx_isr(&l,b[i]);
 TEST_ASSERT_EQUAL_INT(0,uart_link_poll(&l,&out));
 for(size_t i=n/2;i<n;i++)uart_link_rx_isr(&l,b[i]);
 TEST_ASSERT_EQUAL_INT(1,uart_link_poll(&l,&out));TEST_ASSERT_EQUAL_UINT16(9,out.sequence);
}
void test_tx_ring_capacity(void){
 uart_link_t l;uart_link_init(&l);packet_t p={0};
 for(unsigned i=0;i<AETHER_UART_TXQ;i++)TEST_ASSERT_TRUE(uart_link_queue(&l,&p));
 TEST_ASSERT_FALSE(uart_link_queue(&l,&p));
 const uint8_t *buf=0;uint16_t n=0;TEST_ASSERT_TRUE(uart_link_next_tx(&l,&buf,&n));TEST_ASSERT_TRUE(n>0);
 uart_link_tx_complete(&l);TEST_ASSERT_TRUE(uart_link_queue(&l,&p));
}
int main(void){UNITY_BEGIN();RUN_TEST(test_rx_partial_noise);RUN_TEST(test_tx_ring_capacity);return UNITY_END();}
