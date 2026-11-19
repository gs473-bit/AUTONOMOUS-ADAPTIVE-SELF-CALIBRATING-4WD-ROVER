#include "unity.h"
#include "../../src/drivers/encoder.c"
void setUp(void){}
void tearDown(void){}
void test_wrap_positive_and_negative(void){
 TEST_ASSERT_EQUAL_INT32(1,encoder_delta(0,65535,16));
 TEST_ASSERT_EQUAL_INT32(-1,encoder_delta(65535,0,16));
 TEST_ASSERT_EQUAL_INT32(1,encoder_delta(0,0xffffffffu,32));
}
void test_tick_speed_and_unmeasured_cpr(void){
 encoder_t e;encoder_init(&e,16,400);
 TEST_ASSERT_FALSE(encoder_sample(&e,65530,.01f));
 TEST_ASSERT_TRUE(encoder_sample(&e,10,.01f));
 TEST_ASSERT_EQUAL_INT64(16,e.total_ticks);TEST_ASSERT_TRUE(e.valid);
 encoder_init(&e,16,0);encoder_sample(&e,0,.01f);
 TEST_ASSERT_FALSE(encoder_sample(&e,1,.01f));
}
int main(void){UNITY_BEGIN();RUN_TEST(test_wrap_positive_and_negative);RUN_TEST(test_tick_speed_and_unmeasured_cpr);return UNITY_END();}
