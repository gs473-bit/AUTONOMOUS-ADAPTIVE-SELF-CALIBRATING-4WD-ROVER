# Autonomous Adaptive Self-Calibrating 4WD Rover

> **ECE357 Capstone Project — Group 19**  
> **SEE. MAP. PLAN. ADAPT.**

An autonomous 4-wheel-drive (4WD) rover concept built around a Raspberry Pi 4B. The project brings together camera-assisted mapping, path planning, obstacle avoidance, multi-sensor state estimation, wheel-efficiency calibration, and fault-tolerant operation.

**Project status:** Hardware integration and experimental validation are planned/in progress. Features described here are project objectives unless explicitly marked as implemented and tested. No experimental results are claimed in this README.

## Table of Contents

- [Project Overview](#project-overview)
- [Objectives](#objectives)
- [System Architecture](#system-architecture)
- [Planned Features](#planned-features)
- [Hardware and Components](#hardware-and-components)
- [Software Stack](#software-stack)
- [Repository Structure](#repository-structure)
- [Getting Started](#getting-started)
- [Development Roadmap](#development-roadmap)
- [Testing and Validation](#testing-and-validation)
- [Safety](#safety)
- [Known Design Considerations](#known-design-considerations)
- [Team](#team)
- [Contributing](#contributing)
- [License](#license)

## Project Overview

The rover is designed to observe its surroundings, build a usable representation of the environment, plan a route, move using independently controlled wheels, detect unexpected obstacles, and adapt when wheel performance changes or a motor channel becomes unavailable.

The Raspberry Pi 4B is intended to act as the central computing host for vision, mapping, navigation, sensor processing, motor commands, and the smartphone control interface. The design aims to avoid a separate intermediary microcontroller.

### Core workflow

1. **Observe** — Capture images from the selected camera.
2. **Map** — Extract visual information and update a 2D representation of the arena.
3. **Plan** — Find a low-cost route from the estimated rover position to a target.
4. **Navigate** — Generate commands for the four wheel channels.
5. **Detect** — Use vision and ultrasonic ranging to identify obstacles.
6. **Adapt** — Stop or slow down, update the map, re-plan, and apply calibrated wheel commands where possible.

## Objectives

- Develop camera-assisted environmental mapping using OpenCV and ArUco markers.
- Plan a traversable route through a mapped environment.
- Detect unexpected obstacles and trigger local avoidance and route recomputation.
- Combine camera, ultrasonic, inertial, and wheel-feedback measurements for approximate state estimation.
- Estimate differences in wheel response and compensate motor commands.
- Investigate degraded operation after motor-channel failure, subject to safe and experimentally validated constraints.
- Provide smartphone-based Wi-Fi control with manual, autonomous, and stop/hold modes.
- Document hardware, wiring, software setup, integration decisions, test procedures, and results.

## System Architecture

```text
                   ┌─────────────────────────┐
                   │     Camera Module       │
                   └────────────┬────────────┘
                                │ image frames
┌────────────────┐              ▼
│ HC-SR04 Range  │───┐  ┌─────────────────────┐
└────────────────┘   │  │                     │
                     ├─►│   Raspberry Pi 4B   │
┌────────────────┐   │  │                     │
│ MPU6050 IMU    │───┤  │ Vision / Mapping    │
└────────────────┘   │  │ Path Planning       │
                     │  │ Sensor Processing   │
┌────────────────┐   │  │ Control / Telemetry │
│ Wheel Encoders │───┘  └──────────┬──────────┘
└────────────────┘                 │
                                   ▼
                         ┌───────────────────┐
                         │ 2 × L298N Drivers │
                         └─────────┬─────────┘
                                   ▼
                         ┌───────────────────┐
                         │ 4 DC Gear Motors  │
                         └───────────────────┘

        Smartphone ───── Wi-Fi ───── Raspberry Pi
```

This is a conceptual architecture. Actual pin assignments, voltage levels, camera interface compatibility, driver wiring, and power requirements must be confirmed against the exact hardware selected before connection.

## Planned Features

### 1. Camera-assisted mapping

The camera supplies image frames for visual processing. OpenCV and ArUco markers are proposed for extracting landmark and position information and building an approximate map.

The project does **not** currently claim a particular full SLAM implementation or millimetre-accurate localization. Mapping quality and localization drift must be measured experimentally.

### 2. Path planning

The planner is intended to use a grid/map, an estimated rover position, and a destination to calculate a low-cost traversable path. The path is represented as a sequence of waypoints for the motion controller.

The specific search algorithm and map-update policy should be recorded in the implementation documentation once selected and tested.

### 3. Dynamic obstacle avoidance

Vision provides environmental context, while ultrasonic sensors provide short-range distance measurements. A detected obstruction should trigger a safe stop or slowdown, map update, route recomputation, and controlled resumption only when the route is safe.

### 4. Multi-sensor state estimation

The planned sensor roles are:

| Sensor | Intended role |
|---|---|
| Camera | Visual landmarks, ArUco markers, arena boundaries, and environmental context |
| HC-SR04 ultrasonic sensor(s) | Short-range obstacle distance |
| MPU6050 IMU | Angular rate and acceleration measurements for heading/orientation estimation |
| Wheel encoder feedback | Wheel ticks, relative wheel motion, and speed estimation |

Sensor fusion is intended to improve the rover's operational state estimate. Accuracy, drift, update rates, and reliability remain to be characterized through tests.

### 5. Wheel self-calibration

Encoder feedback can be compared with commanded wheel motion to estimate per-wheel response differences. Calibration factors may then be applied to motor commands to improve straight-line tracking and turning consistency.

Any numerical calibration values in planning slides or examples are illustrative only. Actual coefficients must be derived from repeatable experiments on the assembled rover.

### 6. Fault-tolerant operation

The project investigates whether the rover can retain useful mobility after a motor or motor channel becomes unavailable. The response must be determined from the actual drivetrain, available wheel feedback, and tested control strategy.

Degraded operation is **not guaranteed**. Reduced speed, wider turns, a clear operating corridor, and a safe-stop fallback may be necessary. The proposed diagonal-failure configuration must be validated before it is relied on.

### 7. Smartphone control

A planned Wi-Fi interface will support manual directional commands, speed control, emergency stop, and mode selection. Telemetry may include rover mode, sensor state, motor state, and navigation information.

## Hardware and Components

The following list reflects the current project concept. Exact models, quantities, electrical ratings, and availability must be confirmed before assembly.

| Component | Intended purpose | Status / notes |
|---|---|---|
| Raspberry Pi 4B | Main embedded computer | Selected in project architecture |
| Camera module | Image acquisition for mapping | Exact module and Pi interface compatibility must be confirmed |
| OV7670 VGA camera | Candidate camera module | Do not assume direct compatibility with Raspberry Pi without verifying the interface and driver path |
| 2 × L298N motor driver boards | Four motor channels across two boards | Current, voltage drop, and thermal performance require validation |
| 4 × DC gear motors | Independent 4WD actuation | Motor voltage, stall current, gearing, and encoder availability to be confirmed |
| HC-SR04 ultrasonic sensor(s) | Short-range distance measurement | Quantity and mounting positions to be confirmed |
| MPU6050 | Inertial measurements | I²C configuration and calibration required |
| Wheel encoder / magnetic or IR sensors | Wheel motion feedback | Exact encoder design and pulses per revolution to be confirmed |
| 9 g servo | Proposed mechanical/sensor actuation | Final role and power needs to be confirmed |
| Battery | Rover power source | Voltage, chemistry, capacity, current capability, and protection to be selected |
| Buck converter | Regulated logic supply | Must be rated for the Pi and all attached peripherals |
| Physical emergency stop | Hardware-level motor shutdown | Implementation and wiring must be verified |

### Hardware decisions to document

For every selected part, add its exact manufacturer/model, datasheet link, quantity, operating voltage, current requirements, connector/interface, and test status to the hardware documentation.

## Software Stack

The planned software stack includes:

- **Operating system:** Raspberry Pi-compatible Linux distribution (final version to be recorded).
- **Language:** Python is a proposed option for high-level vision, mapping, and navigation; final implementation language and versions must be documented.
- **Computer vision:** OpenCV.
- **Visual markers:** ArUco markers, using an OpenCV build/module that supports the required functionality.
- **Hardware interfaces:** GPIO, I²C, and the verified camera interface as applicable.
- **Motor control:** PWM and direction control through the selected driver boards.
- **Communication:** Local Wi-Fi interface for smartphone control.

Exact package versions, installation commands, GPIO assignments, and service configuration should be added after the hardware and software environment are confirmed. Do not copy unverified pin numbers or camera setup commands into production instructions.

## Repository Structure

The repository can be organized as follows as files are added:

```text
autonomous-adaptive-4wd-rover/
├── README.md
├── docs/
│   ├── system-architecture.md
│   ├── bill-of-materials.md
│   ├── wiring-and-pinout.md
│   ├── camera-setup.md
│   ├── software-setup.md
│   ├── calibration-procedure.md
│   ├── safety-and-power.md
│   └── test-plan.md
├── hardware/
│   ├── schematics/
│   ├── wiring-diagrams/
│   └── datasheets/
├── mechanical/
│   ├── cad/
│   └── drawings/
├── software/
│   ├── camera/
│   ├── mapping/
│   ├── localization/
│   ├── path_planning/
│   ├── obstacle_avoidance/
│   ├── motor_control/
│   ├── calibration/
│   ├── sensors/
│   └── smartphone_interface/
├── tests/
│   ├── hardware/
│   ├── integration/
│   └── results/
└── media/
    ├── photos/
    ├── diagrams/
    └── demo-videos/
```

This is a proposed layout, not a claim that these files or implementations already exist.

## Getting Started

### Prerequisites

Before running the rover software, confirm that you have:

- A Raspberry Pi 4B and a compatible, tested power supply.
- A selected camera and a verified connection path.
- The motor drivers, motors, sensors, and encoders that are actually available.
- A motor power supply appropriate for the motors and drivers.
- A common signal ground where required by the chosen circuit design.
- A physical method to stop motor power safely.
- A Linux/Python environment and version-controlled project files.

### Initial setup

1. **Clone the repository**

   ```bash
   git clone <REPOSITORY_URL>
   cd autonomous-adaptive-4wd-rover
   ```

   Replace `<REPOSITORY_URL>` with the repository's actual Git URL.

2. **Record the platform**

   Document the Raspberry Pi model, OS release, Python version, camera model, and connected hardware.

3. **Verify the camera independently**

   Follow [`docs/camera-setup.md`](docs/camera-setup.md) once created. Confirm the exact camera interface and capture method before installing camera-specific software or attempting integration.

4. **Verify each sensor independently**

   Test ultrasonic readings, IMU communication, and encoder tick counting separately. Record the test conditions and observed results.

5. **Verify motor control with wheels raised**

   Check each motor channel at a conservative command level, confirm direction and PWM response, and monitor driver temperature and current. Keep the rover physically supported so the wheels cannot propel it unexpectedly.

6. **Integrate incrementally**

   Bring up one subsystem at a time, then validate combined operation. Do not run autonomous navigation until emergency stopping, sensor checks, and basic motion control have been verified.

> **Note:** A complete executable installation guide cannot be finalized until the exact camera, motor, encoder, and power hardware and the software implementation are confirmed.

## Development Roadmap

- [ ] Confirm component availability and exact hardware models.
- [ ] Finalize the bill of materials and datasheet collection.
- [ ] Finalize power distribution, grounding, and physical emergency stop.
- [ ] Confirm the camera interface and demonstrate reliable frame capture.
- [ ] Bring up and test each sensor independently.
- [ ] Test all four motor channels and measure current/temperature under load.
- [ ] Implement and validate encoder-based wheel feedback.
- [ ] Implement visual marker detection and mapping.
- [ ] Implement route planning on a known test map.
- [ ] Implement obstacle detection, safe stopping, and re-planning.
- [ ] Integrate sensor state estimation and wheel calibration.
- [ ] Build and test the smartphone control interface.
- [ ] Evaluate motor-failure behavior and document safe operating limits.
- [ ] Run the full validation plan and publish measured results.
- [ ] Add CAD files, wiring diagrams, photographs, and demonstration videos.

## Testing and Validation

The following tests are planned. Mark a test as complete only after the procedure has been run and evidence has been recorded.

| Test | What to record | Status |
|---|---|---|
| Camera image acquisition | Camera model, resolution, frame rate, sample images, failures | Planned |
| Visual marker detection | Detection rate, lighting and distance conditions | Planned |
| Mapping and map updates | Arena layout, map output, repeatability | Planned |
| Path planning | Start/goal, obstacles, computed route, completion | Planned |
| Dynamic obstacle response | Detection distance, stopping behavior, re-planning | Planned |
| Ultrasonic ranging | Reference distance vs measured distance | Planned |
| IMU calibration | Bias, drift, orientation behavior | Planned |
| Encoder feedback | Pulses per revolution, speed and distance error | Planned |
| Wheel self-calibration | Before/after tracking error and calibration factors | Planned |
| Straight-line tracking and turning | Distance, heading error, repeatability | Planned |
| Motor driver thermal/current test | Supply voltage, current, driver temperature, load | Planned |
| Power stability | Pi stability, voltage under load, resets | Planned |
| Smartphone control | Command latency, disconnect handling, stop behavior | Planned |
| Motor-failure response | Failure scenario, safe state, remaining mobility | Planned |
| Software fault and emergency stop | Trigger, stop response, recovery behavior | Planned |

Include logs, plots, photos, test conditions, and a short conclusion for each completed test. Clearly distinguish simulated, illustrative, and measured results.

## Safety

- Use a physical emergency stop that interrupts motor power or the driver-enable path independently of the main navigation process, as appropriate to the final circuit.
- Keep the Pi and peripheral logic supply within their specified voltage and current limits.
- Do not power motors directly from Raspberry Pi GPIO pins.
- Check the motor driver's current rating, voltage drop, and thermal limits against the selected motors and real load.
- Confirm voltage-level compatibility before connecting sensor signals to GPIO.
- Test motor direction and emergency stopping with the rover raised or otherwise restrained.
- Add timeout behavior for lost commands and define a safe response to invalid sensor data or software failure.
- Disconnect power before changing wiring.
- Never treat software obstacle avoidance as a replacement for a hardware emergency stop.

## Known Design Considerations

- **Camera selection:** The project materials list an OV7670 VGA module and also describe a Pi Camera. These are not interchangeable assumptions; confirm the actual module, interface, driver support, and acquisition method before implementation.
- **Mapping scope:** OpenCV and ArUco markers are proposed for visual mapping. A complete SLAM solution is not specified in the current project definition.
- **Motor drivers:** L298N boards can incur significant voltage drop and heat. Measure real motor current and driver temperature under representative load before committing to the design.
- **Localization:** Camera, IMU, ultrasonic, and encoder measurements can improve estimates but do not guarantee drift-free or high-precision localization.
- **Calibration:** Calibration values must be measured on the assembled rover and the intended surface; example values are not experimental results.
- **Fault tolerance:** Continued movement after a motor failure depends on the mechanical layout, remaining actuators, control strategy, and safe operating conditions.
- **Power design:** Battery voltage and capacity, motor specifications, and converter ratings are not finalized in the project definition and must be established before final wiring.

## Team

**ECE357 Capstone Project — Group 19**

- **Gaurav Singh**
- **Jia Malik**
- **Vedansh Sharan**

## Contributing

This repository is the shared engineering record for the project. When contributing:

1. Keep hardware pinouts and part numbers consistent with the confirmed build.
2. Record setup instructions and dependency changes.
3. Test changes on the relevant subsystem before integration.
4. Document safety implications and failure behavior.
5. Do not describe a feature as complete without test evidence.
6. Avoid committing API keys, Wi-Fi passwords, private credentials, or other secrets.

## License

A license has not yet been selected. Until one is added, reuse and redistribution permissions are not explicitly granted. Add a `LICENSE` file once the team has agreed on the intended license.
