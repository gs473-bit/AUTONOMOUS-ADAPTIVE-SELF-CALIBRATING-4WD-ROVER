# AETHER UART protocol v1
Pi -> STM32 over USB (ST-LINK VCP, USART2 PA2/PA3, 115200 8N1). Authoritative byte layout in `firmware/stm32/docs/protocol.md` with C codec in `firmware/stm32/src/protocol/packet.c` and Python mirror `src/rover/comms/protocol.py`.

SOF A5 5A | uint16 LE payload length | uint16 LE sequence | uint8 type | bytes payload | uint16 LE CRC16-CCITT-FALSE over length..payload (polynomial 0x1021, init 0xffff). Payload 0..128 bytes.

Commands: DRIVE 0x01 (float32 LE vx m/s, vy m/s, wz rad/s); STOP 0x02; ARM 0x03; DISARM 0x04; SET_PID 0x05 (uint8 wheel + four float32 gains); SET_MODE 0x06 (uint8); STATUS_REQ 0x07; HEARTBEAT 0x08. Responses 0x81 telemetry, 0x82 ACK, 0x83 NACK, 0x84 fault log. Motion commands are only accepted after physical E-stop release and explicit ARM, which remains disabled with current TODO hardware config.

**Motion safety:** Pi heartbeat thread cannot extend a stale DRIVE indefinitely: Pi issues STOP when motion-command lease expires at 150ms; STM32 safety also expires DRIVE independently at 200ms even if heartbeats continue. Bad CRC/length/sequence are dropped and counted, never refresh the STM32 watchdog.

Telemetry is 59 bytes: float32 wheel_rad_s[4], int32 encoder_counts[4], float32 battery_v, float32 motor_currents_a[4], uint8 state, uint32 fault_flags, uint8 rc_state, uint8 valid_mask. Initial firmware reports validity mask **0** and values 0; **DO NOT treat those values as sensor measurements.**

The physical E-stop must cut motor power and cannot rely on this protocol.
