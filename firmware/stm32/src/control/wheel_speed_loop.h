#ifndef AETHER_WHEEL_LOOP_H
#define AETHER_WHEEL_LOOP_H
#include "control/pid.h"
typedef struct {pid_t controller[4];float reference[4];float previous_duty[4];float ramp_per_s;float max_abs_rad_s;} wheel_loop_t;
void wheel_loop_init(wheel_loop_t *loop);
int wheel_loop_set(wheel_loop_t *loop,const float reference[4]);
void wheel_loop_tick(wheel_loop_t *loop,const float measured[4],float dt,float duty[4]);
#endif
