# Smartphone teleoperation — Pi only, no FlySky receiver
**STATUS: host-mocked UI; not hardware-verified. Firmware motor actuation is disabled.**
```text
Phone browser (authenticated HTTPS/VPN)
    -> Raspberry Pi localhost phone service (HTTP 127.0.0.1:8080)
    -> HeartbeatLink over ST-LINK USB serial (/dev/aether_stm32)
    -> NUCLEO-F303RE safety -> four DRV8876 drivers (still disabled)
```
A browser STOP is a **software command**, not a physical emergency stop. The Nucleo command lease is 200ms and remains authoritative when Pi, phone, Wi-Fi or USB fails. All movement requires separate explicit ARM after every reconnect.

## Setup (with motor VM DISCONNECTED)
1. Install package with its virtualenv according to `docs/bringup.md`; run native host tests.
2. Generate a strong random token:
   ```bash
   python3 -c 'import secrets; print(secrets.token_urlsafe(32))'
   ```
3. Put token in a root-controlled `/etc/aether/phone.env` file, readable only by root, e.g.:
   ```ini
   AETHER_PHONE_TOKEN=<replace-with-random-token>
   AETHER_PHONE_PORT=8080
   AETHER_PHONE_ENABLE_MOTION=0
   # DO NOT SET ACTUAL MOTION LIMITS until calibrated with wheels lifted
   # AETHER_PHONE_MAX_VX=<measured-m/s>
   # AETHER_PHONE_MAX_VY=<measured-m/s>
   # AETHER_PHONE_MAX_WZ=<measured-rad/s>
   ```
   Do not commit this secret file. Use `chmod 600` and a safe deployment process.
4. The service listens ONLY on Pi loopback `127.0.0.1:8080`. To reach the UI from a smartphone, configure an authenticated **HTTPS reverse proxy or private VPN/tunnel** to loopback. This is intentionally not exposed as plain HTTP to public Wi-Fi/Internet. Remote access mechanism is **TODO** until Pi OS/network is confirmed.
5. From the phone, open the secure URL and enter the token. ARM/DRIVE remain rejected with HTTP 423 while motion commissioning is off. STOP/DISARM is allowed for an authenticated client even when motion is locked.
6. UI buttons send normalized forward/lateral/rotation commands while held (100ms repeat), DISARM on release and page hiding. Requests may be dropped, so a physical E-stop and the STM32 command timeout are essential.
7. After full physical validation, enter measured positive max velocities and a deliberate opt-in `AETHER_PHONE_ENABLE_MOTION=1`. **Do not do so just to make the UI move**, and do not bypass the STM32's `AETHER_ACTUATION_ENABLED=0` until hardware safety tests pass.

No camera stream, autonomous path planner, secure reverse proxy, hardware-safe arming acknowledgement, or full telemetry dashboard is claimed for this UI yet. This interface only provides STOP, ARM request and short-lived manual DRIVE packets in the software architecture.
