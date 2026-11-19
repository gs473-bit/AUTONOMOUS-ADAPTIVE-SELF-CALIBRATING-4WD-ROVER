#include "drivers/motor_drv8876.h"
#include "config.h"
#include <math.h>
void motor_drv8876_init(motor_drv8876_t *d) {if(d)*d=(motor_drv8876_t){0};}
void motor_drv8876_disable(motor_drv8876_t *d,unsigned wheel,const motor_output_t *io){
 if(!d)return;
 d->active=0u;d->duty=0.0f;
 if(io){if(io->pwm)io->pwm(wheel,0.0f);if(io->sleep)io->sleep(wheel,0);}
}
void motor_drv8876_set_fault(motor_drv8876_t *d,int fault){if(d)d->fault=(uint8_t)!!fault;}
void motor_drv8876_update_current(motor_drv8876_t *d,float amps){
 if(!d)return;
 if(!isfinite(amps) || amps<0.0f){d->fault=1u;return;}
 d->amps=amps;
 if(d->overcurrent_a>0.0f && amps>d->overcurrent_a)d->fault=1u;
}
int motor_drv8876_apply(motor_drv8876_t *d,unsigned wheel,float duty,const motor_output_t *io,int interlock_verified){
 if(!d || !io || !io->pwm || !io->phase || !io->sleep || !isfinite(duty) ||
    duty < -1.0f || duty>1.0f || wheel>=AETHER_WHEELS || d->fault ||
    !interlock_verified || !AETHER_ACTUATION_ENABLED || d->overcurrent_a<=0.0f){
   motor_drv8876_disable(d,wheel,io);return 0;
 }
 if(fabsf(duty)<0.03f){motor_drv8876_disable(d,wheel,io);return 1;}
 io->pwm(wheel,0.0f); /* avoid drive sign reversal at nonzero PWM */
 io->phase(wheel,duty>=0.0f ? 1:0);
 io->sleep(wheel,1);
 io->pwm(wheel,fabsf(duty));
 d->duty=duty;d->active=1u;return 1;
}
