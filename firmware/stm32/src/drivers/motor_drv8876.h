#ifndef AETHER_MOTOR_DRV8876_H
#define AETHER_MOTOR_DRV8876_H
#include <stdint.h>
typedef enum { MOTOR_COAST=0, MOTOR_FORWARD=1, MOTOR_REVERSE=2, MOTOR_BRAKE_UNVERIFIED=3 } motor_mode_t;
typedef struct {
  void (*pwm)(unsigned,float); void (*phase)(unsigned,int);
  void (*sleep)(unsigned,int);
} motor_output_t;
typedef struct {float duty,amps,overcurrent_a;uint8_t active,fault;} motor_drv8876_t;
void motor_drv8876_init(motor_drv8876_t *driver);
int motor_drv8876_apply(motor_drv8876_t *d,unsigned wheel,float duty,const motor_output_t *out,int interlock_verified);
void motor_drv8876_set_fault(motor_drv8876_t *d,int fault_active);
void motor_drv8876_update_current(motor_drv8876_t *d,float amps);
void motor_drv8876_disable(motor_drv8876_t *d,unsigned wheel,const motor_output_t *out);
#endif
