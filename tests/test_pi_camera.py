from rover.sensors.camera import CameraSource

class FakeCamera:
    def __init__(self): self.active=False
    def create_video_configuration(self,**kwargs): return kwargs
    def configure(self,config): pass
    def start(self): self.active=True
    def capture_array(self): return [[1,2,3]]
    def stop(self): self.active=False

def test_camera_mock():
    cam=CameraSource(FakeCamera())
    assert not cam.read().valid
    cam.start()
    frame=cam.read()
    assert frame.valid and frame.timestamp_ns>0
    cam.close()
    assert not cam.read().valid
