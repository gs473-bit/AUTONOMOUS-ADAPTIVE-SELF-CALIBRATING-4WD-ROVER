#include "safety/fault_manager.h"
#include <string.h>
void faults_init(fault_manager_t *f){if(f)memset(f,0,sizeof(*f));}
void faults_report(fault_manager_t *f,uint32_t flags,uint32_t now){
 if(!f || flags==0u)return;
 if ((f->active_flags & flags)==flags)return;/* one timestamp per newly asserted fault */
 f->active_flags|=flags;
 f->log[f->next]=(fault_entry_t){now,flags};
 f->next=(uint8_t)((f->next+1u)%FAULT_LOG_CAP);
 if(f->count<FAULT_LOG_CAP)f->count++;
}
void faults_clear(fault_manager_t *f,uint32_t mask){if(f)f->active_flags&=~mask;}
int faults_get_recent(const fault_manager_t *f,unsigned index,fault_entry_t *out){
 if(!f || !out || index>=f->count)return 0;
 *out=f->log[(unsigned)(f->next+FAULT_LOG_CAP-1u-index)%FAULT_LOG_CAP];return 1;
}
