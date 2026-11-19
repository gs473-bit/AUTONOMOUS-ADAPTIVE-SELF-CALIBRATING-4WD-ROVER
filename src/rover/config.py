
"""Load and validate the rover's YAML configuration."""

from pathlib import Path
from typing import Any

import yaml


DEFAULT_CONFIG_PATH = (
    Path(__file__).resolve().parents[2] / "config" / "rover.yaml"
)


def load_config(path: str | Path = DEFAULT_CONFIG_PATH) -> dict[str, Any]:
    """Load a YAML configuration and ensure its root is a mapping."""
    config_path = Path(path)

    if not config_path.is_file():
        raise FileNotFoundError(
            f"Configuration file not found: {config_path}"
        )

    try:
        with config_path.open("r", encoding="utf-8") as file:
            config = yaml.safe_load(file)
    except yaml.YAMLError as exc:
        raise ValueError(
            f"Invalid YAML in {config_path}: {exc}"
        ) from exc

    if not isinstance(config, dict):
        raise ValueError(
            "Configuration must contain a YAML mapping at its root."
        )

    return config


def validate_config(config: dict[str, Any]) -> list[str]:
    """Validate structure and all supplied numeric values; null is an unresolved TODO."""
    import math
    issues: list[str] = []
    if not isinstance(config, dict) or not config:
        return ["Configuration is empty."]
    required_sections = {
        "project": str, "compute": dict, "camera": dict, "motor_driver": dict,
        "motors": dict, "encoders": dict, "sensors": dict, "power": dict,
        "safety": dict,
    }
    for key, typ in required_sections.items():
        if key not in config:
            issues.append(f"Missing required key: {key}")
        elif not isinstance(config[key], typ):
            issues.append(f"Invalid type for {key}: expected {typ.__name__}")

    def walk(value, path=""):
        if isinstance(value, dict):
            for key, child in value.items():
                walk(child, f"{path}.{key}" if path else key)
        elif isinstance(value, list):
            for index, child in enumerate(value):
                walk(child, f"{path}[{index}]")
        elif isinstance(value, float) and not math.isfinite(value):
            issues.append(f"Non-finite numeric config value: {path}")
    walk(config)
    for section, key in (("motors", "count_target"), ("motor_driver", "board_count_target")):
        obj = config.get(section)
        if isinstance(obj, dict) and key in obj:
            v = obj[key]
            if isinstance(v, bool) or not isinstance(v, int) or v <= 0:
                issues.append(f"{section}.{key} must be positive integer")
    if isinstance(config.get("motors"), dict) and config["motors"].get("count_target") not in (None, 4):
        issues.append("AETHER-X requires exactly four drive motors")
    for section, key in (("motors", "rated_voltage_v"), ("power", "nominal_voltage_v"),
                         ("encoders", "counts_per_wheel_revolution")):
        obj = config.get(section)
        if isinstance(obj, dict) and obj.get(key) is not None:
            v = obj[key]
            if isinstance(v, bool) or not isinstance(v, (int, float)) or not math.isfinite(v) or v <= 0:
                issues.append(f"{section}.{key} must be finite positive numeric")
    return issues
