
"""Safety-gated dispatcher for navigation commands."""

from dataclasses import dataclass
import math

from rover.control.motor_interface import MotorInterface
from rover.navigation.commands import NavigationAction, NavigationCommand


@dataclass(frozen=True)
class DispatchResult:
    """Outcome of a navigation command."""

    accepted: bool
    reason: str


class CommandDispatcher:
    """Validate commands before forwarding them to a motor interface.

    This class does not implement a physical emergency-stop circuit.
    Hardware-level protections must be implemented independently.
    """

    def __init__(self, motor: MotorInterface) -> None:
        self._motor = motor
        self._emergency_stop_active = True

    @property
    def emergency_stop_active(self) -> bool:
        return self._emergency_stop_active

    def set_emergency_stop(self, active: bool) -> None:
        """Update the software emergency-stop state."""
        self._emergency_stop_active = bool(active)

        if self._emergency_stop_active:
            self._motor.stop()

    def dispatch(self, command: NavigationCommand) -> DispatchResult:
        """Validate a command and forward permitted requests."""
        if not isinstance(command, NavigationCommand):
            return DispatchResult(False, "Invalid command type")

        if command.action == NavigationAction.STOP:
            self._motor.stop()
            return DispatchResult(True, "Stop command accepted")

        if self._emergency_stop_active:
            self._motor.stop()
            return DispatchResult(False, "Emergency stop is active")

        if (
            not math.isfinite(command.speed)
            or not 0.0 < command.speed <= 1.0
        ):
            self._motor.stop()
            return DispatchResult(False, "Invalid movement speed")

        if (
            not math.isfinite(command.duration_s)
            or command.duration_s <= 0.0
        ):
            self._motor.stop()
            return DispatchResult(False, "Invalid movement duration")

        speed = command.speed

        if command.action == NavigationAction.MOVE_FORWARD:
            left, right = speed, speed
        elif command.action == NavigationAction.MOVE_BACKWARD:
            left, right = -speed, -speed
        elif command.action == NavigationAction.TURN_LEFT:
            left, right = -speed, speed
        elif command.action == NavigationAction.TURN_RIGHT:
            left, right = speed, -speed
        else:
            self._motor.stop()
            return DispatchResult(False, "Unsupported navigation action")

        try:
            self._motor.set_wheel_speeds(left, right)
        except Exception:
            try:
                self._motor.stop()
            finally:
                return DispatchResult(False, "Motor command failed")

        return DispatchResult(True, "Command passed software checks")
