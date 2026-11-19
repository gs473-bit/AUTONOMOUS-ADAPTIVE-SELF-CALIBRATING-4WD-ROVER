#include "unity.h"
#include "../../src/drivers/motor_drv8876.c"
static float last_pwm;static int last_sleep;
static void pwm(unsigned wheel,float d){(void)wheel;last_pwm=d;}
static void phase(unsigned wheel,int value){(void)wheel;(void)value;}
static void sleep_pin(unsigned wheel,int value){(void)wheel;last_sleep=value;}
void setUp(void){last_pwm=0;last_sleep=0;}
void tearDown(void){}
void test_default_interlock_disables_all_drive(void){
 motor_drv8876_t m;motor_drv8876_init(&m);m.overcurrent_a=1.0f;
 motor_output_t io={pwm,phase,sleep_pin};
 TEST_ASSERT_FALSE(motor_drv8876_apply(&m,0u,.5f,&io,1));
 TEST_ASSERT_FALSE(m.active);TEST_ASSERT_EQUAL_INT(0,last_sleep);TEST_ASSERT_FLOAT_WITHIN(.001f,0,last_pwm);
}
void test_overcurrent_fault_latches(void){
 motor_drv8876_t m;motor_drv8876_init(&m);m.overcurrent_a=1.0f;
 motor_drv8876_update_current(&m,1.2f);TEST_ASSERT_TRUE(m.fault);
}
int main(void){UNITY_BEGIN();RUN_TEST(test_default_interlock_disables_all_drive);RUN_TEST(test_overcurrent_fault_latches);return UNITY_END();}
