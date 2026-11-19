# Values and decisions needed before actuating hardware
All values marked TODO are not invented by firmware.
| Item | How to obtain | State |
|---|---|---|
| Pi RAM and Raspberry Pi OS version | `free -h`, `cat /etc/os-release` | TODO |
| Receiver and owner | Photograph receiver PCB/model and iBUS cable; recommend Nucleo USART3 for independent RC kill | TODO |
| HC-SR04 count and mounting orientations | Count physical modules, photograph mount locations, allocate distinct Pi GPIO | TODO |
| Loaded rolling wheel radius | Roll 10 revolutions on floor, distance / (20π) | TODO |
| Wheelbase and track width | Measure wheel centre-to-centre front/rear and left/right | TODO |
| Encoder x4 counts per output revolution | Hand-turn marked output shaft once; tally quadrature count on each motor | TODO |
| Motor gearbox ratio | Exact motor suffix/datasheet or turn motor rotor vs output | TODO |
| Maximum wheel speed + accel | Measure safe on stands and with low-current load | TODO |
| 4× IPROPI scaling / ADC voltage | Measure with known current and verify voltage never >3.3V, fit divider/clamp if needed | TODO |
| nFAULT wiring/polarity, 4× nSLEEP pulldowns | Verify Pololu #4037 PCB and continuity; measure reset GPIO levels | TODO |
| Battery divider Rtop/Rbottom | Choose ratio so <=3.3V at at least 12.6V plus spikes, measure resistors | TODO |
| Pack BMS continuous/peak current, cutoff limits | Manufacturer specification, not merely 4A label | TODO |
| Fuse and VM E-stop | Engineer from motor stall, BMS and wire ampacity; test disconnect | TODO |
| MB1136 jumper settings / STM32 pinmap | Cross-check CN7/CN10, PA2/PA3 VCP, TIM AF tables and solder bridges | TODO |
| GY-521 breakout I2C electrical behavior | Check supply regulator and pullups with multimeter and scan | TODO |
| HC-SR04 ECHO divider values | Fit proposed 2.2k/3.3k then measure | TODO |
| Pi Camera model/CSI ribbon | Read board part number and run rpicam detection | TODO |
| ST-LINK unique USB serial for udev | `udevadm info -q property -n /dev/ttyACM0` | TODO |
| PID gains, current/stall limits, latency CPU | Test mocked then validate with current-limited stands and oscilloscope | TODO |

No software acceptance can substitute for a physical fuse, normally-closed E-stop, or driver current limiting.
