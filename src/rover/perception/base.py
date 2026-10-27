
"""Hardware-independent interface for rover perception."""

from abc import ABC, abstractmethod
from typing import Any

from rover.models import Detection


class PerceptionBase(ABC):
    """Base interface for processing camera frames into detections."""

    @abstractmethod
    def process_frame(
        self,
        frame: Any,
        timestamp: float,
    ) -> list[Detection]:
        """Process one frame and return detected objects.

        Args:
            frame: A frame supplied by the selected camera implementation.
            timestamp: Capture timestamp in seconds, using a consistent
                clock source throughout the application.

        Returns:
            A list of detected objects. Return an empty list when no
            objects are detected.

        Raises:
            ValueError: If the frame or timestamp is invalid.
        """
        raise NotImplementedError

    def health_check(self) -> bool:
        """Return whether the perception implementation is ready.

        Concrete implementations should override this method when they
        can report their actual readiness.
        """
        return False
