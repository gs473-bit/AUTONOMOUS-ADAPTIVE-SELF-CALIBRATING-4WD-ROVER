
import pytest

from rover.safety import evaluate_safety


def evaluate(**overrides):
    args = {
        "emergency_stop_active": False,
        "command_timestamp": 9.8,
        "now": 10.0,
        "max_command_age": 0.5,
        "required_sensors_valid": True,
    }
    args.update(overrides)
    return evaluate_safety(**args)


def test_fresh_valid_command_is_permitted():
    assert evaluate().permitted


def test_emergency_stop_blocks_command():
    result = evaluate(emergency_stop_active=True)
    assert not result.permitted
    assert "Emergency stop" in result.reason


def test_stale_command_is_blocked():
    result = evaluate(command_timestamp=8.0)
    assert not result.permitted
    assert "stale" in result.reason


def test_invalid_sensor_data_blocks_command():
    result = evaluate(required_sensors_valid=False)
    assert not result.permitted


def test_future_timestamp_is_blocked():
    result = evaluate(command_timestamp=11.0)
    assert not result.permitted


def test_command_age_must_be_positive():
    with pytest.raises(ValueError, match="positive"):
        evaluate(max_command_age=0)
