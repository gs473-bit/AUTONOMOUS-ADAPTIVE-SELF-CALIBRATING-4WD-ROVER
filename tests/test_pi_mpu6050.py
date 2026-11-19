import math
import pytest
from rover.sensors.mpu6050 import MPU6050

class FakeBus:
    def __init__(self, fail=False):
        self.registers = {}
        self.fail = fail
    def write_byte_data(self, a, r, v):
        self.registers[r] = v
    def read_i2c_block_data(self, a, r, n):
        if self.fail:
            raise OSError("simulated I2C disconnect")
        # +1g Z, zero gyro
        return [0,0,0,0,0x40,0,0,0,0,0,0,0,0,0]

def test_fake_imu_sensor_and_calibration():
    bus = FakeBus()
    imu = MPU6050(bus=bus)
    assert bus.registers[0x6B] == 0
    sample = imu.read()
    assert sample.valid
    assert sample.accel_m_s2[2] == pytest.approx(9.80665)
    assert imu.calibrate_gyro(count=4, sleep=lambda _: None) == pytest.approx((0,0,0))

def test_fake_imu_i2c_fault_is_invalid():
    imu = MPU6050(bus=FakeBus(True))
    sample = imu.read()
    assert not sample.valid
    assert math.isnan(sample.gyro_rad_s[0])
