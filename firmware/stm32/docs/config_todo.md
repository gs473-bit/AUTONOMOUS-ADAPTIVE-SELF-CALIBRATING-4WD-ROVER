# Hardware TODOs — prevent actuation
- Photograph and confirm actual MCU, PCB variant, datasheet, installed headers; current photos: NUCLEO-F303RE, requested default: BlackPill F411CE.
- Confirm 4x DRV8876 Pololu 4037, PH/EN PMODE, nSLEEP pulls, fault and CS/IPROPI scaling.
- Measure wheel loaded radius, wheelbase, track width, motor voltage, gear ratio and x4 wheel-output encoder ticks/rev.
- Confirm 3 or other number of HC-SR04 modules and which computer controls MPU6050/ultrasonic (previously Pi, now specified MCU).
- Confirm actual 3S2P battery discharge/charge ratings, BMS, voltage divider resistor values and regulator.
- Provide safe E-stop NC wiring schematic with motor-supply cut, current limit and fuse selection.
- Approve a conflict-free MCU pin map; no active drive outputs until then.
