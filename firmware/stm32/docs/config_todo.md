# Hardware configuration hold points
**The firmware cannot arm with current default placeholders.**
1. Confirm MCU NUCLEO-F303RE and proposed AF/pin map in docs/pinmap.md against photographed board and its ST-LINK solder bridges.
2. Record four motor label suffixes, gear ratios and x4 encoder counts **at wheel output**; measure by manually rotating one full output revolution and tallying quadrature transitions.
3. Measure loaded Mecanum wheel radius (nominal diameter 80mm is NOT calibrated), longitudinal wheelbase and track width between centers.
4. Measure each DRV8876 carrier's current-sense scaling, VREF and nSLEEP behavior; confirm PH/EN PMODE=LOW. Choose safe current thresholds based on thermal and motor data.
5. Confirm battery 3S2P Wattnine 9.2–12.6V pack label current '4A': obtain BMS continuous/peak/disconnect data; measure voltage-divider resistors and ADC Vref. Never infer low/critical voltages from photo alone.
6. Confirm 3 HC-SR04 modules or different quantity; earlier plan located ultrasound and MPU6050 on Pi. Choose **one host for each**. Measure ECHO resistor divider with a meter before connecting.
7. Confirm FlySky FS-iA6B receiver and iBUS output voltage, RC switch mapping and deadzone; verify manual override policy.
8. Draw and electrically verify normally-closed E-stop switch that independently **cuts motor supply**; test open wire=inactive drive. Confirm physical fuse and motor supply disconnect.
9. Validate UART level compatibility, actual Pi-Nucleo pin pair, baud rate and the packet spec.
10. Prove code-level watchdog, current sensing and battery thresholds on real electrical test hardware before changing AETHER_ACTUATION_ENABLED.
