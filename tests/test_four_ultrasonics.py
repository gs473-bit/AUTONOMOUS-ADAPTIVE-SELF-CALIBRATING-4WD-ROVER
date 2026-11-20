import pytest
from rover.sensors.hcsr04 import UltrasonicArray

class MockCallback:
    def cancel(self): pass

class MockGPIO:
    def __init__(self): self.callbacks={};self.active=[]
    def set_mode(self,*x): pass
    def write(self,*x): pass
    def callback(self,pin,edge,fn):
        self.callbacks[pin]=fn
        return MockCallback()
    def gpio_trigger(self,pin,width,level):
        self.active.append(pin)
        # Four distinguishable, plausible echo widths (microseconds)
        echo={5:6,7:8,9:10,11:12}[pin]
        self.callbacks[echo](echo,1,100)
        self.callbacks[echo](echo,0,1000+pin)

def test_four_sensors_one_at_a_time():
    gpio=MockGPIO()
    configuration={"ultrasonic_count":4,"ultrasonic_gpio_pairs":[[5,6],[7,8],[9,10],[11,12]]}
    array=UltrasonicArray.from_config(configuration,pigpio_instance=gpio)
    try:
        for index in range(4):
            i, reading=array.ping_next()
            assert i==index and reading.valid and .02<reading.distance_m<4
        assert gpio.active==[5,7,9,11]
    finally:
        array.close()

def test_missing_gpio_map_fails_before_pin_initialization():
    gpio=MockGPIO()
    with pytest.raises(ValueError):
        UltrasonicArray.from_config({"ultrasonic_count":4,"ultrasonic_gpio_pairs":None},
                                    pigpio_instance=gpio)
    assert gpio.callbacks=={}

@pytest.mark.parametrize("pairs", [
    [[5,6],[7,8],[9,10],[11,11]],
    [[5,6],[7,8],[9,10],[5,12]],
    [[2,3],[7,8],[9,10],[11,12]],
])
def test_invalid_or_reserved_gpio_map(pairs):
    with pytest.raises(ValueError):
        UltrasonicArray.from_config({"ultrasonic_count":4,"ultrasonic_gpio_pairs":pairs},
                                    pigpio_instance=MockGPIO())
