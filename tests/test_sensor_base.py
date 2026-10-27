
import pytest

from rover.models import SensorReading
from rover.sensors.base import SensorBase


class FakeSensor(SensorBase):
    """Test implementation that does not access hardware."""

    def __init__(self, sensor_id="test_sensor"):
        super().__init__(sensor_id)

    def read(self) -> SensorReading:
        return SensorReading(
            sensor_id=self.sensor_id,
            timestamp=1.0,
            value=12.5,
            unit="test_units",
        )


def test_sensor_accepts_valid_id():
    sensor = FakeSensor("range_sensor")

    assert sensor.sensor_id == "range_sensor"


def test_sensor_rejects_empty_id():
    with pytest.raises(ValueError, match="sensor_id"):
        FakeSensor("")


def test_read_returns_sensor_reading():
    sensor = FakeSensor()

    reading = sensor.read()

    assert isinstance(reading, SensorReading)
    assert reading.sensor_id == "test_sensor"
    assert reading.value == 12.5
    assert reading.valid is True


def test_default_health_check_does_not_claim_availability():
    sensor = FakeSensor()

    assert sensor.health_check() is False


def test_sensor_base_cannot_be_instantiated_directly():
    with pytest.raises(TypeError):
        SensorBase("sensor")
