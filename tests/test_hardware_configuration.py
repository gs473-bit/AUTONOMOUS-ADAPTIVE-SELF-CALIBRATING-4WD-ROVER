from pathlib import Path
import yaml

CONFIG = Path(__file__).resolve().parents[1] / "config" / "rover.yaml"

def test_split_hardware_configuration_confirmations():
    data = yaml.safe_load(CONFIG.read_text())
    assert data["sensors"]["ultrasonic_count"] == 4
    assert data["sensors"]["ultrasonic_gpio_pairs"] is None
    assert data["remote_control"]["type"] == "smartphone_web"
    assert data["remote_control"]["owner"] == "raspberry_pi"
    assert data["remote_control"]["active_by_default"] is False
    assert "rc" not in data
    assert data["power"]["continuous_discharge_current_a"] == 4.0
    assert data["power"]["capacity_ah"] == 4.0
    assert data["power"]["nominal_voltage_v"] == 12.0
    assert data["motors"]["gearbox_ratio"] is None
    assert data["encoders"]["counts_per_wheel_revolution"] is None
