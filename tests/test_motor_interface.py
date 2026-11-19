
"""Tests for the hardware-independent motor interface."""

import pytest

from rover.control.motor_interface import MotorInterface


class DummyMotor(MotorInterface):
    """In-memory motor implementation for interface testing."""

    def __init__(self):
        self.fl = self.fr = self.rl = self.rr = 0.0
        self.stopped = False
        self.closed = False

    def set_wheel_speeds(
        self,
        fl: float,
        fr: float,
        rl: float,
        rr: float,
    ) -> None:
        self.fl, self.fr, self.rl, self.rr = fl, fr, rl, rr
        self.stopped = fl == fr == rl == rr == 0.0

    def stop(self) -> None:
        self.fl = self.fr = self.rl = self.rr = 0.0
        self.stopped = True

    def close(self) -> None:
        self.stop()
        self.closed = True


def test_motor_interface_cannot_be_instantiated_directly():
    with pytest.raises(TypeError):
        MotorInterface()


def test_motor_implementation_accepts_wheel_speed_requests():
    motor = DummyMotor()

    motor.set_wheel_speeds(0.5, 0.3, 0.4, 0.2)

    assert motor.fl == 0.5
    assert motor.fr == 0.3
    assert motor.stopped is False


def test_stop_sets_both_wheel_speeds_to_zero():
    motor = DummyMotor()
    motor.set_wheel_speeds(0.5, -0.5, 0.5, -0.5)

    motor.stop()

    assert motor.fl == 0.0
    assert motor.fr == 0.0
    assert motor.stopped is True


def test_close_stops_motor_and_marks_it_closed():
    motor = DummyMotor()
    motor.set_wheel_speeds(0.5, 0.5, 0.5, 0.5)

    motor.close()

    assert motor.stopped is True
    assert motor.closed is True


def test_interface_requires_all_abstract_methods():
    class IncompleteMotor(MotorInterface):
        def set_wheel_speeds(
            self,
            fl: float,
            fr: float,
            rl: float,
            rr: float,
        ) -> None:
            pass

    with pytest.raises(TypeError):
        IncompleteMotor()
