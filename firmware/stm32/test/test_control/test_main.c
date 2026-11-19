#include "unity.h"
#include "../../src/control/pid.c"
#include "../../src/control/mecanum_kinematics.c"
#include "../../src/control/ramp_limiter.c"
#include <math.h>
void setUp(void) {}
void tearDown(void) {}
void test_kinematics_forward(void){
 mecanum_geometry_t g={.wheel_radius_m=.04f,.wheelbase_m=.20f,.track_width_m=.18f,.max_wheel_rad_s=10.0f};float w[4];
 TEST_ASSERT_TRUE(mecanum_inverse(&g,.08f,0,0,w));
 for(unsigned i=0;i<4u;i++)TEST_ASSERT_FLOAT_WITHIN(.0001f,2.0f,w[i]);
}
void test_kinematics_strafe_rotate_combined(void){
 mecanum_geometry_t g={.04f,.20f,.18f,10.0f};float w[4];
 TEST_ASSERT_TRUE(mecanum_inverse(&g,0,.08f,0,w));
 TEST_ASSERT_FLOAT_WITHIN(.0001f,-2.0f,w[0]);TEST_ASSERT_FLOAT_WITHIN(.0001f,2.0f,w[1]);
 TEST_ASSERT_TRUE(mecanum_inverse(&g,0,0,.4f,w));
 TEST_ASSERT_FLOAT_WITHIN(.0001f,-1.9f,w[0]);TEST_ASSERT_FLOAT_WITHIN(.0001f,1.9f,w[1]);
 TEST_ASSERT_TRUE(mecanum_inverse(&g,.04f,.02f,.1f,w));
 TEST_ASSERT_FALSE(mecanum_inverse(&g,2.0f,0,0,w));
}
void test_pid_step_windup(void){
 pid_t p;pid_init(&p,.2f,8.0f,0,0,-1,1);
 for(unsigned i=0;i<500u;i++){float y=pid_step(&p,100,0,.01f);TEST_ASSERT_TRUE(y<=1.0f && y>=-1.0f);}
 TEST_ASSERT_TRUE(p.integral<=1.0f);
 pid_reset(&p);TEST_ASSERT_EQUAL_UINT(0u,p.initialized);
}
void test_ramp_limit(void){
 TEST_ASSERT_FLOAT_WITHIN(.001f,.1f,ramp_step(0,1,1,.1f));
 TEST_ASSERT_FLOAT_WITHIN(.001f,0.0f,ramp_step(.2f,0,2,.1f));
}
int main(void){UNITY_BEGIN();RUN_TEST(test_kinematics_forward);RUN_TEST(test_kinematics_strafe_rotate_combined);RUN_TEST(test_pid_step_windup);RUN_TEST(test_ramp_limit);return UNITY_END();}
