#ifndef AETHER_CONFIG_H
#define AETHER_CONFIG_H
/* Must stay 0 until wiring, interlocks and current limits are verified. */
#define AETHER_ACTUATION_ENABLED 0
#define AETHER_WHEELS 4u
#define AETHER_PWM_HZ 20000u
#define AETHER_TICK_HZ 1000u
#define AETHER_PID_HZ 200u
#define AETHER_TELEMETRY_HZ 20u
#define AETHER_TIMEOUT_MS 200u
#define AETHER_STOP_SETTLE_MS 30u
#define AETHER_RC_TIMEOUT_MS 100u
/* 0 = unresolved and non-armable; never synthesize mechanical/electrical ratings. */
#define AETHER_WHEEL_RADIUS_M 0.0f
#define AETHER_WHEELBASE_M 0.0f
#define AETHER_TRACK_WIDTH_M 0.0f
#define AETHER_ENCODER_COUNTS_WHEEL_REV 0.0f
#define AETHER_MAX_WHEEL_RAD_S 0.0f
#define AETHER_CURRENT_LIMIT_A 0.0f
#define AETHER_IPROPI_VOLTS_PER_AMP 0.0f
#define AETHER_BATTERY_DIVIDER_RATIO 0.0f
#define AETHER_BATTERY_LOW_V 0.0f
#define AETHER_BATTERY_CRITICAL_V 0.0f
#define AETHER_HCSR04_COUNT 0u /* Pi owned, TODO count and positions */
#define AETHER_HOST_BAUD 115200u
#endif
