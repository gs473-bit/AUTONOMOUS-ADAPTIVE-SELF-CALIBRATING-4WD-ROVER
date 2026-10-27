
"""Validate and limit requested rover velocities.

This module does not communicate with motor drivers or activate hardware.
"""

from dataclasses import dataclass


@dataclass(frozen=True)
class MotionLimits:
    """Maximum permitted absolute linear and angular velocities."""

    max_linear_velocity: float
    max_angular_velocity: float

    def __post_init__(self) -> None:
        if self.max_linear_velocity <= 0:
            raise ValueError("Maximum linear velocity must be positive.")

        if self.max_angular_velocity <= 0:
            raise ValueError("Maximum angular velocity must be positive.")


@dataclass(frozen=True)
class MotionRequest:
    """A bounded motion request expressed in SI units."""

    linear_velocity: float
    angular_velocity: float


def limit_motion(
    linear_velocity: float,
    angular_velocity: float,
    limits: MotionLimits,
) -> MotionRequest:
    """Clamp requested velocities to the configured limits."""
    bounded_linear = max(
        -limits.max_linear_velocity,
        min(linear_velocity, limits.max_linear_velocity),
    )

    bounded_angular = max(
        -limits.max_angular_velocity,
        min(angular_velocity, limits.max_angular_velocity),
    )

    return MotionRequest(
        linear_velocity=bounded_linear,
        angular_velocity=bounded_angular,
    )
