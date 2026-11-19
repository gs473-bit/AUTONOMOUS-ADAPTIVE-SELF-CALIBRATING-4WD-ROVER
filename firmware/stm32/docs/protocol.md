# AETHER-X 01 serial framing v1
**HOST-MOCKED ONLY; no STM32 ↔ Pi wire test.** UART settings and pinouts must be confirmed.
All multi-byte integers and IEEE-754 single-precision floats are **little-endian**.

| Byte offset | Field | Size |
|---|---|---|
| 0..1 | SOF A5 5A | 2 |
| 2..3 | Payload length (max 128) | uint16 |
| 4..5 | Sequence (wrap uint16) | uint16 |
| 6 | Packet type | uint8 |
| 7..6+N | Payload | N |
| 7+N..8+N | CRC16 CCITT-FALSE over bytes 2..6+N | uint16 |

CRC poly 0x1021, init 0xFFFF, no reflection, xorout 0.
Bad CRC, length and sequence packets are rejected and counted, **never refresh safety heartbeat**. Sequence mismatch on DRIVE or HEARTBEAT does not execute; STOP/DISARM may re-sync.

| Type ID | Direction | Payload units / encoding |
|---|---|---|
| 0x01 DRIVE | Pi→STM32 | 3 × float32: vx m/s, vy m/s, wz rad/s |
| 0x02 STOP | Pi→STM32 | none |
| 0x03 ARM | Pi→STM32 | none |
| 0x04 DISARM | Pi→STM32 | none |
| 0x05 SET_PID | Pi→STM32 | uint8 wheel 0..3, float32 kp, ki, kd, kff |
| 0x06 SET_MODE | Pi→STM32 | uint8: 0 manual, 1 autonomous, 2 RC |
| 0x07 STATUS_REQ | Pi→STM32 | none |
| 0x08 HEARTBEAT | Pi→STM32 | none |
| 0x81 TELEMETRY | STM32→Pi | defined below |
| 0x82 ACK | STM32→Pi | uint16 acknowledged sequence |
| 0x83 NACK | STM32→Pi | uint16 acknowledged sequence, uint8 reason |
| 0x84 FAULT_LOG | STM32→Pi | uint8 count, then repeated uint16 fault + uint32 ms |

Telemetry v1 fields in order: float32 wheel_rps[4], int32 encoder_ticks[4], float32 gyro_rad_s[3], float32 accel_m_s2[3], float32 ranges_m[3], float32 battery_v, float32 motor_current_a[4], uint8 state, uint32 fault_flags, uint8 rc_state, uint32 rx_crc_errors, uint32 rx_sequence_errors.
All sensor fields must be explicitly invalid in accompanying validity masks or NaN until measured. Hardware interface not yet complete.

Timing: host sends HEARTBEAT or DRIVE at >=20 Hz. **No valid frame for 200 ms ⇒ stop/disable outputs**. A valid STOP never authorizes rearming. Physical motor-power E-stop is independent of software.
