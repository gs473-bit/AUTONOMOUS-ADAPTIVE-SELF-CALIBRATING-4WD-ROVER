# AETHER-X 01 — NUCLEO-F303RE pin allocation
**PROPOSED: verify against my actual wiring before flashing.**
The photographed NUCLEO-F303RE has STM32F303RET6 (LQFP64), not a BlackPill.
*Clock:* CPU 72 MHz maximum; peripheral clocks/timer dividers require measurement/firmware configuration.
*Sources:* [STM32F303RE datasheet pin AF matrix](https://www.st.com/resource/en/datasheet/stm32f303re.pdf), [Nucleo-64 UM1724 (MB1136)](https://www.st.com/resource/en/user_manual/dm00105823.pdf).

| Function | MCU pin | Timer/peripheral/channel | AF | Header candidates | Notes |
|---|---|---|---|---|---|
| Encoder FL A/B | PA0 / PA1 | TIM2 CH1 / CH2 (32-bit) | AF1 | Arduino A0/A1, CN8/CN7 | Independent encoder mode |
| Encoder FR A/B | PA6 / PA7 | TIM3 CH1 / CH2 (16-bit) | AF2 | Arduino D12/D11, CN5/CN10 | Confirm AF and cable |
| Encoder RL A/B | PB6 / PB7 | TIM4 CH1 / CH2 (16-bit) | AF2 | Arduino D10 + CN7 | Not I2C1 |
| Encoder RR A/B | PC6 / PC7 | TIM8 CH1 / CH2 (16-bit) | AF4 | CN10 / Arduino D9 for PC7 | Does not use TIM8 for PWM |
| PWM FL / FR / RL | PA8 / PA9 / PA10 | TIM1 CH1 / CH2 / CH3 | AF6 | Arduino D7/D8 + CN10 | Single timer 20 kHz candidate |
| PWM RR | PB9 | TIM17 CH1 | AF1 | Arduino D14 / CN5 | **Different timer** to avoid MCU USB PA11/TIM1_CH4 |
| Direction FL FR RL RR | PC8 / PC9 / PC10 / PC11 | GPIO | — | CN7/CN10 | Proposed separate pins |
| nSLEEP FL FR RL RR | PB3 / PB4 / PB5 / PB12 | GPIO | — | CN10 | PB3/PB4 debug/JTAG side functions; no SWO/JTAG use; external pulldowns required |
| nFAULT FL FR RL RR | PC4 / PC5 / PB10 / PA4 | GPIO/EXTI | — | CN7/CN10 / Arduino A2 | EXTI lines 4,5,10,4 **CONFLICT**: PC4 and PA4 share EXTI4! Poll PA4 or relocate nFAULT; verify before wiring |
| IPROPI FL FR RL RR | PB0 / PB1 / PC0 / PC1 | ADC1 scan | analog | Arduino A3/A5 and CN7/CN10 | Calibration and DMA channel pending |
| Battery divider | PC2 | ADC1 scan | analog | CN7 | Resistor values TODO |
| E-stop NC sense | PB13 | GPIO input with pullup | — | CN10 | Open loop must latch ESTOP; separate contact cuts motor VM |
| Host USB VCP | PA2 TX / PA3 RX | USART2 | AF7 | ST-LINK/V2-1 solder-bridge path | Target pins remain reserved exclusively for VCP |
| Optional iBUS RX | PB11 | USART3 RX | AF7 | CN10 | Receiver/type/logic level TODO; disabled by default |
| Debug | PA13 / PA14 | SWD | AF0 | ST-LINK | Reserved |
| Pi camera, MPU6050, HC-SR04 | None | Pi side | — | Pi GPIO/CSI | Do not connect to STM32 |
| User LED/button | PA5 / PC13 | GPIO | — | On-board | Reserved |
| MCU USB data | PA11 / PA12 | USB FS | AF14 | Target MCU | Reserved |
| HSE/LSE | PF0/PF1, PC14/PC15 | Oscillators | — | CN7 | Reserved |
| BOOT0 | BOOT0 | Boot mode | — | On board | Reserved |

Four hardware quadrature timers fit: TIM2, TIM3, TIM4, TIM8. Four PWM outputs fit on TIM1 CH1/2/3 plus TIM17 CH1. **Four channels on TIM1 alone would consume PA11 (MCU USB)**, so this proposal deliberately avoids that USB pin.

**Unresolved (hard NO-GO):** nFAULT EXTI4 conflict above, actual CN7/CN10 hole numbers, GPIO alternate functions at flash time, ADC1 DMA1 channel ownership and ADC-to-IPROPI voltage scaling, ST-LINK VCP wiring jumpers and nSLEEP startup pull-downs. Code stays non-actuating. Verify against actual physical netlist and regenerate this map before attaching motor power.
