from rover.sensors.hcsr04 import UltrasonicArray
import math

class DummyCb:
    def cancel(self): pass

class FakeGPIO:
    def __init__(self): self.handlers={}
    def set_mode(self,*args): pass
    def write(self,*args): pass
    def callback(self,pin,edge,fn): self.handlers[pin]=fn;return DummyCb()
    def gpio_trigger(self,pin,width,level):
        self.handlers[24](24,1,0)
        self.handlers[24](24,0,1000)

def test_pigpio_mock_timestamp_distance():
    a=UltrasonicArray([(23,24)],pigpio_instance=FakeGPIO())
    i, sample = a.ping_next()
    assert i == 0 and sample.valid
    assert math.isclose(sample.distance_m,.1715,rel_tol=.01)
    a.close()

def test_no_sensor_pairs():
    try: UltrasonicArray([],pigpio_instance=FakeGPIO())
    except ValueError: pass
    else: raise AssertionError("accepted empty sensor set")
