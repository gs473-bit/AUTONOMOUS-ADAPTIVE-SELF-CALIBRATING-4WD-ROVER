#include "control/wheel_speed_loop.h"
#include "control/ramp_limiter.h"
#include <math.h>
void wheel_loop_init(wheel_loop_t *l) {
    for(unsigned i=0;i<4u;i++){pid_init(&l->controller[i],0.0f,0.0f,0.0f,0.0f,-1.0f,1.0f);l->reference[i]=l->previous_duty[i]=0.0f;}
    l->ramp_per_s=0.0f;l->max_abs_rad_s=0.0f; /* TODO tuning required */
}
int wheel_loop_set(wheel_loop_t *l,const float ref[4]){
    if (!l || !ref || l->max_abs_rad_s<=0.0f) return 0;
    for(unsigned i=0;i<4u;i++)if(!isfinite(ref[i]) || fabsf(ref[i])>l->max_abs_rad_s) return 0;
    for(unsigned i=0;i<4u;i++)l->reference[i]=ref[i];
    return 1;
}
void wheel_loop_tick(wheel_loop_t *l,const float actual[4],float dt,float duty[4]){
    if(!l || !actual || !duty)return;
    for(unsigned i=0;i<4u;i++){
        float desired=pid_step(&l->controller[i],l->reference[i],actual[i],dt);
        duty[i]=ramp_step(l->previous_duty[i],desired,l->ramp_per_s,dt);
        l->previous_duty[i]=duty[i];
    }
}
