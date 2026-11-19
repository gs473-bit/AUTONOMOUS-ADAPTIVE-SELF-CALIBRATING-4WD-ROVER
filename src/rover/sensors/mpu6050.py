"""Pi-side GY-521 / MPU6050. No STM32 I2C ownership."""
from dataclasses import dataclass
import math
import time

@dataclass(frozen=True)
class ImuSample:
    timestamp_ns: int
    gyro_rad_s: tuple[float, float, float]
    accel_m_s2: tuple[float, float, float]
    valid: bool

class MPU6050:
    ADDRESS = 0x68
    ACCEL_PER_G = 16384.0  # ±2g
    GYRO_PER_DPS = 131.0  # ±250 dps
    def __init__(self, bus=None, address=ADDRESS, bus_id=1):
        self.address = address
        self.bus_id = bus_id
        self._external = bus is not None
        self.bus = bus
        self.bias = (0.0, 0.0, 0.0)
        self.failures = 0
        if self.bus is None:
            from smbus2 import SMBus
            self.bus = SMBus(bus_id)
        self.initialize()

    def initialize(self):
        # Wake device and select documented ±2g / ±250dps ranges, DLPF 44Hz.
        for register, value in ((0x6B, 0), (0x1A, 3), (0x1B, 0), (0x1C, 0)):
            self.bus.write_byte_data(self.address, register, value)

    @staticmethod
    def _int16(hi, lo):
        value = (hi << 8) | lo
        return value - 65536 if value >= 32768 else value

    def read(self):
        try:
            data = self.bus.read_i2c_block_data(self.address, 0x3B, 14)
            if len(data) != 14:
                raise OSError("short MPU6050 read")
            acc = tuple(self._int16(data[i], data[i + 1]) * 9.80665 / self.ACCEL_PER_G for i in (0, 2, 4))
            gyro = tuple(self._int16(data[i], data[i + 1]) * math.pi / (180.0 * self.GYRO_PER_DPS) - b
                         for i, b in zip((8, 10, 12), self.bias))
            self.failures = 0
            return ImuSample(time.monotonic_ns(), gyro, acc, True)
        except (OSError, IOError, ValueError):
            self.failures += 1
            if self.failures >= 3 and not self._external:
                try:
                    self.bus.close()
                    from smbus2 import SMBus
                    self.bus = SMBus(self.bus_id)
                    self.initialize()
                except (OSError, IOError):
                    pass
            return ImuSample(time.monotonic_ns(), (math.nan,) * 3, (math.nan,) * 3, False)

    def calibrate_gyro(self, count=200, period_s=0.005, sleep=time.sleep):
        """Stationary and with all motors disabled; reject excessive motion."""
        values = []
        self.bias = (0.0, 0.0, 0.0)
        for _ in range(count):
            sample = self.read()
            if not sample.valid:
                raise RuntimeError("IMU disconnected during calibration")
            magnitude = math.sqrt(sum(x*x for x in sample.accel_m_s2))
            if not 7.0 <= magnitude <= 12.5:
                raise RuntimeError("IMU must be stationary near 1g")
            values.append(sample.gyro_rad_s)
            sleep(period_s)
        bias = tuple(sum(x[i] for x in values) / count for i in range(3))
        if max(abs(x[i] - bias[i]) for x in values for i in range(3)) > 0.20:
            raise RuntimeError("excessive motion during bias calibration")
        self.bias = bias
        return bias

    def samples(self, rate_hz=200):
        period = 1.0 / rate_hz
        deadline = time.monotonic()
        while True:
            yield self.read()
            deadline += period
            time.sleep(max(0.0, deadline - time.monotonic()))

    def close(self):
        if not self._external:
            self.bus.close()
