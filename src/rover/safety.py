
"""Hardware-independent safety checks for the rover."""

from dataclasses import dataclass


@dataclass(frozen=True)
class SafetyDecision:
    """Result of evaluating whether a motion request may proceed."""

    permitted: bool
    reason: str


def evaluate_safety(
    *,
    emergency_stop_active: bool,
    command_timestamp: float,
    now: float,
    max_command_age: float,
    required_sensors_valid: bool,
) -> SafetyDecision:
    """Evaluate a motion request without accessing hardware.

    Timestamps must use the same clock and time base.
    The caller is responsible for enforcing the decision.
    """
    if max_command_age <= 0:
        raise ValueError("max_command_age must be positive.")

    if command_timestamp < 0 or now < 0:
        raise ValueError("Timestamps must not be negative.")

    if command_timestamp > now:
        return SafetyDecision(
            permitted=False,
            reason="Command timestamp is in the future.",
        )

    if emergency_stop_active:
        return SafetyDecision(
            permitted=False,
            reason="Emergency stop is active.",
        )

    if not required_sensors_valid:
        return SafetyDecision(
            permitted=False,
            reason="Required sensor data is invalid.",
        )

    command_age = now - command_timestamp

    if command_age > max_command_age:
        return SafetyDecision(
            permitted=False,
            reason="Motion command is stale.",
        )

    return SafetyDecision(
        permitted=True,
        reason="Safety checks passed.",
    )
