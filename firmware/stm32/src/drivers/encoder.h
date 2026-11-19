#ifndef AETHER_ENCODER_H
#define AETHER_ENCODER_H
#include <stdint.h>
typedef struct {uint32_t last;int64_t total_ticks;float rad_s,counts_per_wheel_rev;uint8_t bits,initialized,valid;} encoder_t;
void encoder_init(encoder_t *e,uint8_t bits,float counts_per_wheel_rev);
int32_t encoder_delta(uint32_t current,uint32_t previous,uint8_t bits);
int encoder_sample(encoder_t *e,uint32_t counter,float dt_s);
#endif
