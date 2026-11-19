# AETHER-X 01 Wiring Record — PROPOSED, NOT VERIFIED
All entries marked TODO are unverified. **NO MOTOR POWER** until actual carrier/MCU and motor ratings are checked.
| Component | Pin/Port | Connects to | Voltage | Notes |
|---|---|---|---|---|
| Battery + | Pack + | Main fuse | TODO pack | Confirm BMS chemistry, limits, polarity |
| Main fuse | Fused + | Physical normally-closed E-stop motor cut | TODO | Rating from measured stall/BMS/wire limits |
| E-stop output | Motor supply bus | Four DRV8876 VM branches | TODO | Interrupt motor power without software |
| Battery independent branch | 5.1V buck output | Pi USB-C regulated input | 5.1V | Confirm current and ripple; never connect raw 3S pack |
| Battery independent branch | Board-approved regulator | STM32 BlackPill supply | TODO | Exact board power input to verify; logic 3.3V |
| Pi TX (candidate BCM14, header 8) | /dev/serial0 | STM32 UART RX TODO | 3.3V | Cross TX to RX, UART baud TODO |
| Pi RX (candidate BCM15, header 10) | /dev/serial0 | STM32 UART TX TODO | 3.3V | Cross RX to TX, no 5V |
| Pi GND (example header 6) | Common signal reference | STM32 GND + four driver grounds | 0V | Ground return topology must avoid motor current in Pi leads |
| STM32 PWM/PH/nSLEEP outputs | Four independent sets TODO | DRV8876 FL FR RL RR control pins | 3.3V expected | Confirm carrier PMODE/pullups, logic thresholds |
| Four DRV8876 OUT pairs | OUT1/OUT2 | 25GA370-26 FL FR RL RR motors | TODO | Do not hot-unplug motors |
| Four encoders | STM32 timer A/B TODO | FL FR RL RR feedback | TODO | Level-shift if outputs exceed MCU IO rating |
| MPU6050 SDA/SCL | Proposed STM32 I2C TODO | STM32 I2C | 3.3V signal | Address 0x68 or 0x69 depending AD0; scan actual bus |
| HC-SR04 TRIG | STM32 GPIO TODO | HC-SR04 TRIG | TODO | Check module trigger threshold |
| HC-SR04 ECHO | Level shifter or divider | STM32 GPIO TODO | ECHO can be 5V | 5V -> 3.3V divider (example 1k top, 2k bottom) ONLY after measuring and verifying IO limits |
| Pi Camera | Compatible CSI cable | Pi CSI | module-specific | OV7670 parallel is not plug-and-play CSI |
| FlySky receiver | TODO | Receiver interface TODO | TODO | Confirm actual receiver protocol/voltage |

**Important:** if the ultrasonic ECHO is routed to Pi GPIO instead of STM32 GPIO, use compatible 3.3V level shifting. The 1k/2k divider is a nominal example for a push-pull 5V echo source, not an approved wiring prescription before module identification.

## Power verification gates
1. Battery disconnected: continuity/short checks, polarity and wiring review.
2. Use current-limited bench supply initially; verify buck outputs without Pi or MCU connected.
3. Confirm emergency switch interrupts ALL four motor VM feeds independent of firmware.
4. Confirm common reference and correct motor-current return path; do not route motor current through Pi ground.
5. Fit motor power only after regulator, driver, current and fuse tests. Mount wheels in air.
