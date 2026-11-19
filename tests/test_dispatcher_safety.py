from rover.control.dispatcher import CommandDispatcher
from rover.navigation.commands import NavigationCommand

class FakeMotor:
    def __init__(self): self.wheels=(0,0,0,0)
    def set_wheel_speeds(self,*v): self.wheels=v
    def stop(self): self.wheels=(0,0,0,0)
    def close(self): self.stop()

class NoTimer:
    def __init__(self,*a,**kw): self.daemon=False
    def start(self): pass
    def cancel(self): pass

def test_stale_command_blocked():
    m=FakeMotor();d=CommandDispatcher(m,clock=lambda:10.0,timer_factory=NoTimer)
    d.set_emergency_stop(False)
    result=d.dispatch(NavigationCommand.forward(.2,.5),command_timestamp=9.0)
    assert not result.accepted and m.wheels==(0,0,0,0)

def test_sensor_fault_blocks():
    m=FakeMotor();d=CommandDispatcher(m,timer_factory=NoTimer)
    d.set_emergency_stop(False);d.set_required_sensors_valid(False)
    assert not d.dispatch(NavigationCommand.forward(.2,.5)).accepted

def test_fault_stops_active_motor():
    m=FakeMotor();d=CommandDispatcher(m,timer_factory=NoTimer)
    d.set_emergency_stop(False)
    assert d.dispatch(NavigationCommand.forward(.2,.5)).accepted
    d.set_fault(True)
    assert m.wheels == (0,0,0,0)
    assert not d.dispatch(NavigationCommand.forward(.2,.5)).accepted
