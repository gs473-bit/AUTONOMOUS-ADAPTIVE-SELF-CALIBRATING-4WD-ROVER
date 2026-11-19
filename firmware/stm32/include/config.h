#ifndef AETHER_CONFIG_H
#define AETHER_CONFIG_H
/* SAFETY: no motor outputs are enabled until actual pin and electrical review. */
#define AETHER_ACTUATION_ENABLED 0
#define AETHER_WHEEL_COUNT 4u
#define AETHER_HCSR04_COUNT 3u /* requested default; NOT physically confirmed */
#define AETHER_PWM_HZ 20000u
#define AETHER_SCHEDULER_HZ 1000u
#define AETHER_PID_HZ 200u
#define AETHER_IMU_HZ 200u
#define AETHER_ULTRASONIC_HZ_TOTAL 20u
#define AETHER_TELEMETRY_HZ 20u
#define AETHER_COMMAND_TIMEOUT_MS 200u
#define AETHER_RC_TIMEOUT_MS 100u
#define AETHER_CURRENT_LIMIT_A 0.0f /* TODO: carrier, VREF and thermal data */
#define AETHER_WHEEL_RADIUS_M 0.0f /* TODO: measure */
#define AETHER_WHEELBASE_M 0.0f /* TODO: center-to-center */
#define AETHER_TRACK_WIDTH_M 0.0f /* TODO: center-to-center */
#define AETHER_ENCODER_TICKS_PER_WHEEL_REV 0.0f /* TODO: x4 and gear ratio */
#define AETHER_BATTERY_DIVIDER_RATIO 0.0f /* TODO: resistor measurement */
#define AETHER_BATTERY_LOW_V 0.0f /* TODO: BMS/cell limits */
#define AETHER_BATTERY_CRITICAL_V 0.0f /* TODO */
#endif
