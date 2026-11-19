import pytest
from rover.navigation.commands import NavigationCommand, NavigationAction

def test_reject_nan_speed():
    with pytest.raises(ValueError):
        NavigationCommand(NavigationAction.MOVE_FORWARD, float("nan"), 0.1)

def test_reject_nan_duration():
    with pytest.raises(ValueError):
        NavigationCommand(NavigationAction.MOVE_FORWARD, 0.2, float("nan"))

def test_reject_unbounded_duration():
    with pytest.raises(ValueError):
        NavigationCommand(NavigationAction.MOVE_FORWARD, 0.2, 99.0)

def test_config_rejects_nan():
    from rover.config import validate_config
    assert any("Non-finite" in issue for issue in validate_config({"project": "x", "compute": {"bad":float("nan")}}))
