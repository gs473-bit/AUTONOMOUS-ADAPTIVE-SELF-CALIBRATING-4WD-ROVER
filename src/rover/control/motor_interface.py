
"""Hardware-independent motor-control interface for the rover."""

from abc import ABC, abstractmethod


class MotorInterface(ABC):
    """Contract for a differential-drive motor controller.

    Wheel commands are normalized to the range -1.0 to 1.0:
    negative means reverse, zero means stop, positive means forward.

    Concrete implementations must enforce their own hardware safety
    limits and must not assume that a software stop guarantees
    physical motor shutdown.
    """

    @abstractmethod
    def set_wheel_speeds(
        self,
        left_speed: float,
        right_speed: float,
    ) -> None:
        """Request normalized speeds for the left and right wheel groups."""
        raise NotImplementedError

    @abstractmethod
    def stop(self) -> None:
        """Request that both wheel groups stop."""
        raise NotImplementedError

    @abstractmethod
    def close(self) -> None:
        """Release hardware resources and leave outputs in a safe state."""
        raise NotImplementedError
