
"""Tests for motion speed and duration validation."""

import math

import pytest

from rover.control.limits import (
    MAX_DURATION_S,
    MAX_SPEED,
    MIN_SPEED,
    validate_duration,
    validate_speed,
)


@pytest.mark.parametrize("speed", [0.0, 0.25, 0.5, 1.0])
def test_accepts_valid_speeds(speed):
    assert validate_speed(speed) == speed


@pytest.mark.parametrize("speed", [-0.01, 1.01])
def test_rejects_out_of_range_speeds(speed):
    with pytest.raises(ValueError):
        validate_speed(speed)


@pytest.mark.parametrize("speed", [math.nan, math.inf, -math.inf])
def test_rejects_non_finite_speeds(speed):
    with pytest.raises(ValueError, match="finite"):
        validate_speed(speed)


@pytest.mark.parametrize("speed", [True, False, "0.5", None])
def test_rejects_non_numeric_speeds(speed):
    with pytest.raises(ValueError, match="number"):
        validate_speed(speed)


@pytest.mark.parametrize("duration", [0.01, 0.5, 1.0, MAX_DURATION_S])
def test_accepts_valid_durations(duration):
    assert validate_duration(duration) == duration


@pytest.mark.parametrize("duration", [0.0, -0.1, MAX_DURATION_S + 0.01])
def test_rejects_out_of_range_durations(duration):
    with pytest.raises(ValueError):
        validate_duration(duration)


@pytest.mark.parametrize(
    "duration",
    [math.nan, math.inf, -math.inf],
)
def test_rejects_non_finite_durations(duration):
    with pytest.raises(ValueError, match="finite"):
        validate_duration(duration)


@pytest.mark.parametrize("duration", [True, False, "1.0", None])
def test_rejects_non_numeric_durations(duration):
    with pytest.raises(ValueError, match="number"):
        validate_duration(duration)


def test_speed_lower_and_upper_bounds():
    assert validate_speed(MIN_SPEED) == MIN_SPEED
    assert validate_speed(MAX_SPEED) == MAX_SPEED


def test_duration_upper_bound():
    assert validate_duration(MAX_DURATION_S) == MAX_DURATION_S
