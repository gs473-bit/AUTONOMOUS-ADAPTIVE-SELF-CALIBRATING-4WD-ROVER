from rover.control.dispatcher import CommandDispatcher
from rover.navigation.commands import NavigationCommand

class FakeTimer:
    def __init__(self, delay, fn, args=()):
        self.delay, self.fn, self.args = delay, fn, args
        self.daemon = False
        self.cancelled = False
    def start(self): pass
    def cancel(self): self.cancelled = True
    def fire(self):
        if not self.cancelled: self.fn(*self.args)

class FakeMotor:
    def __init__(self): self.speed=(0,0,0,0)
    def set_wheel_speeds(self,*v): self.speed=v
    def stop(self): self.speed=(0,0,0,0)
    def close(self): self.stop()

def test_timer_stops_motors_without_backend_help():
    now=[100.0]; timers=[]
    def factory(*args,**kwargs):
        t=FakeTimer(*args,**kwargs);timers.append(t);return t
    motor=FakeMotor()
    d=CommandDispatcher(motor,clock=lambda:now[0],timer_factory=factory)
    d.set_emergency_stop(False)
    assert d.dispatch(NavigationCommand.forward(.5, .4)).accepted
    assert motor.speed == (.5,)*4
    now[0]+=0.5
    timers[0].fire()
    assert motor.speed == (0,0,0,0)

def test_old_timer_cannot_stop_newer_command():
    now=[0.0];timers=[]
    def factory(*args,**kwargs):
        t=FakeTimer(*args,**kwargs);timers.append(t);return t
    motor=FakeMotor();d=CommandDispatcher(motor,clock=lambda:now[0],timer_factory=factory)
    d.set_emergency_stop(False)
    d.dispatch(NavigationCommand.forward(.4,1.0))
    old=timers[-1]
    now[0]+=.2
    d.dispatch(NavigationCommand.forward(.3,1.0))
    old.fn(*old.args)
    assert motor.speed == (.3,)*4
    now[0]+=1.2
    timers[-1].fire()
    assert motor.speed == (0,0,0,0)
