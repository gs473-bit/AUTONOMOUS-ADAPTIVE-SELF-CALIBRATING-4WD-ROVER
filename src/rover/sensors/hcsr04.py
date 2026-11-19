"""Pi-side HC-SR04 with hardware timestamped pigpio callbacks."""
from dataclasses import dataclass
import math
import threading
import time

@dataclass(frozen=True)
class RangeSample:
    distance_m: float
    timestamp_ns: int
    valid: bool

class UltrasonicArray:
    def __init__(self, pairs, pigpio_instance=None, speed_m_s=343.0):
        if not pairs:
            raise ValueError("Provide at least one (TRIG, ECHO) GPIO pair")
        self._owns_gpio = pigpio_instance is None
        if pigpio_instance is None:
            import pigpio
            pigpio_instance = pigpio.pi()
            if not pigpio_instance.connected:
                raise RuntimeError("pigpiod unavailable")
        self.gpio = pigpio_instance
        self.pairs = list(pairs)
        self.speed_m_s = speed_m_s
        self.lock = threading.Lock()
        self._callback = []
        self._edges = [None] * len(self.pairs)
        self._events = [threading.Event() for _ in self.pairs]
        self._index = 0
        self._last_ping = 0.0
        for i, (trig, echo) in enumerate(self.pairs):
            self.gpio.set_mode(trig, 1)  # output
            self.gpio.set_mode(echo, 0)  # input, must already be level-shifted
            self.gpio.write(trig, 0)
            self._callback.append(self.gpio.callback(echo, 2, self._edge_handler(i)))

    def _edge_handler(self, i):
        def handler(gpio, level, tick):
            del gpio
            if level == 1:
                self._edges[i] = tick
            elif level == 0 and self._edges[i] is not None:
                # 32-bit microsecond timestamps wrap naturally.
                width = (tick - self._edges[i]) & 0xffffffff
                self._edges[i] = None
                if 100 <= width <= 25000:
                    self._width = width
                    self._events[i].set()
        return handler

    def ping_next(self):
        with self.lock:
            elapsed = time.monotonic() - self._last_ping
            if elapsed < 0.060:
                time.sleep(0.060 - elapsed)  # >=60ms avoids cross-sensor echoes
            i = self._index
            self._index = (i+1) % len(self.pairs)
            self._events[i].clear()
            self._edges[i] = None
            self._width = None
            self._last_ping = time.monotonic()
            self.gpio.gpio_trigger(self.pairs[i][0], 10, 1)
            received = self._events[i].wait(0.030)
            distance = self._width * 1e-6 * self.speed_m_s / 2 if received and self._width is not None else math.nan
            valid = bool(received and 0.02 <= distance <= 4.0)
            return i, RangeSample(distance if valid else math.nan, time.monotonic_ns(), valid)

    def close(self):
        for cb in self._callback:
            cb.cancel()
        if self._owns_gpio:
            self.gpio.stop()
