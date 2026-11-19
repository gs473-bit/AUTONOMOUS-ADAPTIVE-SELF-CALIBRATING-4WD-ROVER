#ifndef AETHER_ADC_SENSE_H
#define AETHER_ADC_SENSE_H
#include <stdint.h>
typedef struct {float currents_a[4],battery_v,filter_alpha,ipropri_v_per_a,divider_ratio,low_v,critical_v;uint8_t valid,low,critical;} adc_sense_t;
void adc_sense_init(adc_sense_t *s);
int adc_sense_update(adc_sense_t *s,const float voltages[5]);
#endif
