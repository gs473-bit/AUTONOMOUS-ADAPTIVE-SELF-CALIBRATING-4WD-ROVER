#include "control/pid.h"
#include <math.h>
void pid_init(pid_t *p,float kp,float ki,float kd,float kff,float lo,float hi) {
    *p=(pid_t){.kp=kp,.ki=ki,.kd=kd,.kff=kff,.kaw=0.5f,.out_min=lo,.out_max=hi};
}
void pid_reset(pid_t *p){p->integral=0.0f;p->prev=0.0f;p->initialized=0u;}
float pid_step(pid_t *p,float ref,float measurement,float dt) {
    if (!p || !isfinite(ref) || !isfinite(measurement) || !isfinite(dt) || dt<=0.0f || p->out_max<=p->out_min) return 0.0f;
    const float err=ref-measurement;
    const float derivative=p->initialized ? -(measurement-p->prev)/dt : 0.0f;
    const float raw=p->kp*err + p->integral + p->kd*derivative + p->kff*ref;
    const float out=raw>p->out_max ? p->out_max : (raw<p->out_min ? p->out_min : raw);
    p->integral += (p->ki*err + p->kaw*(out-raw))*dt;
    if (p->integral>p->out_max) p->integral=p->out_max;
    if (p->integral<p->out_min) p->integral=p->out_min;
    p->prev=measurement;p->initialized=1u;return out;
}
