"""Pi Camera CSI capture; Picamera2 is installed from Raspberry Pi OS apt."""
from dataclasses import dataclass
import time

@dataclass(frozen=True)
class CameraFrame:
    pixels: object
    timestamp_ns: int
    valid: bool

class CameraSource:
    def __init__(self, camera=None):
        if camera is None:
            from picamera2 import Picamera2
            camera = Picamera2()
        self.camera = camera
        self.started = False

    def start(self):
        self.camera.configure(self.camera.create_video_configuration(main={"format":"RGB888"}))
        self.camera.start()
        self.started = True

    def read(self):
        if not self.started:
            return CameraFrame(None, time.monotonic_ns(), False)
        try:
            frame = self.camera.capture_array()
            return CameraFrame(frame, time.monotonic_ns(), frame is not None)
        except (OSError, RuntimeError):
            return CameraFrame(None, time.monotonic_ns(), False)

    def close(self):
        if self.started:
            self.camera.stop()
        self.started = False
