#ifndef AETHER_CONFIG_H
#define AETHER_CONFIG_H
/* Hardware interlock: production actuation requires explicit engineering approval. */
#define AETHER_ACTUATION_ENABLED 0
#define AETHER_WHEEL_COUNT 4u
#define AETHER_HCSR04_COUNT 3u /* TODO confirm quantity; previously Pi-hosted */
#define AETHER_PWM_HZ 20000u /* proposed, not measured */
#define AETHER_TICK_HZ 1000u
#define AETHER_PID_HZ 200u
#define AETHER_IMU_HZ 200u
#define AETHER_SONAR_HZ_TOTAL 20u
#define AETHER_TELEMETRY_HZ 20u
#define AETHER_COMMAND_TIMEOUT_MS 200u
#define AETHER_RC_TIMEOUT_MS 100u
#define AETHER_FAULT_LOG_CAP 16u
#define AETHER_CURRENT_LIMIT_A 0.0f /* TODO verified per-carrier current limit */
#define AETHER_WHEEL_RADIUS_M 0.0f /* TODO measure loaded effective radius */
#define AETHER_WHEELBASE_M 0.0f /* TODO front/rear center distance */
#define AETHER_TRACK_WIDTH_M 0.0f /* TODO left/right center distance */
#define AETHER_ENCODER_TICKS_PER_WHEEL_REV 0.0f /* TODO output shaft x4 count */
#define AETHER_BATTERY_DIVIDER_RATIO 0.0f /* TODO actual resistors and Vref */
#define AETHER_BATTERY_LOW_V 0.0f /* TODO approved cell/BMS threshold */
#define AETHER_BATTERY_CRITICAL_V 0.0f /* TODO approved cell/BMS threshold */
#define AETHER_ENCODER_STALL_MIN_TICKS 0 /* TODO calibrate */
#define AETHER_SERIAL_BAUD 0u /* TODO: choose and test with Pi */
#endif
