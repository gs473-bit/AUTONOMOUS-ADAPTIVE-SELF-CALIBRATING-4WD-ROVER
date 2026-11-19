#include "app/controller.h"
#include "drivers/uart_link.h"
#include "safety/failsafe.h"
#include "safety/fault_manager.h"
#include "control/wheel_speed_loop.h"
#include "control/mecanum_kinematics.h"
#include "hal_port/uart_vcp.h"
#include "hal_port/actuation_hw.h"
#include "hal_port/adc_hw.h"
#include "hal_port/iwdg_hw.h"
#include "drivers/motor_drv8876.h"
#include "drivers/encoder.h"
#include "drivers/adc_sense.h"
#include "drivers/estop.h"
#include "safety/watchdog.h"
#include "pins.h"
#include "config.h"
#include <string.h>
#include <math.h>
static uart_link_t link;
static failsafe_t safety;
static fault_manager_t faults;
static wheel_loop_t loop;
static uint16_t txseq;
static uint32_t last_tick,tx_at;
static uint8_t serial_ok;
static uint8_t mode,hw_ready;
static motor_drv8876_t motors[4];
static encoder_t encoder[4];
static adc_sense_t adc;
static estop_t estop;
static watchdog_t loop_watchdog;
static uint32_t sample_at;
static void queue_ack(uint8_t type,uint16_t rxseq,uint8_t reason){
 packet_t ack={0};
 ack.sequence=txseq++;ack.type=type;ack.length=(uint16_t)(type==PKT_NACK?3:2);
 ack.payload[0]=(uint8_t)rxseq;ack.payload[1]=(uint8_t)(rxseq>>8u);
 if(type==PKT_NACK)ack.payload[2]=reason;
 (void)uart_link_queue(&link,&ack);
}
static float get_float(const uint8_t *p){
 float v;memcpy(&v,p,sizeof(v));return v;/* ARM Cortex-M4 little endian IEEE754 */
}
static void put_u32(uint8_t *b,uint32_t value){
 for(unsigned i=0;i<4u;i++)b[i]=(uint8_t)(value>>(8u*i));
}
static void put_float(uint8_t *b,float value){memcpy(b,&value,4u);}
static void report_fault_log(void){
 packet_t p={0};p.sequence=txseq++;p.type=PKT_FAULT_LOG;
 unsigned count=faults.count<15u?faults.count:15u;
 p.payload[0]=(uint8_t)count;p.length=(uint16_t)(1u+8u*count);
 for(unsigned i=0;i<count;i++){
  fault_entry_t entry;
  if(faults_get_recent(&faults,i,&entry)){
   put_u32(&p.payload[1u+8u*i],entry.at_ms);
   put_u32(&p.payload[5u+8u*i],entry.flags);
  }
 }
 (void)uart_link_queue(&link,&p);
}
static void status_packet(void){
 packet_t p={0};p.type=PKT_TELEMETRY;p.sequence=txseq++;
 p.length=59u;/* 4 wheel float + 4 tick int + battery float + 4 currents float
               * + state uint8 + fault uint32 + RC uint8 + valid mask uint8 */
 memset(p.payload,0,p.length);
 if(hw_ready && adc.valid){
  for(unsigned i=0;i<4u;i++){
    put_float(&p.payload[4u*i],encoder[i].rad_s);
    put_u32(&p.payload[16u+4u*i],(uint32_t)encoder[i].total_ticks);
    put_float(&p.payload[36u+4u*i],adc.currents_a[i]);
  }
  put_float(&p.payload[32],adc.battery_v);
  p.payload[58]=0x07u;/* wheel speed/ticks, battery, currents */
 }
 p.payload[52]=(uint8_t)safety.state;
 uint32_t f=faults.active_flags;
 for(unsigned i=0;i<4u;i++)p.payload[53+i]=(uint8_t)(f>>(8u*i));
 p.payload[57]=safety.rc_active? (safety.rc_signal_valid?1u:2u):0u;
 /* Validity mask remains zero unless ADC + encoder hardware was accepted. */
 (void)uart_link_queue(&link,&p);
}
static void process(const packet_t *p,uint32_t now){
 switch(p->type){
 case PKT_HEARTBEAT:
  if(p->length){queue_ack(PKT_NACK,p->sequence,2u);break;}
  failsafe_heartbeat(&safety,now);queue_ack(PKT_ACK,p->sequence,0u);break;
 case PKT_STOP:case PKT_DISARM:
  if(p->length){queue_ack(PKT_NACK,p->sequence,2u);break;}
  failsafe_disarm(&safety);queue_ack(PKT_ACK,p->sequence,0u);break;
 case PKT_ARM:
  if(p->length){queue_ack(PKT_NACK,p->sequence,2u);break;}
  queue_ack(failsafe_arm(&safety,now)?PKT_ACK:PKT_NACK,p->sequence,4u);break;
 case PKT_DRIVE:{
  if(p->length!=12u){queue_ack(PKT_NACK,p->sequence,2u);break;}
  float vx=get_float(&p->payload[0]),vy=get_float(&p->payload[4]),wz=get_float(&p->payload[8]);
  float speeds[4]={0};
  const mecanum_geometry_t g={AETHER_WHEEL_RADIUS_M,AETHER_WHEELBASE_M,AETHER_TRACK_WIDTH_M,AETHER_MAX_WHEEL_RAD_S};
  if(!isfinite(vx)||!isfinite(vy)||!isfinite(wz) || !mecanum_inverse(&g,vx,vy,wz,speeds) ||
     !wheel_loop_set(&loop,speeds) || !failsafe_drive(&safety,now)){
    failsafe_disarm(&safety);queue_ack(PKT_NACK,p->sequence,4u);break;
  }
  queue_ack(PKT_ACK,p->sequence,0u);break;
 }
 case PKT_SET_PID:{
  if(p->length!=17u || safety.state!=STATE_DISARMED || p->payload[0]>=4u){
   queue_ack(PKT_NACK,p->sequence,2u);break;
  }
  const float kp=get_float(&p->payload[1]),ki=get_float(&p->payload[5]);
  const float kd=get_float(&p->payload[9]),kff=get_float(&p->payload[13]);
  if(!isfinite(kp)||!isfinite(ki)||!isfinite(kd)||!isfinite(kff) ||
     kp<0||ki<0||kd<0||kff<0||kp>20||ki>20||kd>20||kff>20){
   queue_ack(PKT_NACK,p->sequence,2u);break;
  }
  pid_init(&loop.controller[p->payload[0]],kp,ki,kd,kff,-1.0f,1.0f);
  queue_ack(PKT_ACK,p->sequence,0u);break;
 }
 case PKT_SET_MODE:
  if(p->length!=1u || p->payload[0]>2u || safety.state!=STATE_DISARMED){
   queue_ack(PKT_NACK,p->sequence,2u);break;
  }
  mode=p->payload[0];safety.rc_active=(uint8_t)(mode==2u);
  queue_ack(PKT_ACK,p->sequence,0u);break;
 case PKT_CLEAR_ESTOP:
  if(p->length || !hw_ready || !estop_rearm(&estop,now)){
    queue_ack(PKT_NACK,p->sequence,4u);break;
  }
  safety.estop_released=1u;
  queue_ack(failsafe_clear_estop(&safety)?PKT_ACK:PKT_NACK,p->sequence,4u);break;
 case PKT_FAULT_REQ:
  if(p->length){queue_ack(PKT_NACK,p->sequence,2u);break;}
  report_fault_log();break;
 case PKT_STATUS_REQ:
  if(p->length){queue_ack(PKT_NACK,p->sequence,2u);break;}
  status_packet();break;
 default:queue_ack(PKT_NACK,p->sequence,5u);break;
 }
}
void controller_init(void){
 uart_link_init(&link);failsafe_init(&safety);faults_init(&faults);
 wheel_loop_init(&loop);
 failsafe_begin_selftest(&safety);
 /* Safe software-only self-test: physical NC E-stop and hardware calibration
  * remain unresolved. No code may set interlock true in this revision. */
 failsafe_selftest_complete(&safety,1);
 for(unsigned i=0;i<4u;i++){
   motor_drv8876_init(&motors[i]);
   motors[i].overcurrent_a=AETHER_CURRENT_LIMIT_A;
   encoder_init(&encoder[i],i==0u?32u:16u,AETHER_ENCODER_COUNTS_WHEEL_REV);
 }
 adc_sense_init(&adc);
 adc.ipropri_v_per_a=AETHER_IPROPI_VOLTS_PER_AMP;
 adc.divider_ratio=AETHER_BATTERY_DIVIDER_RATIO;
 adc.low_v=AETHER_BATTERY_LOW_V;
 adc.critical_v=AETHER_BATTERY_CRITICAL_V;
 estop_init(&estop);
 watchdog_init(&loop_watchdog,AETHER_TIMEOUT_MS);
 loop.max_abs_rad_s=AETHER_MAX_WHEEL_RAD_S;
 loop.ramp_per_s=0.0f;/* TODO real safe ramp, refuses actuation until set */
 hw_ready=(uint8_t)(AETHER_ACTUATION_ENABLED && AETHER_PINMAP_VERIFIED &&
     AETHER_WHEEL_RADIUS_M>0.0f && AETHER_WHEELBASE_M>0.0f &&
     AETHER_TRACK_WIDTH_M>0.0f && AETHER_ENCODER_COUNTS_WHEEL_REV>0.0f &&
     AETHER_CURRENT_LIMIT_A>0.0f && AETHER_IPROPI_VOLTS_PER_AMP>0.0f &&
     AETHER_BATTERY_DIVIDER_RATIO>1.0f && AETHER_BATTERY_LOW_V>AETHER_BATTERY_CRITICAL_V &&
     loop.ramp_per_s>0.0f);
 if(hw_ready){
    hw_ready=(uint8_t)(actuation_hw_init() && adc_hw_init() && iwdg_hw_init());
 }
 safety.hardware_interlock=hw_ready;
 serial_ok=(uint8_t)uart_vcp_init();
 if(!serial_ok)faults_report(&faults,FAULT_LINK,0u);
 last_tick=0u;tx_at=0u;txseq=0u;mode=0u;sample_at=0u;
}
void controller_tick(uint32_t now){
 if(serial_ok){
  uint8_t incoming[64];size_t n=uart_vcp_read(incoming,sizeof(incoming));
  for(size_t i=0;i<n;i++)uart_link_rx_isr(&link,incoming[i]);
  packet_t p={0};int rc;
  while((rc=uart_link_poll(&link,&p))!=0){
   if(rc==1)process(&p,now);
   else if(rc==-2)faults_report(&faults,FAULT_LINK,now);/* invalid sequence */
  }
 }
 if(now!=last_tick){
  if(!watchdog_healthy_loop(&loop_watchdog,now))faults_report(&faults,FAULT_CONFIGURATION,now);
  last_tick=now;
  if(hw_ready){
    const int closed=actuation_hw_estop_nc_closed();
    estop_sample(&estop,closed,now);
    if(!closed)failsafe_estop(&safety,1);
    else safety.estop_released=1u;
    for(unsigned i=0;i<4u;i++){
       if(actuation_hw_fault_active(i)){
          motor_drv8876_set_fault(&motors[i],1);
          faults_report(&faults,FAULT_DRIVER,now);
          failsafe_fault(&safety,FAULT_DRIVER);
       }
    }
    float voltages[5];
    if(adc_hw_volts(voltages) && adc_sense_update(&adc,voltages)){
       if(adc.critical){faults_report(&faults,FAULT_BATTERY,now);failsafe_fault(&safety,FAULT_BATTERY);}
       for(unsigned i=0;i<4u;i++){
          motor_drv8876_update_current(&motors[i],adc.currents_a[i]);
          if(motors[i].fault){faults_report(&faults,FAULT_CURRENT,now);failsafe_fault(&safety,FAULT_CURRENT);}
       }
    }
    if((uint32_t)(now-sample_at)>=5u){
      float feedback[4]={0};sample_at=now;
      for(unsigned i=0;i<4u;i++){
         (void)encoder_sample(&encoder[i],actuation_hw_encoder_counter(i),0.005f);
         feedback[i]=encoder[i].rad_s;
      }
      float duty[4]={0};
      if(safety.outputs_enabled && adc.valid){
         wheel_loop_tick(&loop,feedback,0.005f,duty);
         for(unsigned i=0;i<4u;i++)
           if(!motor_drv8876_apply(&motors[i],i,duty[i],actuation_hw_outputs(),hw_ready))
             failsafe_fault(&safety,FAULT_CONFIGURATION);
      }
    }
  }
  failsafe_tick(&safety,now);
  if(!safety.outputs_enabled)actuation_hw_disable_all();
  if(hw_ready && !faults.active_flags)(void)iwdg_hw_feed();
  if(safety.state==STATE_ARMED || safety.state==STATE_RUNNING){
    /* actuation disabled in compile-time config; outputs stay unconfigured */
    if(!AETHER_ACTUATION_ENABLED)failsafe_disarm(&safety);
  }
  if((uint32_t)(now-tx_at)>=1000u/AETHER_TELEMETRY_HZ){
    tx_at=now;status_packet();
  }
 }
 if(serial_ok){
  const uint8_t *bytes;uint16_t n;
  if(uart_link_next_tx(&link,&bytes,&n) && uart_vcp_write(bytes,n))uart_link_tx_complete(&link);
 }
}
