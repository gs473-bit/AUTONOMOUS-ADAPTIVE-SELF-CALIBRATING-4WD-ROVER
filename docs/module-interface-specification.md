
# ECE357 Rover — Module Interface Specification

## 1. Purpose

Define the proposed interfaces between software modules for the Autonomous Adaptive Self-Calibrating 4WD Rover.

This specification is an initial software contract. Interfaces may evolve as the hardware and algorithms are confirmed.

## 2. Interface Principles

- Use consistent units and clearly named fields.
- Include timestamps for sensor measurements.
- Represent unavailable or invalid measurements explicitly.
- Separate sensor acquisition from navigation and control.
- Do not let a perception or navigation module directly manipulate GPIO.
- Apply safety checks before any motor command reaches the hardware.
- Never interpret missing data as a valid zero measurement.

## 3. Common Conventions

| Item | Convention |
|---|---|
| Distance | Metres internally |
| Time | Seconds for durations; monotonic timestamps for measurement timing |
| Angles | Radians internally |
| Wheel speed | Radians per second internally |
| Status | Explicit validity and error information |
| Coordinates | Named reference frame required |
| Missing values | Explicitly unavailable, not silently replaced |

Conversions at device boundaries must be documented and tested.

## 4. Sensor Interface

Location: `src/rover/sensors/`

### Proposed input

Device-specific communication from confirmed hardware.

### Proposed output

A normalized measurement record containing:

- `sensor_id`: identifier of the source
- `timestamp`: measurement time
- `value`: measurement or structured reading
- `unit`: measurement unit
- `valid`: whether the reading passed basic checks
- `error`: optional diagnostic information

Sensor-specific fields may be added when required.

### Requirements

- Reject malformed readings.
- Report communication failures.
- Identify stale data.
- Keep device-specific code out of navigation logic.

## 5. Camera and Perception Interface

Location: `src/rover/perception/`

### Input

A frame from a confirmed camera interface, together with its capture timestamp.

### Output

A perception result containing:

- `timestamp`: frame capture time
- `frame_valid`: whether the frame was acquired successfully
- `detections`: zero or more detected objects or markers
- `processing_status`: success or failure information

Each detection should specify its class or marker identifier and the coordinate frame used for its position, when position is available.

### Requirements

- Distinguish a valid frame with no detections from a failed frame.
- Document image resolution and processing assumptions.
- Do not claim metric distance from an image unless the required calibration and method are available.

## 6. State Estimation Interface

Location: `src/rover/navigation/`

### Input

- Valid sensor measurements
- Available wheel-feedback readings
- Camera-derived observations, when supported

### Output

An estimated rover state containing:

- `timestamp`
- `position`, if estimable
- `orientation`, if estimable
- `linear_velocity`, if estimable
- `angular_velocity`, if estimable
- `validity` or uncertainty information

The coordinate frame and units must be explicit.

Do not invent position or velocity values when the available sensors cannot support the estimate.

## 7. Navigation Interface

Location: `src/rover/navigation/`

### Input

- Estimated rover state
- Target or waypoint
- Valid obstacle information

### Output

A motion request containing:

- `timestamp`
- `linear_velocity`
- `angular_velocity`
- `valid_until`
- `reason`, if the request is a stop or degraded-motion request

The motion request is a desired command, not permission to bypass safety checks.

## 8. Control Interface

Location: `src/rover/control/`

### Input

- Motion request
- Available wheel feedback
- Safety state
- Relevant configuration and limits

### Output

A bounded wheel-command request containing:

- `timestamp`
- Per-wheel target or output commands
- `valid_until`
- `enabled` or stop state
- Diagnostic information

The final wheel-command representation must match the confirmed motor-driver interface.

### Requirements

- Enforce configured limits.
- Reject stale motion requests.
- Handle invalid feedback explicitly.
- Provide a defined stop behavior.
- Keep calibration parameters and their units documented.

## 9. Hardware Interface

Location: `src/rover/hardware/`

### Input

A validated hardware command from the control layer.

### Output

- Command acceptance or rejection
- Device status, when available
- Hardware errors
- Actual feedback, when supported

The hardware interface must not silently convert a failed command into a successful result.

No hardware implementation should be enabled until pin mappings, voltage levels, driver compatibility, and power arrangements have been verified.

## 10. Safety Interface

Safety checks must be able to inhibit motion independently of normal navigation decisions.

The exact emergency-stop circuit and software signal are hardware-design decisions that remain to be verified.

The system must define behavior for:
- Emergency-stop activation
- Stale sensor data
- Lost communication
- Controller exceptions
- Invalid motor commands
- Startup and shutdown

A software stop command alone is not a substitute for a suitable physical emergency-stop mechanism.

## 11. Configuration Interface

Location: `src/rover/config.py`

Current responsibilities:
- Load YAML configuration.
- Reject missing configuration files.
- Reject malformed YAML.
- Require a mapping at the YAML root.

Future responsibilities:
- Validate required settings and types.
- Validate safe operating limits.
- Check configuration compatibility with confirmed hardware.

## 12. Testing Contract

Each module should have tests for:

1. Valid input.
2. Missing or malformed input.
3. Boundary values.
4. Stale or invalid data, where applicable.
5. Error propagation.
6. Safety-related rejection behavior.

Use mocked hardware for software tests. Physical hardware tests must be recorded separately in `docs/build-log.md`.

## 13. Implementation Status

This specification describes proposed interfaces. It does not mean the interfaces are implemented or that the rover has passed integration testing.
