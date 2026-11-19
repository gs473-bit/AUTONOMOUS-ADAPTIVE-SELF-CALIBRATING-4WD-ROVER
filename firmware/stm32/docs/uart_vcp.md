# NUCLEO-F303RE link
Target USART2 PA2 TX / PA3 RX is routed to ST-LINK/V2-1 USB VCP (as configured by MB1136 solder bridges). Pi attaches USB cable, **not** Pi GPIO UART pins. Set UART 115200 8N1; non-blocking circular DMA RX uses DMA1_Channel6 (verify against RM0316 and ST-LINK hardware revision). TX is currently bounded blocking (10ms timeout), not DMA; replace and stress-test before real motion.

Check `ls -l /dev/ttyACM*` and `udevadm info -q property -n /dev/ttyACM0` on the Pi. No commands may enable motors while `AETHER_ACTUATION_ENABLED=0`. Loss of USB/host must force STM32-local timeout; firmware state tests are mocked only. Dedicated Linux heartbeat alone does not suffice; motion command age is separately enforced in STM32 safety.
