#include "unity.h"
#include "../../src/drivers/estop.c"
void setUp(void){}
void tearDown(void){}
void test_open_wire_immediate_latch(void){estop_t e;estop_init(&e);estop_sample(&e,1,10);TEST_ASSERT_FALSE(estop_rearm(&e,15));TEST_ASSERT_TRUE(estop_rearm(&e,31));estop_sample(&e,0,32);TEST_ASSERT_TRUE(e.latched);}
void test_no_rearm_without_release(void){estop_t e;estop_init(&e);estop_sample(&e,0,100);TEST_ASSERT_FALSE(estop_rearm(&e,200));}
int main(void){UNITY_BEGIN();RUN_TEST(test_open_wire_immediate_latch);RUN_TEST(test_no_rearm_without_release);return UNITY_END();}
