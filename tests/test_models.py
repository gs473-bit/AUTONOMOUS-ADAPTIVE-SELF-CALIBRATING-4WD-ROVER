
import pytest

from rover.models import Detection, RoverState, SensorReading


def test_valid_sensor_reading():
    reading = SensorReading(
        sensor_id="test_sensor",
        timestamp=1.0,
        value=2.5,
        unit="m",
    )

    assert reading.valid is True
    assert reading.value == 2.5
    assert reading.error is None


def test_sensor_reading_rejects_empty_id():
    with pytest.raises(ValueError, match="sensor_id"):
        SensorReading(
            sensor_id="",
            timestamp=1.0,
            value=2.5,
            unit="m",
        )


def test_invalid_sensor_reading_can_report_error():
    reading = SensorReading(
        sensor_id="range_sensor",
        timestamp=2.0,
        value=None,
        unit="m",
        valid=False,
        error="No reading received",
    )

    assert reading.valid is False
    assert reading.error == "No reading received"


def test_valid_detection():
    detection = Detection(
        label="aruco_marker",
        confidence=0.95,
        position=(1.0, 2.0),
        coordinate_frame="camera",
    )

    assert detection.label == "aruco_marker"
    assert detection.confidence == 0.95
    assert detection.position == (1.0, 2.0)


def test_detection_rejects_invalid_confidence():
    with pytest.raises(ValueError, match="Confidence"):
        Detection(label="obstacle", confidence=1.5)


def test_detection_position_requires_coordinate_frame():
    with pytest.raises(ValueError, match="coordinate frame"):
        Detection(
            label="obstacle",
            position=(1.0, 2.0),
        )


def test_unknown_rover_state_is_allowed():
    state = RoverState(timestamp=1.0)

    assert state.valid is False
    assert state.position is None
    assert state.linear_velocity is None


def test_rover_state_position_requires_coordinate_frame():
    with pytest.raises(ValueError, match="coordinate frame"):
        RoverState(
            timestamp=1.0,
            position=(1.0, 2.0),
        )


def test_negative_timestamp_is_rejected():
    with pytest.raises(ValueError, match="timestamp"):
        SensorReading(
            sensor_id="test_sensor",
            timestamp=-1.0,
            value=1.0,
            unit="m",
        )
