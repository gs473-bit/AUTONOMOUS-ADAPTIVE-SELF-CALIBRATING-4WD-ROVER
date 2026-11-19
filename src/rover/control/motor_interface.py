"""Four independently controlled wheel channels (FL, FR, RL, RR)."""
from abc import ABC, abstractmethod
import math


def _wheel_value(value: float) -> float:
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError("Wheel command must be numeric")
    if not math.isfinite(value) or abs(value) > 1.0:
        raise ValueError("Wheel command must be finite and in [-1, 1]")
    return float(value)


class MotorInterface(ABC):
    """Four-channel normalized Mecanum motor contract; no hardware side effects here."""

    @abstractmethod
    def set_wheel_speeds(self, fl: float, fr: float, rl: float, rr: float) -> None:
        """Request normalized per-wheel speeds. Implementations MUST apply failsafes."""
        raise NotImplementedError

    @abstractmethod
    def stop(self) -> None:
        """Request all four wheel commands become zero."""
        raise NotImplementedError

    @abstractmethod
    def close(self) -> None:
        """Release hardware resources with motors disabled."""
        raise NotImplementedError


class LegacyDifferentialMotorInterface(ABC):
    """DEPRECATED two-group API. Use MotorInterface for the physical rover."""

    @abstractmethod
    def set_wheel_speeds(self, left_speed: float, right_speed: float) -> None:
        raise NotImplementedError
