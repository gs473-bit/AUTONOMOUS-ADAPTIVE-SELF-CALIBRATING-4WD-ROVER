# AETHER-X 01 split-architecture wiring — DRAFT, not verified

**PROPOSED: verify against actual wiring before applying power.** This diagram does NOT authorize motor power. Pi UART GPIO14/15 are **NOT USED**; Nucleo communicates through ST-LINK USB VCP on USART2 PA2/PA3.

| Component | Pin/port | Connects to | Voltage | Notes |
|---|---|---|---|---|
| Wattnine 3S2P battery (+) | Pack output | Fuse then physical NC E-stop then motor power distribution | 9.2–12.6V label | 4A rating marked, verify BMS continuous/peak current and connector polarity |
| Motor distribution | Fused motor branch | 4x Pololu DRV8876 #4037 VIN | pack voltage | Motor power **DISCONNECTED** for Pi and firmware tests |
| Battery (−) | Motor return wiring | Driver GND return, separate heavy current path | 0V ref | Do not send motor current through Pi GPIO/header ground |
| Pi USB-C | Independent 5.1V 3A-rated regulated supply | Raspberry Pi 4B | 5.1V | Verify actual PSU and cable; **never raw 12.6V battery** |
| Nucleo ST-LINK USB | USB-A from powered Pi | NUCLEO-F303RE ST-LINK/V2-1 | USB 5V | Check JP5 / Nucleo power jumpers and board manual before other supplies; never backfeed Pi 5V rail |
| Nucleo UART2 | PA2 TX / PA3 RX internal to ST-LINK VCP | Pi `/dev/aether_stm32` (fallback /dev/ttyACM0) | MCU 3.3V logic, USB external | DO NOT cross-wire Pi GPIO UART to these pins |
| Pi GY-521 VCC/GND | Pi physical pins 1 / 6 | GY-521 VCC/GND | 3.3V proposed | Confirm breakout regulator and pull-up levels with actual board |
| Pi GY-521 SDA/SCL | Pi physical pins 3 / 5 | GPIO2 SDA1 / GPIO3 SCL1 | 3.3V I2C | Default address 0x68 subject to i2cdetect |
| Pi HC-SR04 VCC/GND | Pi physical 2 / 9 | Sensor power | 5V | Per sensor, avoid Pi 5V overload |
| Pi HC-SR04 TRIG | GPIO23 physical 16 | TRIG first sensor | 3.3V logic | Additional sensor pins TODO |
| Pi HC-SR04 ECHO | Through 2.2kΩ upper / 3.3kΩ lower divider to GPIO24 physical 18 | ECHO first sensor | 5V -> approx 3.0V | Measure output and ensure <3.3V before connecting Pi |
| Nucleo PWM 1/2/3/4 | TIM1 PA8/PA9/PA10 + TIM17 PB9 | DRV8876 EN/IN1 FL/FR/RL/RR | 3.3V | PH/EN chosen; PWM ~20kHz if verified 72MHz clock |
| Nucleo DIR x4 | PC8 / PC9 / PC10 / PC11 | DRV8876 PH/IN2 | 3.3V | Proposed |
| Nucleo nSLEEP x4 | PB3 / PB4 / PB5 / PB12 | DRV8876 SLP | 3.3V | External fail-low pulldowns required; PB3/4 debug conflict review |
| Nucleo nFAULT x4 | PC4 / PC5 / PB10 / PB8 | DRV8876 FLT (active low) | 3.3V or carrier pullup TODO | Four distinct EXTI lines if enabled |
| Nucleo CS/IPROPI x4 | PB0/PB1/PC0/PC1 ADC1 | DRV8876 CS | **0–3.3V MAX** | Pololu nominal 2.49 V/A may exceed ADC input at high current; analog divider/clamp TBD |
| Nucleo battery ADC | PC2 ADC1 | Battery via selected voltage divider | 0–3.3V | MUST handle full 12.6V and spikes, resistor values TODO |
| Nucleo encoders A/B | TIM2 PA0/PA1 (FL); TIM3 PA6/PA7 (FR); TIM4 PB6/PB7 (RL); TIM8 PC6/PC7 (RR) | Yellow A, Green B per motor | 3.3V IO only | Blue encoder VCC 3.3V only after physical verification, Black GND |
| NC E-stop status | PB13 input pullup | Normally-closed contact to GND when released | 3.3V | Open circuit = ESTOP; use independent *motor-power* cut contact |
| FlySky iBUS RX (optional) | PB11 USART3_RX proposed | Receiver data | TBD | Receiver model, ownership and voltage not confirmed |
| Motor outputs | DRV8876 OUT1/OUT2 | Motor red / white | 12V nominal | Swapping leads reverses rotation; do not hot-plug |
| Driver PMODE | GND | Each driver PMODE | 0V | PH/EN mode selected before awakening drivers |

Why **PH/EN**: one PWM plus one direction per motor, easier to enforce signed limits and independent nSLEEP defaults. On Pololu #4037 confirm PMODE and on-board defaults, fault/current-sense scaling against its actual schematic; 3.3V on SLEEP changes current limiting.

**No-go until inspected:** tested fuse/E-stop, Nucleo board power jumpers, verified independent motor supply/ground routing, encoder voltage, IC ADC protection, external SLEEP pull-downs, per-motor current and stall data. A normal 4A-marked pack can be overloaded by four motor stalls. Electrical cut-off may permit coasting.
