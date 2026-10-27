
"""Structured navigation commands for the rover."""

from dataclasses import dataclass
from enum import Enum


class NavigationAction(str, Enum):
    """High-level actions requested by navigation."""

    STOP = "stop"
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"


@dataclass(frozen=True)
class NavigationCommand:
    """A validated request for a high-level rover movement."""

    action: NavigationAction
    speed: float = 0.0
    duration_s: float = 0.0

    def __post_init__(self) -> None:
        """Reject invalid commands before they reach control code."""
        if not isinstance(self.action, NavigationAction):
            raise ValueError("action must be a NavigationAction")

        if not 0.0 <= self.speed <= 1.0:
            raise ValueError("speed must be between 0.0 and 1.0")

        if self.duration_s < 0.0:
            raise ValueError("duration_s cannot be negative")

        if self.action == NavigationAction.STOP:
            if self.speed != 0.0:
                raise ValueError("STOP commands must have zero speed")

    @classmethod
    def stop(cls) -> "NavigationCommand":
        """Create a stop command."""
        return cls(action=NavigationAction.STOP)

    @classmethod
    def forward(
        cls,
        speed: float,
        duration_s: float,
    ) -> "NavigationCommand":
        """Request forward motion for a bounded duration."""
        if duration_s <= 0.0:
            raise ValueError("Forward duration must be greater than zero")

        return cls(
            action=NavigationAction.MOVE_FORWARD,
            speed=speed,
            duration_s=duration_s,
        )
