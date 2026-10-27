
# Rover System Test Plan

**Project:** ECE357 Autonomous Adaptive Self-Calibrating 4WD Rover  
**Team:** Group 19  
**Status:** Planned — testing has not yet been completed

## 1. Test Objectives

- Verify individual hardware components before integration.
- Validate motor control and wheel feedback.
- Test camera capture and visual marker detection.
- Measure sensor reliability and calibration.
- Validate safe stopping, obstacle avoidance and navigation.
- Record evidence for every test.

## 2. Hardware and Power Tests

| ID | Test | Acceptance Criteria | Status |
|---|---|---|---|
| T01 | Hardware inventory | Exact models and ratings documented | Planned |
| T02 | Power supply | Voltages and polarity verified with a multimeter | Planned |
| T03 | Emergency stop | Propulsion can be stopped independently of software | Planned |
| T04 | Motor driver | Ratings checked against motor requirements | Planned |

## 3. Motor and Encoder Tests

| ID | Test | Acceptance Criteria | Status |
|---|---|---|---|
| T05 | Single motor | Controlled short movement and reliable stop | Planned |
| T06 | Motor direction | Forward and reverse commands verified safely | Planned |
| T07 | Four motor channels | Each channel responds correctly and stops reliably | Planned |
| T08 | Wheel encoders | Repeatable counts during wheel rotation | Planned |
| T09 | Straight-line movement | Distance and heading error measured over repeated runs | Planned |

## 4. Sensors and Perception

| ID | Test | Acceptance Criteria | Status |
|---|---|---|---|
| T10 | Camera capture | Valid image captured and inspected | Planned |
| T11 | ArUco detection | Expected marker ID detected in test images | Planned |
| T12 | Ultrasonic sensor | Readings compared with known distances | Planned |
| T13 | MPU6050 IMU | Stable raw measurements logged | Planned |
| T14 | Sensor integration | Timestamped sensor readings collected consistently | Planned |

## 5. Navigation and System Integration

| ID | Test | Acceptance Criteria | Status |
|---|---|---|---|
| T15 | Mapping | Test environment represented in a documented map | Planned |
| T16 | Path planning | Valid route generated between start and goal | Planned |
| T17 | Obstacle response | Rover stops or safely replans when an obstacle is detected | Planned |
| T18 | Wheel calibration | Before-and-after tracking errors measured | Planned |
| T19 | Communication loss | Rover reaches a safe stopped state when control disconnects | Planned |
| T20 | Fault-mode investigation | Degraded operation evaluated under controlled conditions | Planned |

## 6. Required Evidence

For every test, record:

- Test date and operator.
- Hardware models and wiring revision.
- Software version or Git commit.
- Exact command or procedure.
- Actual terminal output.
- Measurements with units and test conditions.
- Photographs, diagrams or video where relevant.
- Result and corrective actions.

## 7. Result Definitions

- **Planned:** Test has not started.
- **In Progress:** Test is being performed.
- **Passed:** Acceptance criteria met and evidence recorded.
- **Failed:** Acceptance criteria not met.
- **Blocked:** Required hardware, setup or approval is unavailable.

## 8. Safety Requirements

- Review the power system and physical emergency stop before powered tests.
- Raise and secure the rover during initial motor tests.
- Disconnect power before modifying wiring.
- Never connect a potentially 5 V sensor output directly to an unprotected Raspberry Pi GPIO input.
- Do not conduct autonomous navigation tests until motor stopping, sensor behaviour and emergency-stop operation have been verified.

**Important:** Planned acceptance criteria are not experimental results. Update each status only after the corresponding test has actually been performed.
