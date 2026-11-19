#include "unity.h"
#include "../../src/drivers/ibus_rx.c"
void setUp(void){}
void tearDown(void){}
static void frame(uint8_t f[32]){
 f[0]=0x20;f[1]=0x40;
 for(unsigned i=0;i<14u;i++){f[2u+2u*i]=0xdcu;f[3u+2u*i]=0x05u;}
 unsigned s=0u;for(unsigned i=0;i<30u;i++)s+=f[i];
 const uint16_t crc=(uint16_t)(0xffffu-s);f[30]=(uint8_t)crc;f[31]=(uint8_t)(crc>>8u);
}
void test_ibus_valid_and_timeout(void){
 ibus_rx_t r;ibus_rx_init(&r);uint8_t f[32];frame(f);
 for(unsigned i=0;i<31u;i++)TEST_ASSERT_FALSE(ibus_rx_byte(&r,f[i],1u));
 TEST_ASSERT_TRUE(ibus_rx_byte(&r,f[31],1u));TEST_ASSERT_EQUAL_UINT16(1500,r.channel[0]);
 TEST_ASSERT_FALSE(ibus_rx_lost(&r,100));TEST_ASSERT_TRUE(ibus_rx_lost(&r,102));
}
void test_ibus_bad_checksum(void){
 ibus_rx_t r;ibus_rx_init(&r);uint8_t f[32];frame(f);f[31]^=1u;
 for(unsigned i=0;i<32u;i++)(void)ibus_rx_byte(&r,f[i],1);
 TEST_ASSERT_EQUAL_UINT32(1,r.bad_checksum);
}
int main(void){UNITY_BEGIN();RUN_TEST(test_ibus_valid_and_timeout);RUN_TEST(test_ibus_bad_checksum);return UNITY_END();}
