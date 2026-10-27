
"""Tests for structured navigation commands."""

import pytest

from rover.navigation.commands import (
    NavigationAction,
    NavigationCommand,
)


def test_stop_command_has_zero_speed_and_duration():
    command = NavigationCommand.stop()

    assert command.action == NavigationAction.STOP
    assert command.speed == 0.0
    assert command.duration_s == 0.0


def test_forward_command_stores_requested_values():
    command = NavigationCommand.forward(
        speed=0.5,
        duration_s=2.0,
    )

    assert command.action == NavigationAction.MOVE_FORWARD
    assert command.speed == 0.5
    assert command.duration_s == 2.0


@pytest.mark.parametrize("speed", [-0.1, 1.1])
def test_rejects_speed_outside_normalized_range(speed):
    with pytest.raises(ValueError, match="speed must be between"):
        NavigationCommand(
            action=NavigationAction.MOVE_FORWARD,
            speed=speed,
            duration_s=1.0,
        )


def test_rejects_negative_duration():
    with pytest.raises(ValueError, match="duration_s cannot be negative"):
        NavigationCommand(
            action=NavigationAction.MOVE_FORWARD,
            speed=0.5,
            duration_s=-1.0,
        )


def test_forward_rejects_zero_duration():
    with pytest.raises(ValueError, match="Forward duration"):
        NavigationCommand.forward(
            speed=0.5,
            duration_s=0.0,
        )


def test_stop_rejects_nonzero_speed():
    with pytest.raises(ValueError, match="STOP commands"):
        NavigationCommand(
            action=NavigationAction.STOP,
            speed=0.5,
        )


def test_rejects_action_that_is_not_an_enum_member():
    with pytest.raises(ValueError, match="action must be"):
        NavigationCommand(action="forward")


def test_command_is_immutable():
    command = NavigationCommand.stop()

    with pytest.raises(AttributeError):
        command.speed = 0.5
