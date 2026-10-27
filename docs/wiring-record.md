
# Rover Wiring Record

**Project:** ECE357 Autonomous Adaptive Self-Calibrating 4WD Rover  
**Branch:** main  
**Status:** Draft — wiring not yet verified

## 1. Motor Driver Connections

| Signal | Raspberry Pi GPIO (BCM) | Driver Pin | Verified |
|---|---|---|---|
| Motor A enable | TBD | ENA | No |
| Motor A direction 1 | TBD | IN1 | No |
| Motor A direction 2 | TBD | IN2 | No |
| Motor B enable | TBD | ENB | No |
| Motor B direction 1 | TBD | IN3 | No |
| Motor B direction 2 | TBD | IN4 | No |

Repeat for the second motor driver after identifying the exact board.

## 2. Sensor Connections

| Sensor | Interface | Pi Pins | Verified |
|---|---|---|---|
| Camera | To confirm | To confirm | No |
| HC-SR04 | GPIO | To confirm | No |
| MPU6050 | I2C | SDA/SCL, verify board | No |
| Wheel encoders | GPIO or suitable interface | To confirm | No |

## 3. Power System

- Battery model and voltage: Not confirmed
- Motor supply: Not confirmed
- Regulated Pi supply: Not confirmed
- Fuse and rating: Not confirmed
- Physical emergency stop: Not confirmed
- Grounding arrangement: To be reviewed

## 4. Safety Checklist

- [ ] Disconnect power before changing wiring.
- [ ] Verify all signal voltages are compatible with 3.3 V Pi GPIO.
- [ ] Protect the Pi from a potentially 5 V HC-SR04 ECHO signal.
- [ ] Confirm motor and driver current ratings.
- [ ] Verify supply voltage and polarity with a multimeter.
- [ ] Have the power and emergency-stop design reviewed before motor tests.

## 5. Test Evidence

- Date:
- Test performed:
- Actual measurements:
- Photo or diagram:
- Result: Not tested
- Outstanding issues:

**Important:** Do not use this draft as a final pin-by-pin wiring diagram. Update it from the actual components and approved wiring plan.
