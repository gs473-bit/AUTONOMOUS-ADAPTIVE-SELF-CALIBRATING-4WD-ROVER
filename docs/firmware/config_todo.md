# AETHER X01 unresolved hardware measurements
Current firmware is intentionally **non-actuating**.
- Measure effective loaded 80 mm nominal wheel radius, front/rear wheel-center distance (wheelbase) and left/right center spacing (track width).
- Count output shaft quadrature transitions for one full output rotation (x4 convention); verify gearbox ratio against *your* 12V/60rpm motor, not generic 25GA370 datasheet.
- Measure IPROPI volts/amp and driver's current limit at actual nSLEEP/VREF; obtain thermals and fuse values.
- Verify 3S2P 12V nominal 4000mAh 4A-marked NMC pack output/charging barrel polarity, BMS continuous/peak current and low/critical voltage values; design/divide ADC to <=3.3V at 12.6V.
- Measure NC E-stop circuit such that breaking the loop removes motor power, and verify logic input is fail-open.
- Confirm Pi IMU GY-521 logic voltage, HC-SR04 divider and number/positions, Pi Camera cable/model.
- Confirm whether iBUS receiver is FS-iA6B and whether it is wired to STM32 USART3 or another connector; exact RC channel mapping and switch semantics.
- Confirm verified header position for every motor pin. Resolve nFAULT EXTI line collision in candidate pinmap and PB3/PB4 debug modes; assign ADC DMA and USART2 RX DMA without conflicts.
- Pi model RAM and Raspberry Pi OS version.
