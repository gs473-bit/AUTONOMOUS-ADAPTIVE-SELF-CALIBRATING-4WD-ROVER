
"""Shared data models for the ECE357 rover.

These models contain data only. They do not access GPIO or control hardware.
"""

from dataclasses import dataclass, field
from typing import Any


@dataclass
class SensorReading:
    """Represent one sensor measurement."""

    sensor_id: str
    timestamp: float
    value: Any
    unit: str
    valid: bool = True
    error: str | None = None

    def __post_init__(self) -> None:
        if not self.sensor_id.strip():
            raise ValueError("sensor_id must not be empty.")

        if not self.unit.strip():
            raise ValueError("unit must not be empty.")

        if self.timestamp < 0:
            raise ValueError("timestamp must not be negative.")

        if self.valid and self.error is not None:
            raise ValueError(
                "A valid reading must not contain an error."
            )


@dataclass
class Detection:
    """Represent a detected object or visual marker."""

    label: str
    confidence: float | None = None
    position: tuple[float, ...] | None = None
    coordinate_frame: str | None = None
    metadata: dict[str, Any] = field(default_factory=dict)

    def __post_init__(self) -> None:
        if not self.label.strip():
            raise ValueError("Detection label must not be empty.")

        if self.confidence is not None:
            if not 0.0 <= self.confidence <= 1.0:
                raise ValueError(
                    "Confidence must be between 0 and 1."
                )

        if self.position is not None and not self.coordinate_frame:
            raise ValueError(
                "A position requires a coordinate frame."
            )


@dataclass
class RoverState:
    """Represent the estimated rover state.

    Unknown values remain None until supported by sensor data.
    """

    timestamp: float
    position: tuple[float, ...] | None = None
    orientation: tuple[float, ...] | None = None
    linear_velocity: float | None = None
    angular_velocity: float | None = None
    coordinate_frame: str | None = None
    valid: bool = False
    uncertainty: dict[str, float] = field(default_factory=dict)

    def __post_init__(self) -> None:
        if self.timestamp < 0:
            raise ValueError("timestamp must not be negative.")

        if self.position is not None and not self.coordinate_frame:
            raise ValueError(
                "Position requires a coordinate frame."
            )

        if self.orientation is not None and not self.coordinate_frame:
            raise ValueError(
                "Orientation requires a coordinate frame."
            )

        for name, value in self.uncertainty.items():
            if value < 0:
                raise ValueError(
                    f"Uncertainty for {name!r} must not be negative."
                )
