#ifndef AETHER_PID_H
#define AETHER_PID_H
typedef struct {float kp,ki,kd,kff,kaw,out_min,out_max,integral,prev;unsigned initialized;} pid_t;
void pid_init(pid_t *p,float kp,float ki,float kd,float kff,float min_out,float max_out);
float pid_step(pid_t *p,float reference,float measured,float dt);
void pid_reset(pid_t *p);
#endif
