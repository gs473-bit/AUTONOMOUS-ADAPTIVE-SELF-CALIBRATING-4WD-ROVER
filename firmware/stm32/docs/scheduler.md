# Cooperative 1 kHz scheduler (initial integration)
| Task | Target period | Executed now | Estimate of F303RE CPU time |
|---|---|---|---|
| USART2 DMA receive drain + packet parsing | every 1 ms | yes | <0.3 ms typical (estimate) |
| Safety state checks | every 1 ms | yes | <0.05 ms (estimate) |
| Wheel encoder capture + PID | 5 ms / 200 Hz | NOT hardware-enabled | <0.3 ms (estimate) |
| Pi MPU6050 acquisition | 5 ms / 200 Hz | Pi only | 0 on STM32 |
| Pi HC-SR04 | sequential >=60ms | Pi only | 0 on STM32 |
| Telemetry | 50 ms / 20 Hz | yes, validity mask 0 | TX currently bounded 10ms |
| IWDG refresh | healthy main loop | TODO, NOT INITIALIZED | Not safety qualified |

Estimates are not profiled measurements. USART2 TX is bounded blocking, not DMA; must be made asynchronous and scheduler load validated before using actual motor power. Current app always keeps motor outputs unconfigured.
