
import pytest

from rover.config import load_config, validate_config


def test_load_config_from_yaml(tmp_path):
    config_file = tmp_path / "rover.yaml"
    config_file.write_text(
        "rover:\n  name: test-rover\n",
        encoding="utf-8",
    )

    config = load_config(config_file)

    assert config["rover"]["name"] == "test-rover"


def test_load_config_missing_file(tmp_path):
    missing_file = tmp_path / "missing.yaml"

    with pytest.raises(FileNotFoundError):
        load_config(missing_file)


def test_load_config_invalid_yaml(tmp_path):
    config_file = tmp_path / "invalid.yaml"
    config_file.write_text("rover: [unclosed", encoding="utf-8")

    with pytest.raises(ValueError, match="Invalid YAML"):
        load_config(config_file)


def test_load_config_requires_mapping(tmp_path):
    config_file = tmp_path / "list.yaml"
    config_file.write_text("- item1\n- item2\n", encoding="utf-8")

    with pytest.raises(ValueError, match="root"):
        load_config(config_file)


def test_validate_config_empty():
    issues = validate_config({})

    assert "Configuration is empty." in issues


def test_validate_config_nonempty():
    assert validate_config({"rover": {"name": "test"}}) == []
