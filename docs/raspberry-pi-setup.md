
# Raspberry Pi Software Setup

## 1. Purpose

This guide prepares the Raspberry Pi for the ECE357 Autonomous Adaptive Self-Calibrating 4WD Rover software.

The setup does not assume that motors, sensors, or the camera are connected or operational.

## 2. Before You Start

Confirm that you have:

- Access to the project repository.
- A Raspberry Pi with a compatible Raspberry Pi OS installation.
- Network access for installing Python packages.
- Permission to use the Raspberry Pi.
- The actual hardware inventory and wiring records, when available.

Do not connect or power unverified hardware based only on this software guide.

## 3. Prepare the Operating System

1. Boot the Raspberry Pi.
2. Complete the operating-system setup.
3. Connect to a trusted network.
4. Open a terminal on the Raspberry Pi.
5. Confirm that the system date and time are correct.

## 4. Obtain the Project

Clone the team's repository using its actual GitHub URL:

    git clone YOUR_REPOSITORY_URL
    cd YOUR_REPOSITORY_DIRECTORY

Replace both placeholders with the real repository URL and directory name.

## 5. Create a Python Environment

From the project root, run:

    python3 --version
    python3 -m venv .venv
    source .venv/bin/activate
    python -m pip install --upgrade pip
    python -m pip install -e ".[dev]"

If virtual-environment creation fails because of missing system packages, consult the documentation for your installed Raspberry Pi OS version before installing additional packages.

## 6. Run the Software Tests

With the virtual environment active, run:

    python -m pytest -v

Review the output carefully.

A successful run indicates that the tests passed in that software environment. It does not prove that the physical rover, sensors, motor drivers, or camera work.

## 7. Review the Configuration

Inspect:

- `config/rover.yaml`
- `docs/hardware-inventory.md`
- `docs/wiring-record.md`
- `docs/camera-setup.md`
- `docs/test-plan.md`

Treat any unverified pin assignments, component specifications, or wiring details as provisional.

## 8. Hardware Safety

Before connecting hardware:

- Verify each component's exact model and voltage/current ratings.
- Confirm the power distribution and common-ground requirements.
- Protect Raspberry Pi GPIO pins from signals above their permitted voltage.
- Level-protect an HC-SR04 ECHO signal if the selected module outputs 5 V.
- Verify motor-driver wiring and motor supply arrangements before applying power.
- Keep the physical emergency stop accessible during powered testing.

Do not run motor-control code until the wiring and power system have been checked.

## 9. Camera Readiness

The USB camera diagnostic script is intended for compatible USB/UVC cameras only.

Do not assume it supports a CSI camera or an OV7670 module. Confirm the actual camera model and interface before choosing its software setup.

## 10. Record the Setup

Record the following in `docs/build-log.md`:

- Raspberry Pi model and operating-system version.
- Python version.
- Dependency-installation result.
- Test results and any errors.
- Confirmed hardware models and outstanding questions.

Do not record passwords, API keys, or other secrets in the repository.
