
"""Tests for the hardware-independent perception interface."""

import pytest

from rover.models import Detection
from rover.perception.base import PerceptionBase


class DummyPerception(PerceptionBase):
    """Minimal implementation used to test the interface."""

    def process_frame(
        self,
        frame,
        timestamp: float,
    ) -> list[Detection]:
        if frame is None:
            raise ValueError("Frame cannot be None")

        if timestamp < 0:
            raise ValueError("Timestamp cannot be negative")

        return []


def test_perception_base_cannot_be_instantiated_directly():
    """The abstract base class must require an implementation."""
    with pytest.raises(TypeError):
        PerceptionBase()


def test_dummy_perception_returns_empty_list_when_no_detections():
    perception = DummyPerception()

    result = perception.process_frame(
        frame=object(),
        timestamp=1.0,
    )

    assert result == []
    assert isinstance(result, list)


def test_dummy_perception_rejects_missing_frame():
    perception = DummyPerception()

    with pytest.raises(ValueError, match="Frame cannot be None"):
        perception.process_frame(frame=None, timestamp=1.0)


def test_dummy_perception_rejects_negative_timestamp():
    perception = DummyPerception()

    with pytest.raises(ValueError, match="Timestamp cannot be negative"):
        perception.process_frame(frame=object(), timestamp=-1.0)


def test_health_check_defaults_to_not_ready():
    perception = DummyPerception()

    assert perception.health_check() is False


def test_concrete_perception_can_override_health_check():
    class ReadyPerception(DummyPerception):
        def health_check(self) -> bool:
            return True

    perception = ReadyPerception()

    assert perception.health_check() is True
