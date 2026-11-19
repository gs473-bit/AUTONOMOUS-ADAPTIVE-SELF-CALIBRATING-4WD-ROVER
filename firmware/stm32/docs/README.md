# Firmware structure and build status
PlatformIO chosen for reproducible STM32Cube HAL builds and Unity native tests.\nSTM32Cube HAL provides the F303RE peripheral drivers and ST-LINK upload route.\nHardware actuation is disabled until pin/wiring and battery safety checks are signed off.

Build (from `firmware/stm32`): `pio run -e nucleo_f303re`; host tests: `pio test -e native`.
The board bootstrap initializes HAL only and never enables motor outputs.
