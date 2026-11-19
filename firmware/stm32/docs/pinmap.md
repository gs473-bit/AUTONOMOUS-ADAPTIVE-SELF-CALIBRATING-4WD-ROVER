# AETHER-X 01 pin allocation
**PROPOSED: verify against my actual wiring before flashing.**

## Stop condition: hardware identity conflict
The user's actual PCB photo identified **NUCLEO-F303RE**. The new spec names **BlackPill STM32F411CE** (default). These are different MCUs, packages, headers, timers and toolchain targets. **This firmware targets F411CE, NOT the photographed F303RE, and must not be flashed onto that Nucleo.**

F411CE 48-pin package has TIM1/2/3/4/5/9/10/11. **TIM2/TIM5 share accessible PA0/PA1 encoder CH1/2 locations on the 48-pin board; allocating four hardware quadrature encoder timers simultaneously with four PWM, two independent UARTs and USB/SWD avoidance is not presently established.** TIM2/TIM3/TIM4 hardware encoder + fourth GPIO quadrature using EXTI is the proposed fallback. Resource analysis must be redone using the confirmed actual board and pin-level wiring.

| Function | Proposed pin | Peripheral | AF | Status |
|---|---|---|---|---|
| FL encoder A/B | PA0/PA1 | TIM2 CH1/CH2 | AF1 | Proposed |
| FR encoder A/B | PA6/PA7 | TIM3 CH1/CH2 | AF2 | Proposed |
| RL encoder A/B | PB6/PB7 | TIM4 CH1/CH2 | AF2 | Proposed |
| RR encoder A/B | TODO | EXTI software decoder | GPIO input | **UNRESOLVED** |
| FL PWM | PA8 | TIM1 CH1 | AF1 | Proposed |
| FR PWM | PB14 | TIM12? **NOT AVAILABLE** | N/A | **REJECTED**; no allocation without verified timer AF |
| RL PWM | PB8 | TIM10 CH1 | AF3 | Proposed |
| RR PWM | PB9 | TIM11 CH1 | AF3 | Proposed |
| Host UART | PA2 TX / PA3 RX | USART2 | AF7 | Proposed |
| iBUS UART RX | TODO | USART1 | AF7 | **UNRESOLVED** if TIM4 PB6/PB7 occupied |
| IMU I2C | TODO | I2C1 | AF4 | Unresolved |
| HC-SR04 1..3 TRIG/ECHO | TODO | GPIO/EXTI/timer | TODO | Unresolved |
| Battery ADC | TODO | ADC1 | analog | Unresolved |
| 4x current sense ADC | TODO | ADC1 | analog | Unresolved |
| 4x direction, sleep, fault | TODO | GPIO/EXTI | GPIO | Unresolved |
| NC E-stop | TODO | GPIO EXTI, pull-up | GPIO | Unresolved |
| SWD | PA13, PA14 | SWD | AF0 | RESERVED |
| USB | PA11, PA12 | USB FS | AF10 | RESERVED |
| BOOT0 | BOOT0 | Boot mode | N/A | RESERVED |

No mutually consistent complete allocation is claimed. **Never wire from this table** until pin and peripheral conflicts are resolved.
