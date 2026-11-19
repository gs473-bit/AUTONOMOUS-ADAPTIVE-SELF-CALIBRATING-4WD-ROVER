#include "drivers/adc_sense.h"
#include <math.h>
void adc_sense_init(adc_sense_t *s){if(s)*s=(adc_sense_t){.filter_alpha=.2f};}
int adc_sense_update(adc_sense_t *s,const float v[5]){
 if(!s || !v || s->ipropri_v_per_a<=0.0f || s->divider_ratio<=1.0f ||
    s->critical_v<=0.0f || s->low_v<=s->critical_v)return 0;
 for(unsigned i=0;i<5u;i++)if(!isfinite(v[i])||v[i]<0.0f||v[i]>3.3f)return 0;
 for(unsigned i=0;i<4u;i++){
  float a=v[i]/s->ipropri_v_per_a;
  s->currents_a[i]=s->valid ? (1.0f-s->filter_alpha)*s->currents_a[i]+s->filter_alpha*a:a;
 }
 float vb=v[4]*s->divider_ratio;
 s->battery_v=s->valid ? (1.0f-s->filter_alpha)*s->battery_v+s->filter_alpha*vb:vb;
 s->valid=1u;s->critical=(uint8_t)(s->battery_v<=s->critical_v);
 s->low=(uint8_t)(s->battery_v<=s->low_v);return 1;
}
