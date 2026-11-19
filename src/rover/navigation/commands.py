
"""Structured navigation commands for the rover."""

from dataclasses import dataclass
from enum import Enum
import math
from rover.control.limits import MAX_DURATION_S


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
        if not isinstance(self.action, NavigationAction):
            raise ValueError("action must be a NavigationAction")
        if isinstance(self.speed, bool) or not isinstance(self.speed, (int, float)) or not math.isfinite(self.speed):
            raise ValueError("speed must be a finite number")
        if not 0.0 <= self.speed <= 1.0:
            raise ValueError("speed must be between 0.0 and 1.0")
        if isinstance(self.duration_s, bool) or not isinstance(self.duration_s, (int, float)) or not math.isfinite(self.duration_s):
            raise ValueError("duration_s must be a finite number")
        if self.duration_s < 0.0:
            raise ValueError("duration_s cannot be negative")
        if self.duration_s > MAX_DURATION_S:
            raise ValueError("duration_s exceeds maximum duration")
        if self.action == NavigationAction.STOP:
            if self.speed != 0.0:
                raise ValueError("STOP commands must have zero speed")
            if self.duration_s != 0.0:
                raise ValueError("STOP commands must have zero duration")
        elif self.duration_s == 0.0:
            raise ValueError("Movement duration must be greater than zero")

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
