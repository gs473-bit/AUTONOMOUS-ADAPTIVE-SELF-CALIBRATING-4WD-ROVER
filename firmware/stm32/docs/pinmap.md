# AETHER-X 01 — NUCLEO-F303RE pin allocation
**PROPOSED: verify against my actual wiring before flashing.** Never energize VM with this map unverified.

Target: photographed ST **NUCLEO-F303RE / STM32F303RET6 (LQFP64)**, not F411 BlackPill. Sources: [ST STM32F303RE datasheet AF matrix](https://www.st.com/resource/en/datasheet/stm32f303re.pdf), [ST MB1136 Nucleo-64 board manual](https://www.st.com/resource/en/user_manual/dm00105823-stm32-nucleo%20-64-boards-mb1136-stmicroelectronics.pdf).

| Function | Pin candidate | Timer/peripheral | AF | Notes |
|---|---|---|---|---|
| Encoder FL A/B | PA0 / PA1 | TIM2_CH1/CH2 | AF1 | 32-bit TIM2 |
| Encoder FR A/B | PA6 / PA7 | TIM3_CH1/CH2 | AF2 | 16-bit |
| Encoder RL A/B | PB6 / PB7 | TIM4_CH1/CH2 | AF2 | 16-bit; do not share with I2C1 |
| Encoder RR A/B | PC6 / PC7 | TIM8_CH1/CH2 | AF4 | 16-bit; TIM8 available on F303RE |
| PWM FL | PA8 | TIM1_CH1 | AF6 | Candidate, confirm board header |
| PWM FR | PC1 | TIM1_CH2 | AF2 | Candidate |
| PWM RL | PC2 | TIM1_CH3 | AF2 | Candidate |
| PWM RR | PC3 | TIM1_CH4 | AF2 | Candidate |
| Pi TX / Pi RX | PC5 RX / PC4 TX | USART1_RX/TX | AF7 | Pi TX->PC5, Pi RX<-PC4; 3.3V logic |
| FlySky iBUS | PB11 RX | USART3_RX | AF7 | UART receiver output voltage TODO |
| MPU6050 optional MCU | PA9 SCL / PA10 SDA | I2C2 | AF4 | **Previous user assignment was Pi**; do NOT duplicate wiring |
| HC-SR04 #1 echo | PB14 | TIM15_CH1 | AF1 | 5V ECHO requires divider |
| HC-SR04 #2 echo | PB15 | TIM15_CH2 | AF1 | 5V ECHO requires divider |
| HC-SR04 #3 echo | PB8 | TIM16_CH1 | AF1 | 5V ECHO requires divider; count TODO |
| Sonar #1/#2/#3 TRIG | PC8 / PC9 / PC10 | GPIO outputs | GPIO | Round-robin, 5V sensor supply |
| Motor FL/FR/RL/RR DIR | PC11 / PC12 / PD2 / PB2 | GPIO outputs | GPIO | PH control |
| Motor FL/FR/RL/RR SLEEP | PB3 / PB4 / PB5 / PB12 | GPIO outputs | GPIO | **PB3/PB4 debug pin considerations**; pulldown required |
| Motor FL/FR/RL/RR nFAULT | PB13 / PA15 / PC14 / PC15 | GPIO input | GPIO | **PA15 JTAG, PC14/15 LSE conflict**; verify oscillator/debug |
| Motor FL/FR/RL/RR CS/IPROPI | PA4 / PB0 / PB1 / PA2 | ADC channels | Analog | **PA2 may be connected to ST-LINK VCP**; MUST resolve solder bridge before use |
| Battery ADC | PC0 | ADC | Analog | Divider ratio TODO; max ADC 3.3V |
| NC E-stop status | PB10 | GPIO input pull-up | GPIO | Open wire = ESTOP; separate physical motor cut required |
| LED/buzzer optional | TODO | GPIO | TODO | Do not reuse on-board PA5 LED until reviewed |
| ST-LINK VCP | PA2/PA3 | USART2 | AF7 | ST-LINK route may conflict with PA2 current sensing |
| SWD | PA13/PA14 | SWDIO/SWCLK | AF0 | **RESERVED** |
| User button / LED | PC13 / PA5 | GPIO | GPIO | **RESERVED** |
| USB D-/D+ | PA11 / PA12 | USB FS | AF14 | **RESERVED** |
| BOOT0 | BOOT0 | Boot selector | n/a | **RESERVED** |

### Resource review
- Four **distinct encoder-mode timers** are available: TIM2 (32-bit), TIM3 (16-bit), TIM4 (16-bit), TIM8 (16-bit). On 16-bit channels, always compute wrap-safe signed delta.
- Four PWM channels share TIM1; no timer conflict with quadrature.
- Three sonar capture channels use TIM15 CH1/CH2 and TIM16 CH1. Ultrasonic driver must alternate edge polarity or use appropriate capture configuration (not configured yet).
- USART1 host and USART3 iBUS are independent. **DMA request/channel assignments are TODO** and must be verified using F303RE DMA tables before enabling DMA; current HAL port does not configure DMA.
- ST-LINK bridges on PA2, debug/oscillator restrictions for PA15/PC14/PC15/PB3/PB4, all shield pin accessibility and actual wire positions **remain unresolved**. This is a *candidate logical allocation*, not an approved fully wired hardware plan.
- In particular, previous project decisions put MPU6050 and HC-SR04 on the **Pi**. Drivers in the STM32 tree may be implemented but remain disabled until a single owner is selected. Do not wire devices to both controllers.
