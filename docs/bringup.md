# AETHER-X 01 build, Pi / Nucleo bring-up — no hardware tests yet
All tests below are planned. **PROPOSED: verify pinmap and assembly before flashing.** *Never connect motor VM before steps 1–9 are passed, and do not remove the compile-time interlock while hardware values remain TODO.*

1. **Install toolchain (on your laptop or Pi with PlatformIO).** PlatformIO and Python 3.10+ required.
   ```bash
   python3 -m pip install --user platformio
   ```
   **PASS:** `pio --version` prints a version. If unavailable, use a venv or the VS Code PlatformIO extension.

2. **Compile only, with motor power DISCONNECTED.**
   ```bash
   cd firmware/stm32
   ```
   ```bash
   pio run -e nucleo_f303re
   ```
   **PASS:** `SUCCESS`, file `.pio/build/nucleo_f303re/firmware.bin`. No STM32F411 build may be used.

3. **Run native host-mocked C tests.**
   ```bash
   pio test -e native
   ```
   **PASS:** every Unity test `PASSED`, no compilation warnings/errors. These are **host-mocked**, not real rover tests.

4. **Flash via Nucleo USB/ST-LINK, with motor supplies and driver inputs physically disconnected.**
   ```bash
   pio run -e nucleo_f303re -t upload
   ```
   **PASS:** ST-LINK programming verify successful. *Fallback only if the Nucleo mass-storage drive mounts*: copy `.pio/build/nucleo_f303re/firmware.bin` to its ST-LINK drive and verify device reset. Never flash a BlackPill binary.

5. **Power the Pi from its independent regulated 5.1V/3A source; verify OS.**
   ```bash
   cat /proc/device-tree/model
   ```
   **PASS:** `Raspberry Pi 4 Model B`.
   ```bash
   cat /etc/os-release
   ```
   **PASS:** supported 64-bit Pi OS; **Bookworm is only an assumption**.

6. **USB virtual COM enumeration.** Connect Pi USB-A to Nucleo ST-LINK USB.
   ```bash
   ls -l /dev/ttyACM*
   ```
   **PASS:** `/dev/ttyACM0` or another AC M interface exists. Identify the exact ST-LINK with `udevadm info -q property -n /dev/ttyACM0`. Device number may change. Set up the reviewed udev template.

7. **Pi software and host tests.**
   ```bash
   python3 -m venv --system-site-packages .venv
   ```
   ```bash
   . .venv/bin/activate
   ```
   ```bash
   python -m pip install -e ".[dev]"
   ```
   ```bash
   python -m pytest -v
   ```
   **PASS:** all tests pass. They are **host-mocked**.

8. **MPU6050 alone.** Confirm GY-521 3.3V supply and SDA/SCL pull-ups, enable Pi I2C through `sudo raspi-config`, reboot.
   ```bash
   sudo i2cdetect -y 1
   ```
   **PASS:** `68` (or verified `69`) seen at address. An address is only enumeration; read sample/perform stationary gyro calibration separately.

9. **Four HC-SR04 units and camera, each separately.** Count is confirmed as FOUR. Assign four unique Pi BCM TRIG/ECHO pairs only after documenting physical mounting. Fit FOUR independent ECHO dividers, measure each ~3.0V high with meter, keep Pi GPIO disconnected until then. Verify one sensor at a time via `UltrasonicArray.from_config()`; round-robin testing uses >=60ms ping separation (about <=4Hz/sensor) to limit cross-talk, not 20Hz per sensor. Pi OS/driver compatibility is still unverified. Camera:
   ```bash
   rpicam-hello --list-cameras
   ```
   **PASS:** compatible CSI sensor listed. Image capture must be separately verified.

10. **Encoder hand-turn test with motor power OFF.** Before applying encoder VCC, verify 3.3V compatibility with datasheet. Hardware timer mapping is proposed only.
    **PASS:** monotonic A/B counts and reversible sign under hand rotation, no overflow spikes. The current DISARM-only firmware does not yet export valid encoder readings; **NO-GO until it does**.

11. **One motor with WHEELS LIFTED — DANGEROUS, NOT YET APPROVED.** Requires tested physical E-stop, fuse, current-limited bench supply, firmware nFAULT/ADC/encoder validation, and externally low nSLEEP. Disconnect battery and use bench supply only.
    **PASS:** documented current, direction, low-duty response, thermal stability. **Current firmware actuation is compiled OFF.**

12. **Link timeout / USB unplug — DANGEROUS when motors powered.** Verify independently on stands that the Nucleo disables nSLEEP within the configured command timeout even with Pi dead. Electrical disable within 200ms is required; stopping *mechanical motion* within 200ms cannot be assumed.
    **PASS:** scope/logic-analyzer nSLEEP and PWM, plus motor current measurement.

13. **E-stop — DANGEROUS when powered.** Open normally-closed hardware contact, verify all four motor VM feeds drop and GPIO E-stop latches. Wire-break must be detected. Manual switch release alone must NOT rearm.
    **PASS:** power cut without relying on Pi/USB and explicit rearm required.

14. **Smartphone UI / closed-loop speed / ground test.** Phone uses an authenticated Pi-only local web endpoint (see `docs/smartphone_control.md`), NEVER iBUS. Verify manual STOP and loss of Wi-Fi/phone input with **motor power disconnected first**. Enable drive only after speed limits, motor supply, E-stop, current sense and timeout are physically qualified. Tune PID on stands, then start low-speed ground trials.
    **PASS:** no unsafe overspeed/stall/overcurrent; only then controlled low-speed ground run.

**Systemd (only after reviewed service path and username):**
```bash
sudo cp deploy/aether.service /etc/systemd/system/aether.service
```
```bash
sudo systemctl daemon-reload
```
```bash
sudo systemctl enable --now aether.service
```
```bash
sudo journalctl -u aether.service -f
```
STOP: `sudo systemctl stop aether.service`. REBOOT: `sudo reboot`. SHUTDOWN: `sudo shutdown -h now`. Git rollback: `git log --oneline -5` then `git switch --detach <known-good-sha>` (deploy only after repeat tests). If Pi freezes, use E-stop before power recovery; never unplug motor-control USB expecting a guaranteed mechanical brake.
