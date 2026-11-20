#include "unity.h"
#include "../../src/safety/failsafe.c"
#include "../../src/safety/fault_manager.c"
#include "../../src/safety/watchdog.c"
void setUp(void){}
void tearDown(void){}
static void ready(failsafe_t *s){
 failsafe_init(s);failsafe_begin_selftest(s);failsafe_selftest_complete(s,1);
 s->estop_latched=0u;s->estop_released=1u;s->hardware_interlock=1u;
 failsafe_heartbeat(s,100u);
}
void test_boot_and_explicit_arm(void){
 failsafe_t s;failsafe_init(&s);TEST_ASSERT_EQUAL(STATE_BOOT,s.state);
 failsafe_begin_selftest(&s);TEST_ASSERT_EQUAL(STATE_SELF_TEST,s.state);
 failsafe_selftest_complete(&s,1);TEST_ASSERT_EQUAL(STATE_DISARMED,s.state);
 TEST_ASSERT_FALSE(failsafe_arm(&s,100));s.estop_released=1u;s.estop_latched=0u;s.hardware_interlock=1u;
 failsafe_heartbeat(&s,100);TEST_ASSERT_TRUE(failsafe_arm(&s,100));
 TEST_ASSERT_TRUE(failsafe_drive(&s,101));TEST_ASSERT_TRUE(s.outputs_enabled);
}
void test_link_timeout_disables(void){failsafe_t s;ready(&s);failsafe_arm(&s,100);failsafe_drive(&s,101);
 failsafe_tick(&s,301);TEST_ASSERT_EQUAL(STATE_DISARMED,s.state);TEST_ASSERT_FALSE(s.outputs_enabled);}
void test_heartbeat_cannot_extend_stale_drive(void){failsafe_t s;ready(&s);failsafe_arm(&s,100);failsafe_drive(&s,101);failsafe_heartbeat(&s,250);failsafe_tick(&s,301);TEST_ASSERT_EQUAL(STATE_DISARMED,s.state);}
void test_estop_fault_priority(void){failsafe_t s;ready(&s);failsafe_arm(&s,100);failsafe_drive(&s,101);
 failsafe_fault(&s,1u);TEST_ASSERT_EQUAL(STATE_FAULT,s.state);
 failsafe_estop(&s,1);TEST_ASSERT_EQUAL(STATE_ESTOP,s.state);
 failsafe_tick(&s,130);TEST_ASSERT_EQUAL(STATE_ESTOP,s.state);}
void test_estop_release_requires_explicit_clear(void){
 failsafe_t s;ready(&s);failsafe_estop(&s,1);
 TEST_ASSERT_FALSE(failsafe_clear_estop(&s));
 failsafe_estop(&s,0);TEST_ASSERT_TRUE(s.estop_latched);
 TEST_ASSERT_TRUE(failsafe_clear_estop(&s));
 TEST_ASSERT_EQUAL(STATE_DISARMED,s.state);
}
void test_disarm_after_manual_stop(void){
 failsafe_t s;ready(&s);failsafe_arm(&s,100);
 TEST_ASSERT_TRUE(failsafe_drive(&s,101));
 failsafe_disarm(&s);
 TEST_ASSERT_EQUAL(STATE_DISARMED,s.state);
 TEST_ASSERT_FALSE(s.outputs_enabled);
 TEST_ASSERT_FALSE(failsafe_drive(&s,102));
}
void test_watchdog_and_fault_ring(void){watchdog_t w;watchdog_init(&w,200);
 TEST_ASSERT_TRUE(watchdog_expired(&w,0));watchdog_valid_packet(&w,50);
 TEST_ASSERT_FALSE(watchdog_expired(&w,249));TEST_ASSERT_TRUE(watchdog_expired(&w,250));
 fault_manager_t f;faults_init(&f);faults_report(&f,FAULT_DRIVER,42);
 fault_entry_t e;TEST_ASSERT_TRUE(faults_get_recent(&f,0,&e));TEST_ASSERT_EQUAL_UINT32(FAULT_DRIVER,e.flags);}
int main(void){UNITY_BEGIN();RUN_TEST(test_boot_and_explicit_arm);RUN_TEST(test_link_timeout_disables);RUN_TEST(test_heartbeat_cannot_extend_stale_drive);RUN_TEST(test_estop_fault_priority);RUN_TEST(test_estop_release_requires_explicit_clear);RUN_TEST(test_disarm_after_manual_stop);RUN_TEST(test_watchdog_and_fault_ring);return UNITY_END();}
