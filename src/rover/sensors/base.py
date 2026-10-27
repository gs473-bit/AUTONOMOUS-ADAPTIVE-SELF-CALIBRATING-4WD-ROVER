
"""Base interface for rover sensor implementations."""

from abc import ABC, abstractmethod

from rover.models import SensorReading


class SensorBase(ABC):
    """Common interface for sensors with a single reading output."""

    def __init__(self, sensor_id: str) -> None:
        if not sensor_id.strip():
            raise ValueError("sensor_id must not be empty.")

        self.sensor_id = sensor_id

    @abstractmethod
    def read(self) -> SensorReading:
        """Acquire one reading or return an invalid reading."""
        raise NotImplementedError

    def health_check(self) -> bool:
        """Return whether the sensor is currently available.

        Concrete drivers should override this when they can check
        device availability. The default makes no availability claim.
        """
        return False
