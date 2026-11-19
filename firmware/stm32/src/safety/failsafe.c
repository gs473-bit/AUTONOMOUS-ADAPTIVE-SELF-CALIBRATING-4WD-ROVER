#include "safety/failsafe.h"
#include "config.h"
void failsafe_init(failsafe_t *s){if(s)*s=(failsafe_t){.state=STATE_BOOT,.estop_latched=1u};}
void failsafe_begin_selftest(failsafe_t *s){if(s && s->state==STATE_BOOT)s->state=STATE_SELF_TEST;}
void failsafe_selftest_complete(failsafe_t *s,int passed){
 if(!s || s->state!=STATE_SELF_TEST)return;
 s->selftest_pass=(uint8_t)!!passed;
 s->state=passed?STATE_DISARMED:STATE_FAULT;s->outputs_enabled=0u;
}
void failsafe_heartbeat(failsafe_t *s,uint32_t now){if(s){s->last_valid_ms=now;s->heartbeat_seen=1u;}}
int failsafe_arm(failsafe_t *s,uint32_t now){
 if(!s || s->state!=STATE_DISARMED || !s->selftest_pass || !s->hardware_interlock ||
    s->estop_latched || !s->estop_released || s->fault_flags || !s->heartbeat_seen ||
    (uint32_t)(now-s->last_valid_ms)>=AETHER_TIMEOUT_MS)return 0;
 s->state=STATE_ARMED;s->outputs_enabled=0u;return 1;
}
int failsafe_drive(failsafe_t *s,uint32_t now){
 if(!s || (s->state!=STATE_ARMED && s->state!=STATE_RUNNING) ||
    (uint32_t)(now-s->last_valid_ms)>=AETHER_TIMEOUT_MS || (s->rc_active && !s->rc_signal_valid))return 0;
 s->state=STATE_RUNNING;s->outputs_enabled=1u;s->last_drive_ms=now;s->last_valid_ms=now;return 1;
}
void failsafe_disarm(failsafe_t *s){
 if(!s)return;
 s->outputs_enabled=0u;
 if(s->state==STATE_ARMED || s->state==STATE_RUNNING)s->state=STATE_DISARMED;
}
void failsafe_estop(failsafe_t *s,int circuit_open){
 if(!s)return;
 s->estop_released=(uint8_t)!circuit_open;
 if(circuit_open){s->estop_latched=1u;s->state=STATE_ESTOP;s->outputs_enabled=0u;}
}
void failsafe_fault(failsafe_t *s,uint32_t flags){
 if(!s || !flags)return;
 s->fault_flags|=flags;s->outputs_enabled=0u;
 if(s->state!=STATE_ESTOP)s->state=STATE_FAULT;
}
void failsafe_tick(failsafe_t *s,uint32_t now){
 if(!s)return;
 if(s->estop_latched && s->state!=STATE_BOOT && s->state!=STATE_SELF_TEST){s->state=STATE_ESTOP;s->outputs_enabled=0u;return;}
 if(s->fault_flags){failsafe_fault(s,s->fault_flags);return;}
 if((s->state==STATE_ARMED || s->state==STATE_RUNNING) &&
    (!s->heartbeat_seen || (uint32_t)(now-s->last_valid_ms)>=AETHER_TIMEOUT_MS ||
     (s->state==STATE_RUNNING && (uint32_t)(now-s->last_drive_ms)>=AETHER_TIMEOUT_MS) ||
     (s->rc_active && !s->rc_signal_valid))) failsafe_disarm(s);
}
