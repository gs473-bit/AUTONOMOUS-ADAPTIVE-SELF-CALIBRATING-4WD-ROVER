
"""Tests for the hardware-independent motor interface."""

import pytest

from rover.control.motor_interface import MotorInterface


class DummyMotor(MotorInterface):
    """In-memory motor implementation for interface testing."""

    def __init__(self):
        self.left_speed = 0.0
        self.right_speed = 0.0
        self.stopped = False
        self.closed = False

    def set_wheel_speeds(
        self,
        left_speed: float,
        right_speed: float,
    ) -> None:
        self.left_speed = left_speed
        self.right_speed = right_speed
        self.stopped = left_speed == 0.0 and right_speed == 0.0

    def stop(self) -> None:
        self.left_speed = 0.0
        self.right_speed = 0.0
        self.stopped = True

    def close(self) -> None:
        self.stop()
        self.closed = True


def test_motor_interface_cannot_be_instantiated_directly():
    with pytest.raises(TypeError):
        MotorInterface()


def test_motor_implementation_accepts_wheel_speed_requests():
    motor = DummyMotor()

    motor.set_wheel_speeds(0.5, 0.3)

    assert motor.left_speed == 0.5
    assert motor.right_speed == 0.3
    assert motor.stopped is False


def test_stop_sets_both_wheel_speeds_to_zero():
    motor = DummyMotor()
    motor.set_wheel_speeds(0.5, -0.5)

    motor.stop()

    assert motor.left_speed == 0.0
    assert motor.right_speed == 0.0
    assert motor.stopped is True


def test_close_stops_motor_and_marks_it_closed():
    motor = DummyMotor()
    motor.set_wheel_speeds(0.5, 0.5)

    motor.close()

    assert motor.stopped is True
    assert motor.closed is True


def test_interface_requires_all_abstract_methods():
    class IncompleteMotor(MotorInterface):
        def set_wheel_speeds(
            self,
            left_speed: float,
            right_speed: float,
        ) -> None:
            pass

    with pytest.raises(TypeError):
        IncompleteMotor()
