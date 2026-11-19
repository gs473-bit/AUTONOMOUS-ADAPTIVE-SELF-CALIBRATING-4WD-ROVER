#include "app/controller.h"
#include "drivers/uart_link.h"
#include "safety/failsafe.h"
#include "safety/fault_manager.h"
#include "control/wheel_speed_loop.h"
#include "control/mecanum_kinematics.h"
#include "hal_port/uart_vcp.h"
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
static uint8_t mode;
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
static void status_packet(void){
 packet_t p={0};p.type=PKT_TELEMETRY;p.sequence=txseq++;
 p.length=59u;/* 4 wheel float + 4 tick int + battery float + 4 currents float
               * + state uint8 + fault uint32 + RC uint8 + valid mask uint8 */
 memset(p.payload,0,p.length);
 p.payload[52]=(uint8_t)safety.state;
 uint32_t f=faults.active_flags;
 for(unsigned i=0;i<4u;i++)p.payload[53+i]=(uint8_t)(f>>(8u*i));
 p.payload[57]=safety.rc_active? (safety.rc_signal_valid?1u:2u):0u;
 p.payload[58]=0u;/* EVERY sensor invalid; hardware sampling not established */
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
 safety.hardware_interlock=0u;serial_ok=(uint8_t)uart_vcp_init();
 if(!serial_ok)faults_report(&faults,FAULT_LINK,0u);
 last_tick=0u;tx_at=0u;txseq=0u;mode=0u;
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
  last_tick=now;failsafe_tick(&safety,now);
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
