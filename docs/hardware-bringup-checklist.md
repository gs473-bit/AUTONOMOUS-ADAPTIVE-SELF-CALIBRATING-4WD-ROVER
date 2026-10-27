
# ECE357 Rover — Hardware Bring-Up Checklist

## Purpose

Track each hardware component from identification to verified integration.

**Rule:** Do not mark a component as tested or integrated until the corresponding physical test has been performed and its result recorded.

## Status Definitions

- [ ] Not started
- [ ] Identified: exact model and specifications recorded
- [ ] Inspected: physical condition and connectors checked
- [ ] Wired: connections checked against the approved wiring record
- [ ] Tested: a documented test was performed and its result recorded
- [ ] Integrated: verified with the relevant rover software module

Use the build log to record evidence and the date of each completed stage.

## 1. Compute Platform — Raspberry Pi

- [ ] Confirm exact Raspberry Pi model and board revision
- [ ] Confirm power supply rating and operating-system version
- [ ] Confirm safe boot and stable operation
- [ ] Confirm Python environment and dependencies
- [ ] Run automated software tests
- [ ] Record results in `docs/build-log.md`

## 2. Camera

- [ ] Confirm actual camera model and interface
- [ ] Confirm connector and electrical compatibility
- [ ] Identify the correct software interface
- [ ] Inspect the connection before powering on
- [ ] Run a camera-specific frame acquisition test
- [ ] Record image resolution and test outcome
- [ ] Verify camera output in the perception module

**Important:** The USB diagnostic script is only for compatible USB/UVC cameras. Do not assume it supports CSI or parallel-interface cameras such as many OV7670 modules.

## 3. Motor Drivers

- [ ] Confirm exact driver-board model and specifications
- [ ] Verify motor voltage and current requirements
- [ ] Verify driver voltage/current ratings and cooling requirements
- [ ] Confirm power distribution and grounding
- [ ] Verify logic-level compatibility with Raspberry Pi GPIO
- [ ] Inspect wiring with power disconnected
- [ ] Test one motor channel using an approved safe procedure
- [ ] Verify stop behavior before testing additional channels
- [ ] Record results before integrating motor-control software

**Safety:** Do not assume the two planned L298N boards are available or suitable until their specifications and the motors are confirmed.

## 4. DC Gear Motors and Wheel Feedback

- [ ] Record the exact motor model and ratings
- [ ] Identify the encoder model, if fitted
- [ ] Confirm encoder output voltage and signal type
- [ ] Verify encoder interface compatibility
- [ ] Test each motor independently with the wheels safely supported
- [ ] Verify direction and stopping behavior
- [ ] Record encoder counts and test conditions, if applicable
- [ ] Confirm wheel feedback is read correctly by software

Do not assume encoders are present merely because wheel feedback is part of the design.

## 5. Ultrasonic Sensor

- [ ] Confirm exact sensor model and supply requirements
- [ ] Verify trigger and echo pin assignments
- [ ] Check signal voltage levels
- [ ] Add suitable level protection if the echo output exceeds permitted GPIO voltage
- [ ] Test readings against known distances
- [ ] Record repeatability and invalid-reading behavior
- [ ] Integrate only after the sensor test passes

## 6. IMU — MPU6050, If Confirmed

- [ ] Confirm exact board and interface
- [ ] Verify supply voltage and I2C logic-level compatibility
- [ ] Confirm SDA/SCL wiring and I2C address
- [ ] Detect the device on the bus
- [ ] Read sensor values while stationary
- [ ] Check for plausible responses during controlled movement
- [ ] Record calibration procedure and results

## 7. Power System and Emergency Stop

- [ ] Identify battery or power-source model
- [ ] Confirm voltage, current capability, and protection
- [ ] Verify separate logic and motor supply requirements
- [ ] Inspect wiring, connectors, insulation, and polarity
- [ ] Confirm the emergency-stop design and its actual effect
- [ ] Test the stop mechanism under a reviewed, controlled procedure
- [ ] Verify that stopping does not depend solely on software

Do not power motors until the power system and wiring have been reviewed.

## 8. System Integration

- [ ] Confirm all required components are identified
- [ ] Confirm wiring record reflects the actual assembly
- [ ] Complete individual component tests
- [ ] Verify sensor data reaches the relevant software modules
- [ ] Verify motor commands and stopping behavior safely
- [ ] Test camera processing with recorded or live frames
- [ ] Test obstacle-detection behavior in a controlled setup
- [ ] Test the emergency stop
- [ ] Record integration results and outstanding issues

## 9. Final Record

For each component, record:

- Component and exact model
- Test date and person performing the test
- Wiring or configuration used
- Test procedure
- Expected result
- Actual result
- Evidence, if available
- Pass/fail status and unresolved issues

**Current project status:** Hardware availability, exact models, wiring, and physical test results must be confirmed by the team. This checklist does not certify that any component has been tested.
