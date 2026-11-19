#ifndef AETHER_ACTUATION_HW_H
#define AETHER_ACTUATION_HW_H
#include <stdint.h>
#include "drivers/motor_drv8876.h"
/* All functions compile, but init returns false until wiring is approved. */
int actuation_hw_init(void);
const motor_output_t *actuation_hw_outputs(void);
uint32_t actuation_hw_encoder_counter(unsigned wheel);
int actuation_hw_estop_nc_closed(void);
int actuation_hw_fault_active(unsigned wheel);
void actuation_hw_disable_all(void);
#endif
