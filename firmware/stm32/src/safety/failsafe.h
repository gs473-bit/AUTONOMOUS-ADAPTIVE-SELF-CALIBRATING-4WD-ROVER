#ifndef AETHER_FAILSAFE_H
#define AETHER_FAILSAFE_H
#include <stdint.h>
typedef enum {STATE_BOOT,STATE_SELF_TEST,STATE_DISARMED,STATE_ARMED,STATE_RUNNING,STATE_FAULT,STATE_ESTOP} rover_state_t;
typedef struct {
 rover_state_t state;uint32_t last_valid_ms,last_drive_ms;uint32_t fault_flags;
 uint8_t selftest_pass,hardware_interlock,estop_released,estop_latched,heartbeat_seen,outputs_enabled;
 uint8_t rc_active,rc_signal_valid;
} failsafe_t;
void failsafe_init(failsafe_t *s);
void failsafe_begin_selftest(failsafe_t *s);
void failsafe_selftest_complete(failsafe_t *s,int passed);
void failsafe_heartbeat(failsafe_t *s,uint32_t ms);
int failsafe_arm(failsafe_t *s,uint32_t ms);
int failsafe_drive(failsafe_t *s,uint32_t ms);
void failsafe_disarm(failsafe_t *s);
void failsafe_estop(failsafe_t *s,int circuit_open);
void failsafe_fault(failsafe_t *s,uint32_t flags);
int failsafe_clear_estop(failsafe_t *s);
void failsafe_tick(failsafe_t *s,uint32_t now);
#endif
