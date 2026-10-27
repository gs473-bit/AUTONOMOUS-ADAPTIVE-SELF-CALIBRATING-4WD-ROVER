
"""Tests for the safety-gated command dispatcher."""

from rover.control.dispatcher import CommandDispatcher
from rover.navigation.commands import (
    NavigationAction,
    NavigationCommand,
)


def test_dispatcher_starts_with_emergency_stop_active():
    dispatcher = CommandDispatcher()

    assert dispatcher.emergency_stop_active is True


def test_movement_is_rejected_by_default():
    dispatcher = CommandDispatcher()
    command = NavigationCommand.forward(
        speed=0.5,
        duration_s=1.0,
    )

    result = dispatcher.dispatch(command)

    assert result.accepted is False
    assert result.reason == "Emergency stop is active"


def test_stop_command_is_accepted_during_emergency_stop():
    dispatcher = CommandDispatcher()

    result = dispatcher.dispatch(NavigationCommand.stop())

    assert result.accepted is True
    assert result.reason == "Stop command accepted"


def test_movement_passes_software_checks_after_gate_is_cleared():
    dispatcher = CommandDispatcher()
    dispatcher.set_emergency_stop(False)

    command = NavigationCommand.forward(
        speed=0.5,
        duration_s=1.0,
    )
    result = dispatcher.dispatch(command)

    assert result.accepted is True
    assert result.reason == "Command passed software checks"


def test_dispatcher_rejects_invalid_command_type():
    dispatcher = CommandDispatcher()

    result = dispatcher.dispatch("move forward")

    assert result.accepted is False
    assert result.reason == "Invalid command type"


def test_emergency_stop_can_be_reactivated():
    dispatcher = CommandDispatcher()
    dispatcher.set_emergency_stop(False)
    dispatcher.set_emergency_stop(True)

    result = dispatcher.dispatch(
        NavigationCommand.forward(speed=0.5, duration_s=1.0)
    )

    assert dispatcher.emergency_stop_active is True
    assert result.accepted is False
    assert result.reason == "Emergency stop is active"


def test_stop_command_action_is_stop():
    command = NavigationCommand.stop()

    assert command.action == NavigationAction.STOP
