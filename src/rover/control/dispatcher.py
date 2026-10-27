
"""Safety-gated dispatcher for high-level navigation commands."""

from dataclasses import dataclass

from rover.navigation.commands import NavigationAction, NavigationCommand


@dataclass(frozen=True)
class DispatchResult:
    """Outcome of validating a navigation command."""

    accepted: bool
    reason: str


class CommandDispatcher:
    """Validate commands before a future motor-control layer receives them.

    This class does not operate GPIO pins or energize motors.
    """

    def __init__(self) -> None:
        self._emergency_stop_active = True

    @property
    def emergency_stop_active(self) -> bool:
        return self._emergency_stop_active

    def set_emergency_stop(self, active: bool) -> None:
        """Update the software emergency-stop state."""
        self._emergency_stop_active = active

    def dispatch(self, command: NavigationCommand) -> DispatchResult:
        """Validate a command without executing physical movement."""
        if not isinstance(command, NavigationCommand):
            return DispatchResult(False, "Invalid command type")

        if command.action == NavigationAction.STOP:
            return DispatchResult(True, "Stop command accepted")

        if self._emergency_stop_active:
            return DispatchResult(False, "Emergency stop is active")

        if command.speed <= 0.0:
            return DispatchResult(False, "Movement speed must be positive")

        if command.duration_s <= 0.0:
            return DispatchResult(False, "Movement duration must be positive")

        return DispatchResult(True, "Command passed software checks")
