
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
    """Return configuration issues without accessing hardware."""
    issues = []

    if not config:
        issues.append("Configuration is empty.")

    return issues
