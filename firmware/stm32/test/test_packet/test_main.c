#include "unity.h"
#include "../../src/protocol/packet.c"
#include <string.h>
void setUp(void) {}
void tearDown(void) {}
void test_crc_reference(void) {TEST_ASSERT_EQUAL_HEX16(0x29B1u,packet_crc16((const uint8_t *)"123456789",9));}
void test_roundtrip_and_partial(void) {
 packet_t a={0}; a.sequence=22; a.type=PKT_DRIVE; a.length=3;
 a.payload[0]=4; a.payload[1]=5; a.payload[2]=6;
 uint8_t bytes[PROTO_MAX_FRAME];size_t n=0; TEST_ASSERT_TRUE(packet_encode(&a,bytes,sizeof(bytes),&n));
 packet_parser_t p;packet_parser_init(&p);packet_t r={0};
 for(size_t i=0;i<n-1;i++) TEST_ASSERT_EQUAL_INT(0,packet_parser_feed(&p,bytes[i],&r));
 TEST_ASSERT_EQUAL_INT(1,packet_parser_feed(&p,bytes[n-1],&r));
 TEST_ASSERT_EQUAL_UINT16(22,r.sequence);TEST_ASSERT_EQUAL_UINT8(6,r.payload[2]);
}
void test_crc_error(void){
 packet_t a={0};uint8_t bytes[PROTO_MAX_FRAME];size_t n=0;
 TEST_ASSERT_TRUE(packet_encode(&a,bytes,sizeof(bytes),&n));bytes[n-1]^=1u;
 packet_parser_t p;packet_parser_init(&p);packet_t r={0};
 for(size_t i=0;i<n;i++) (void)packet_parser_feed(&p,bytes[i],&r);
 TEST_ASSERT_EQUAL_UINT32(1,p.bad_crc);
}
void test_gap_rejected(void){
 packet_t a={0};uint8_t b[PROTO_MAX_FRAME];size_t n=0;packet_parser_t p;
 packet_parser_init(&p);packet_t r={0};
 TEST_ASSERT_TRUE(packet_encode(&a,b,sizeof(b),&n));
 for(size_t i=0;i<n;i++) (void)packet_parser_feed(&p,b[i],&r);
 a.sequence=2;a.type=PKT_DRIVE;TEST_ASSERT_TRUE(packet_encode(&a,b,sizeof(b),&n));
 int result=0;for(size_t i=0;i<n;i++) result=packet_parser_feed(&p,b[i],&r);
 TEST_ASSERT_EQUAL_INT(-2,result);TEST_ASSERT_EQUAL_UINT32(1,p.bad_sequence);
}
void test_garbage_prefix(void){
 packet_t a={0};uint8_t b[PROTO_MAX_FRAME];size_t n=0;packet_parser_t p;packet_parser_init(&p);packet_t r={0};
 for(size_t i=0;i<7;i++) (void)packet_parser_feed(&p,0x42u,&r);
 TEST_ASSERT_TRUE(packet_encode(&a,b,sizeof(b),&n));
 int result=0;for(size_t i=0;i<n;i++) result=packet_parser_feed(&p,b[i],&r);
 TEST_ASSERT_EQUAL_INT(1,result);TEST_ASSERT_TRUE(p.noise>=7u);
}
int main(void){UNITY_BEGIN();RUN_TEST(test_crc_reference);RUN_TEST(test_roundtrip_and_partial);RUN_TEST(test_crc_error);RUN_TEST(test_gap_rejected);RUN_TEST(test_garbage_prefix);return UNITY_END();}
