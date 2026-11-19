#include "unity.h"
#include "../../src/drivers/adc_sense.c"
void setUp(void){}
void tearDown(void){}
void test_reject_unconfigured_divider(void){adc_sense_t s;adc_sense_init(&s);const float v[5]={0};TEST_ASSERT_FALSE(adc_sense_update(&s,v));}
void test_filtered_battery_flags(void){adc_sense_t s;adc_sense_init(&s);s.ipropri_v_per_a=2.49f;s.divider_ratio=5;s.low_v=10.0f;s.critical_v=9.0f;const float v[5]={0,0,0,0,1.8f};TEST_ASSERT_TRUE(adc_sense_update(&s,v));TEST_ASSERT_TRUE(s.critical);}
int main(void){UNITY_BEGIN();RUN_TEST(test_reject_unconfigured_divider);RUN_TEST(test_filtered_battery_flags);return UNITY_END();}
