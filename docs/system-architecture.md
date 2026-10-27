
# ECE357 Rover — System Architecture

## 1. Purpose

This document describes the proposed software and hardware architecture for the Autonomous Adaptive Self-Calibrating 4WD Rover.

The Raspberry Pi 4B is the intended compute platform. Actual component availability, interface compatibility, and electrical specifications must be confirmed before integration.

## 2. Architectural Goals

- Acquire data from the camera and available sensors.
- Estimate rover motion using available feedback.
- Detect obstacles and interpret the environment.
- Plan a safe route toward a target.
- Control the four-wheel drive system.
- Support adaptive control and calibration.
- Provide a mechanism for stopping the rover safely.
- Record test results and system status.

These are design goals, not claims that the features have been implemented or tested.

## 3. Proposed Architecture

```text
                   Raspberry Pi 4B
                         |
                Application / Main Loop
                         |
          +--------------+--------------+
          |              |              |
       Sensors        Perception      Interface
          |              |              |
     +----+----+      Camera data    Status / Commands
     |    |    |
   IMU  Range  Wheel
        Sensor Feedback
          |              |
          +------+-------+
                 |
          State Estimation
                 |
          Navigation / Planner
                 |
          Motion Controller
                 |
          Motor Driver Interface
                 |
          Motor Drivers
                 |
           Four DC Motors

    Physical Emergency Stop
       -> Independent safe-stop mechanism
          appropriate to the final design
```

The diagram is conceptual. The final wiring, communication interfaces, execution order, and emergency-stop implementation must be established during engineering review.

## 4. Software Modules

### 4.1 Main Application

Location: `src/rover/main.py`

Responsibilities:
- Serve as the application entry point.
- Coordinate initialization and execution.
- Report initialization errors.
- Avoid enabling hardware until the relevant checks are complete.

The initial scaffold is not a complete rover control loop.

### 4.2 Configuration

Location: `src/rover/config.py`

Responsibilities:
- Load the YAML configuration.
- Validate basic configuration structure.
- Report missing files and malformed configuration.

Future work may add validation for individual settings and hardware limits.

### 4.3 Sensors

Location: `src/rover/sensors/`

Intended responsibilities:
- Acquire readings from confirmed sensors.
- Apply suitable filtering and validation.
- Report unavailable or invalid readings.

Possible devices include an ultrasonic distance sensor, an IMU, and wheel-feedback sensors, subject to confirmation.

### 4.4 Perception

Location: `src/rover/perception/`

Intended responsibilities:
- Process camera frames.
- Support visual detection and localization tasks.
- Provide validated observations to downstream modules.

OpenCV and ArUco processing are proposed capabilities. The camera interface and available computational resources must be confirmed.

### 4.5 Navigation

Location: `src/rover/navigation/`

Intended responsibilities:
- Represent the rover's estimated state.
- Select routes or motion targets.
- Incorporate obstacle information.
- Request safe motion from the control layer.

Mapping and path-planning algorithms remain to be selected and tested.

### 4.6 Control

Location: `src/rover/control/`

Intended responsibilities:
- Translate motion requests into wheel commands.
- Apply limits to requested motion.
- Support calibration and adaptive control as the design matures.
- Stop or inhibit motion when required safety conditions are not met.

No motor-control implementation should be treated as safe until tested with the actual driver, motors, and power system.

### 4.7 Hardware Interface

Location: `src/rover/hardware/`

Intended responsibilities:
- Isolate device-specific communication from higher-level software.
- Centralize GPIO, I2C, and other confirmed hardware interfaces.
- Provide consistent error reporting.
- Prevent unverified hardware operations during initialization.

The actual interface libraries will be selected after hardware identification.

## 5. Data Flow

The intended high-level flow is:

1. Acquire sensor and camera data.
2. Validate readings and identify sensor failures.
3. Estimate the rover's state.
4. Generate navigation targets using environmental information.
5. Calculate motion commands.
6. Apply safety checks and motion limits.
7. Send approved commands through the hardware interface.
8. Record observations, commands, and test outcomes.

The implementation must define timing, stale-data handling, and behavior when a required sensor becomes unavailable.

## 6. Safety Architecture

Safety requirements include:

- Keep the physical emergency stop accessible.
- Use a suitable stop mechanism that does not depend solely on the main application remaining responsive.
- Verify power distribution and grounding before motor tests.
- Protect Raspberry Pi GPIO from overvoltage.
- Reject invalid sensor data where it could cause unsafe motion.
- Define safe behavior for software exceptions and communication loss.
- Test motor stopping behavior before autonomous movement.

The emergency-stop circuit and its effect on motor power must be designed and verified for the final hardware.

## 7. Configuration and Hardware Records

Relevant files:

- `config/rover.yaml`
- `docs/hardware-inventory.md`
- `docs/wiring-record.md`
- `docs/camera-setup.md`
- `docs/hardware-bring-up-checklist.md`
- `docs/test-plan.md`
- `docs/build-log.md`

These records must reflect the actual assembled system, not merely the original design proposal.

## 8. Implementation Milestones

1. Confirm hardware models and availability.
2. Verify the Raspberry Pi software environment.
3. Validate configuration loading and automated tests.
4. Bring up and test individual sensors.
5. Test motor-driver channels using a reviewed procedure.
6. Integrate sensor acquisition and state estimation.
7. Develop perception and navigation.
8. Implement bounded motor control and safety checks.
9. Test obstacle response and emergency stopping.
10. Conduct controlled system-level trials and document results.

## 9. Current Status

This document describes the intended architecture. It does not certify hardware availability, completed implementation, successful tests, or autonomous operation.
