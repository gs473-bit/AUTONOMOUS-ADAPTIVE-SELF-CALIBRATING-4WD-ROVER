
import pytest

from rover.control.limits import MotionLimits, limit_motion


def test_motion_within_limits_is_unchanged():
    limits = MotionLimits(
        max_linear_velocity=1.0,
        max_angular_velocity=2.0,
    )

    request = limit_motion(0.5, 1.0, limits)

    assert request.linear_velocity == 0.5
    assert request.angular_velocity == 1.0


def test_positive_velocities_are_clamped():
    limits = MotionLimits(1.0, 2.0)

    request = limit_motion(3.0, 5.0, limits)

    assert request.linear_velocity == 1.0
    assert request.angular_velocity == 2.0


def test_negative_velocities_are_clamped():
    limits = MotionLimits(1.0, 2.0)

    request = limit_motion(-3.0, -5.0, limits)

    assert request.linear_velocity == -1.0
    assert request.angular_velocity == -2.0


def test_zero_velocity_is_preserved():
    limits = MotionLimits(1.0, 2.0)

    request = limit_motion(0.0, 0.0, limits)

    assert request.linear_velocity == 0.0
    assert request.angular_velocity == 0.0


@pytest.mark.parametrize(
    ("linear", "angular"),
    [(0.5, -1.0), (-0.5, 1.0), (1.0, 2.0)],
)
def test_boundary_and_mixed_requests(linear, angular):
    limits = MotionLimits(1.0, 2.0)

    request = limit_motion(linear, angular, limits)

    assert -1.0 <= request.linear_velocity <= 1.0
    assert -2.0 <= request.angular_velocity <= 2.0


def test_zero_linear_limit_is_rejected():
    with pytest.raises(ValueError, match="linear velocity"):
        MotionLimits(0.0, 2.0)


def test_negative_angular_limit_is_rejected():
    with pytest.raises(ValueError, match="angular velocity"):
        MotionLimits(1.0, -2.0)
