
"""Validation utilities for rover motion commands."""

import math


MIN_SPEED = 0.0
MAX_SPEED = 1.0
MIN_DURATION_S = 0.0
MAX_DURATION_S = 2.0


def validate_speed(speed: float) -> float:
    """Validate and return a finite normalized speed."""
    if isinstance(speed, bool) or not isinstance(speed, (int, float)):
        raise ValueError("Speed must be a number")

    if not math.isfinite(speed):
        raise ValueError("Speed must be finite")

    if not MIN_SPEED <= speed <= MAX_SPEED:
        raise ValueError(
            f"Speed must be between {MIN_SPEED} and {MAX_SPEED}"
        )

    return float(speed)


def validate_duration(duration_s: float) -> float:
    """Validate a positive, bounded movement duration."""
    if (
        isinstance(duration_s, bool)
        or not isinstance(duration_s, (int, float))
    ):
        raise ValueError("Duration must be a number")

    if not math.isfinite(duration_s):
        raise ValueError("Duration must be finite")

    if not MIN_DURATION_S < duration_s <= MAX_DURATION_S:
        raise ValueError(
            f"Duration must be greater than {MIN_DURATION_S} "
            f"and at most {MAX_DURATION_S} seconds"
        )

    return float(duration_s)
