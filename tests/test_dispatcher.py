
"""Tests for the motor-integrated command dispatcher."""

from rover.control.dispatcher import CommandDispatcher
from rover.control.motor_interface import MotorInterface
from rover.navigation.commands import (
    NavigationAction,
    NavigationCommand,
)


class DummyMotor(MotorInterface):
    """Record motor requests without energizing physical motors."""

    def __init__(self):
        self.left_speed = 0.0
        self.right_speed = 0.0
        self.stop_calls = 0

    def set_wheel_speeds(self, left_speed, right_speed):
        self.left_speed = left_speed
        self.right_speed = right_speed

    def stop(self):
        self.left_speed = 0.0
        self.right_speed = 0.0
        self.stop_calls += 1

    def close(self):
        self.stop()


def make_dispatcher():
    motor = DummyMotor()
    dispatcher = CommandDispatcher(motor)
    return dispatcher, motor


def test_dispatcher_starts_with_emergency_stop_active():
    dispatcher, _ = make_dispatcher()

    assert dispatcher.emergency_stop_active is True


def test_movement_is_rejected_by_default():
    dispatcher, motor = make_dispatcher()

    result = dispatcher.dispatch(
        NavigationCommand.forward(speed=0.5, duration_s=1.0)
    )

    assert result.accepted is False
    assert result.reason == "Emergency stop is active"
    assert motor.left_speed == 0.0
    assert motor.right_speed == 0.0


def test_stop_command_is_accepted_during_emergency_stop():
    dispatcher, motor = make_dispatcher()

    result = dispatcher.dispatch(NavigationCommand.stop())

    assert result.accepted is True
    assert result.reason == "Stop command accepted"
    assert motor.stop_calls == 1


def test_forward_command_sets_equal_positive_wheel_speeds():
    dispatcher, motor = make_dispatcher()
    dispatcher.set_emergency_stop(False)

    result = dispatcher.dispatch(
        NavigationCommand.forward(speed=0.4, duration_s=1.0)
    )

    assert result.accepted is True
    assert motor.left_speed == 0.4
    assert motor.right_speed == 0.4


def test_backward_command_sets_equal_negative_wheel_speeds():
    dispatcher, motor = make_dispatcher()
    dispatcher.set_emergency_stop(False)

    result = dispatcher.dispatch(
        NavigationCommand(
            action=NavigationAction.MOVE_BACKWARD,
            speed=0.3,
            duration_s=1.0,
        )
    )

    assert result.accepted is True
    assert motor.left_speed == -0.3
    assert motor.right_speed == -0.3


def test_left_turn_uses_opposite_wheel_directions():
    dispatcher, motor = make_dispatcher()
    dispatcher.set_emergency_stop(False)

    result = dispatcher.dispatch(
        NavigationCommand(
            action=NavigationAction.TURN_LEFT,
            speed=0.25,
            duration_s=1.0,
        )
    )

    assert result.accepted is True
    assert motor.left_speed == -0.25
    assert motor.right_speed == 0.25


def test_right_turn_uses_opposite_wheel_directions():
    dispatcher, motor = make_dispatcher()
    dispatcher.set_emergency_stop(False)

    result = dispatcher.dispatch(
        NavigationCommand(
            action=NavigationAction.TURN_RIGHT,
            speed=0.25,
            duration_s=1.0,
        )
    )

    assert result.accepted is True
    assert motor.left_speed == 0.25
    assert motor.right_speed == -0.25


def test_reactivating_emergency_stop_requests_motor_stop():
    dispatcher, motor = make_dispatcher()
    dispatcher.set_emergency_stop(False)
    dispatcher.dispatch(
        NavigationCommand.forward(speed=0.5, duration_s=1.0)
    )

    dispatcher.set_emergency_stop(True)

    assert dispatcher.emergency_stop_active is True
    assert motor.left_speed == 0.0
    assert motor.right_speed == 0.0


def test_invalid_command_type_is_rejected():
    dispatcher, _ = make_dispatcher()

    result = dispatcher.dispatch("move forward")

    assert result.accepted is False
    assert result.reason == "Invalid command type"


def test_motor_exception_is_reported_as_failure():
    class FailingMotor(DummyMotor):
        def set_wheel_speeds(self, left_speed, right_speed):
            raise RuntimeError("Simulated motor failure")

    motor = FailingMotor()
    dispatcher = CommandDispatcher(motor)
    dispatcher.set_emergency_stop(False)

    result = dispatcher.dispatch(
        NavigationCommand.forward(speed=0.5, duration_s=1.0)
    )

    assert result.accepted is False
    assert result.reason == "Motor command failed"
    assert motor.left_speed == 0.0
    assert motor.right_speed == 0.0
