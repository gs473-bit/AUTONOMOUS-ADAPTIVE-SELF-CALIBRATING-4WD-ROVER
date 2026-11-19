#ifndef AETHER_FAULT_MANAGER_H
#define AETHER_FAULT_MANAGER_H
#include <stdint.h>
#define FAULT_LOG_CAP 16u
enum {FAULT_DRIVER=1u,FAULT_CURRENT=2u,FAULT_ENCODER_STALL=4u,FAULT_BATTERY=8u,FAULT_ESTOP=16u,FAULT_LINK=32u,FAULT_CONFIGURATION=64u};
typedef struct {uint32_t at_ms;uint32_t flags;} fault_entry_t;
typedef struct {fault_entry_t log[FAULT_LOG_CAP];uint8_t next,count;uint32_t active_flags;} fault_manager_t;
void faults_init(fault_manager_t *f);
void faults_report(fault_manager_t *f,uint32_t flags,uint32_t now);
void faults_clear(fault_manager_t *f,uint32_t mask);
int faults_get_recent(const fault_manager_t *f,unsigned newest_index,fault_entry_t *out);
#endif
