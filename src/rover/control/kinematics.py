"""Mecanum inverse kinematics (X configuration). Confirm real wheel signs on stands."""
from dataclasses import dataclass
import math
from rover.control.motor_interface import _wheel_value


@dataclass(frozen=True)
class MecanumGeometry:
    radius_m: float
    wheelbase_m: float
    track_width_m: float

    def __post_init__(self):
        for name in ("radius_m", "wheelbase_m", "track_width_m"):
            value = getattr(self, name)
            if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value) or value <= 0:
                raise ValueError(f"{name} must be finite and positive")

    @classmethod
    def from_config(cls, config: dict):
        geometry = config["mecanum"]
        return cls(float(geometry["wheel_radius_m"]), float(geometry["wheelbase_m"]), float(geometry["track_width_m"]))


def inverse_mecanum(vx: float, vy: float, wz: float, geometry: MecanumGeometry, max_wheel_rad_s: float) -> tuple[float, float, float, float]:
    """Return normalized FL, FR, RL, RR. Does not clamp: rejects infeasible speeds."""
    if any(isinstance(x, bool) or not isinstance(x, (int, float)) or not math.isfinite(x) for x in (vx, vy, wz, max_wheel_rad_s)):
        raise ValueError("Non-finite or nonnumeric chassis velocity/speed limit")
    if max_wheel_rad_s <= 0:
        raise ValueError("max_wheel_rad_s must be positive")
    k = (geometry.wheelbase_m + geometry.track_width_m) / 2.0
    r = geometry.radius_m
    speeds = ((vx-vy-k*wz)/r, (vx+vy+k*wz)/r, (vx+vy-k*wz)/r, (vx-vy+k*wz)/r)
    return tuple(_wheel_value(x / max_wheel_rad_s) for x in speeds)
