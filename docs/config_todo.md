# Values and decisions needed before actuating hardware
All values marked TODO are not invented by firmware.
| Item | How to obtain | State |
|---|---|---|
| Pi RAM and Raspberry Pi OS version | `free -h`, `cat /etc/os-release` | TODO |
| Smartphone UI access | Configure local token and authenticated HTTPS reverse proxy/private tunnel; leave movement disabled until commissioned | TODO deployment |
| HC-SR04 count | **4 modules confirmed** by owner, Pi-controlled | CONFIRMED, not hardware-tested |
| HC-SR04 mounting orientations and BCM GPIO pairs | Record four positions and four distinct TRIG/ECHO pairs; each ECHO needs voltage divider | TODO |
| Loaded rolling wheel radius | Roll 10 revolutions on floor, distance / (20π) | TODO |
| Wheelbase and track width | Measure wheel centre-to-centre front/rear and left/right | TODO |
| Encoder x4 counts per output revolution | Earlier motor wiring sheet says 11 signals for each channel per MOTOR shaft turn; verify whether pulses/cycles or edges and multiply only by verified gearbox ratio; physically hand-turn output shaft to calibrate | TODO |
| Motor gearbox ratio | A generic 25GA370 Tronsun datasheet example is 26:1 and **231 RPM**, not the 12 V 60 RPM motor supplied; earlier seller table offers a 12 V 60 RPM / 103:1 row but exact variant identity not proven. Read actual model stamp and count output rotation | TODO physical verification |
| Maximum wheel speed + accel | Measure safe on stands and with low-current load | TODO |
| 4× IPROPI scaling / ADC voltage | Measure with known current and verify voltage never >3.3V, fit divider/clamp if needed | TODO |
| nFAULT wiring/polarity, 4× nSLEEP pulldowns | Verify Pololu #4037 PCB and continuity; measure reset GPIO levels | TODO |
| Battery divider Rtop/Rbottom | Choose ratio so <=3.3V at at least 12.6V plus spikes, measure resistors | TODO |
| Battery continuous current | User confirms 4 A continuous, nominal 12 V, 4 Ah; treat as shared total pack budget | CONFIRMED owner report |
| Pack BMS peak cutoff, voltage thresholds, connectors | Manufacturer BMS spec and measured connector polarity still required | TODO |
| Fuse and VM E-stop | Engineer from motor stall, BMS and wire ampacity; test disconnect | TODO |
| MB1136 jumper settings / STM32 pinmap | Cross-check CN7/CN10, PA2/PA3 VCP, TIM AF tables and solder bridges | TODO |
| GY-521 breakout I2C electrical behavior | Check supply regulator and pullups with multimeter and scan | TODO |
| HC-SR04 ECHO divider values | Fit proposed 2.2k/3.3k then measure | TODO |
| Pi Camera model/CSI ribbon | Read board part number and run rpicam detection | TODO |
| ST-LINK unique USB serial for udev | `udevadm info -q property -n /dev/ttyACM0` | TODO |
| PID gains, current/stall limits, latency CPU | Test mocked then validate with current-limited stands and oscilloscope | TODO |

No software acceptance can substitute for a physical fuse, normally-closed E-stop, or driver current limiting.

## Encoder and gearbox references (do not copy into safety-critical firmware yet)
The provided 12V/60RPM 25GA370 chart includes a **103:1** reduction row. The motor wiring diagram states **11 signals per motor rotation** on the yellow/green feedback channels. If these 11 signals are *cycles per channel* and the gear ratio truly equals 103, x4 quadrature decoding would yield **11 × 4 × 103 = 4532** edges per output revolution. This is a *conditional calculation*, **not a calibrated encoder CPR**. A separate Tronsun 25GA370 datasheet describes a 26:1 / 231 RPM variant, which is not the claimed 60 RPM motor. Verify actual motor label/markings and measure output-shaft ticks before setting STM32 `AETHER_ENCODER_COUNTS_WHEEL_REV`. The unresolved setting remains zero and blocks actuation.
