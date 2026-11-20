"""USB serial framed transport. Reconnection never replays DRIVE or ARM."""
import math
import struct
import threading
import time

from rover.comms.protocol import (
    Parser, Packet, encode, DRIVE, STOP, ARM, DISARM, HEARTBEAT,
)

class SerialTransport:
    def __init__(self, device="/dev/aether_stm32", baudrate=115200, factory=None):
        if factory is None:
            import serial
            factory = serial.Serial
        self.device = device
        self.baudrate = baudrate
        self.factory = factory
        self.port = None
        self.parser = Parser()

    def connect(self):
        self.disconnect()
        self.port = self.factory(self.device, self.baudrate, timeout=0.02)
        self.parser = Parser()

    def disconnect(self):
        if self.port is not None:
            try:
                self.port.close()
            finally:
                self.port = None

    def send(self, packet):
        if self.port is None:
            raise ConnectionError("STM32 VCP unavailable")
        self.port.write(encode(packet))

    def receive(self):
        if self.port is None:
            raise ConnectionError("STM32 VCP unavailable")
        return self.parser.feed(self.port.read(256))

class FakeSerial:
    def __init__(self, *args, **kwargs):
        self.closed = False
        self.rx = bytearray()
        self.tx = bytearray()
    def write(self, frame):
        if self.closed:
            raise OSError("unplugged")
        self.tx.extend(frame)
        return len(frame)
    def read(self, n):
        chunk = bytes(self.rx[:n])
        del self.rx[:n]
        return chunk
    def close(self):
        self.closed = True

class HeartbeatLink:
    """20-50Hz heartbeat thread independent of planner; stale DRIVE becomes STOP."""
    def __init__(self, transport=None, period=0.04, lease_s=0.15, clock=time.monotonic):
        if not 0 < period < lease_s < 0.2:
            raise ValueError("Heartbeat/command lease must be below STM32 200ms timeout")
        self.transport = transport or SerialTransport()
        self.period = period
        self.lease_s = lease_s
        self.clock = clock
        self.seq = 0
        self.last_drive_time = None
        self.connection_generation = 0
        self.lock = threading.RLock()
        self.stop_event = threading.Event()
        self.thread = None

    def _send(self, kind, payload=b""):
        packet = Packet(self.seq, kind, payload)
        self.transport.send(packet)
        self.seq = (self.seq + 1) & 0xFFFF

    def arm(self):
        with self.lock:
            self._send(ARM)

    def drive(self, vx, vy, wz):
        if not all(math.isfinite(v) for v in (vx, vy, wz)):
            raise ValueError("velocity components must be finite")
        with self.lock:
            self._send(DRIVE, struct.pack("<fff", vx, vy, wz))
            self.last_drive_time = self.clock()

    def disarm(self):
        with self.lock:
            self._send(DISARM)
            self.last_drive_time = None

    def heartbeat_once(self):
        with self.lock:
            if self.last_drive_time is not None and self.clock() - self.last_drive_time >= self.lease_s:
                self._send(STOP)
                self.last_drive_time = None
            self._send(HEARTBEAT)

    def _run(self):
        while not self.stop_event.is_set():
            try:
                if self.transport.port is None:
                    self.transport.connect()
                    self.seq = 0
                    self.last_drive_time = float("-inf")
                    # Never ARM after reconnection. MCU must require explicit ARM.
                self.heartbeat_once()
                self.transport.receive()
            except (OSError, ConnectionError):
                self.transport.disconnect()
            self.stop_event.wait(self.period)

    def start(self):
        if self.thread and self.thread.is_alive():
            return
        self.stop_event.clear()
        self.thread = threading.Thread(target=self._run, name="aether-heartbeat", daemon=True)
        self.thread.start()

    def close(self):
        self.stop_event.set()
        if self.thread:
            self.thread.join(timeout=1.0)
        self.transport.disconnect()
