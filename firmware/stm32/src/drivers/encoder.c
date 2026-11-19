#include "drivers/encoder.h"
#include <math.h>
void encoder_init(encoder_t *e,uint8_t bits,float cpr) {
 if(e)*e=(encoder_t){.bits=bits,.counts_per_wheel_rev=cpr};
}
int32_t encoder_delta(uint32_t now,uint32_t before,uint8_t bits){
 return bits==16u ? (int16_t)(uint16_t)(now-before) : (int32_t)(now-before);
}
int encoder_sample(encoder_t *e,uint32_t counter,float dt){
 if(!e || (e->bits!=16u && e->bits!=32u) || !isfinite(dt) || dt<=0.0f)return 0;
 if(!e->initialized){e->last=counter;e->initialized=1u;return 0;}
 const int32_t delta=encoder_delta(counter,e->last,e->bits);
 e->last=counter;e->total_ticks+=delta;
 if(!(e->counts_per_wheel_rev>0.0f) || !isfinite(e->counts_per_wheel_rev)){e->valid=0u;return 0;}
 const float instant=((float)delta * 6.28318530718f)/(e->counts_per_wheel_rev*dt);
 e->rad_s=e->valid ? 0.75f*e->rad_s + 0.25f*instant : instant;
 e->valid=1u;return 1;
}
