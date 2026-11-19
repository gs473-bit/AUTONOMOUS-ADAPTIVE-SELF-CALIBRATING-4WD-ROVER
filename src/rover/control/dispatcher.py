
"""Safety-gated dispatcher for navigation commands."""

from dataclasses import dataclass
import math
import threading
import time

from rover.control.motor_interface import MotorInterface
from rover.safety import evaluate_safety
from rover.navigation.commands import NavigationAction, NavigationCommand


@dataclass(frozen=True)
class DispatchResult:
    """Outcome of a navigation command."""

    accepted: bool
    reason: str


class CommandDispatcher:
    """Validate commands before forwarding them to a motor interface.

    This class does not implement a physical emergency-stop circuit.
    Hardware-level protections must be implemented independently.
    """

    def __init__(self, motor: MotorInterface, *, clock=None, timer_factory=None) -> None:
        self._motor = motor
        self._emergency_stop_active = True
        self._fault_active = False
        self._required_sensors_valid = True
        self._max_command_age = 0.2
        self._clock = clock or time.monotonic
        self._timer_factory = timer_factory or threading.Timer
        self._timer = None
        self._deadline = None
        self._generation = 0
        self._lock = threading.RLock()

    def _cancel_timer(self):
        self._generation += 1
        if self._timer is not None:
            self._timer.cancel()
            self._timer = None
        self._deadline = None

    def _timeout(self, generation):
        with self._lock:
            if generation != self._generation:
                return
            remaining = self._deadline - self._clock()
            if remaining > 0:
                self._timer = self._timer_factory(remaining, self._timeout, args=(generation,))
                self._timer.daemon = True
                self._timer.start()
                return
            self._motor.stop()
            self._cancel_timer()

    def poll_watchdog(self):
        """Diagnostic / deterministic fake-clock evaluation (not hardware watchdog)."""
        with self._lock:
            if self._deadline is not None and self._clock() >= self._deadline:
                self._timeout(self._generation)

    @property
    def emergency_stop_active(self) -> bool:
        return self._emergency_stop_active

    def set_emergency_stop(self, active: bool) -> None:
        """Update the software emergency-stop state."""
        self._emergency_stop_active = bool(active)

        if self._emergency_stop_active:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()

    def set_fault(self, active: bool):
        self._fault_active = bool(active)
        if self._fault_active:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()

    def set_required_sensors_valid(self, valid: bool):
        self._required_sensors_valid = bool(valid)
        if not valid:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()

    def dispatch(self, command: NavigationCommand, *, command_timestamp=None, now=None) -> DispatchResult:
        """Validate a command and forward permitted requests."""
        if not isinstance(command, NavigationCommand):
            return DispatchResult(False, "Invalid command type")

        if command.action == NavigationAction.STOP:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()
            return DispatchResult(True, "Stop command accepted")

        current_time = self._clock() if now is None else now
        timestamp = current_time if command_timestamp is None else command_timestamp
        if self._fault_active:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()
            return DispatchResult(False, "Motor fault active")
        decision = evaluate_safety(
            emergency_stop_active=self._emergency_stop_active,
            command_timestamp=timestamp,
            now=current_time,
            max_command_age=self._max_command_age,
            required_sensors_valid=self._required_sensors_valid,
        )
        if not decision.permitted:
            with self._lock:
                self._cancel_timer()
                self._motor.stop()
            return DispatchResult(False, decision.reason.rstrip("."))



        if (
            not math.isfinite(command.speed)
            or not 0.0 < command.speed <= 1.0
        ):
            self._motor.stop()
            return DispatchResult(False, "Invalid movement speed")

        if (
            not math.isfinite(command.duration_s)
            or command.duration_s <= 0.0
        ):
            self._motor.stop()
            return DispatchResult(False, "Invalid movement duration")

        speed = command.speed

        if command.action == NavigationAction.MOVE_FORWARD:
            fl, fr, rl, rr = speed, speed, speed, speed
        elif command.action == NavigationAction.MOVE_BACKWARD:
            fl, fr, rl, rr = -speed, -speed, -speed, -speed
        elif command.action == NavigationAction.TURN_LEFT:
            fl, fr, rl, rr = -speed, speed, -speed, speed
        elif command.action == NavigationAction.TURN_RIGHT:
            fl, fr, rl, rr = speed, -speed, speed, -speed
        else:
            self._motor.stop()
            return DispatchResult(False, "Unsupported navigation action")

        try:
            with self._lock:
                self._cancel_timer()
                self._motor.set_wheel_speeds(fl, fr, rl, rr)
                self._deadline = self._clock() + command.duration_s
                generation = self._generation
                self._timer = self._timer_factory(command.duration_s, self._timeout, args=(generation,))
                self._timer.daemon = True
                self._timer.start()
        except Exception:
            try:
                self._motor.stop()
            finally:
                return DispatchResult(False, "Motor command failed")

        return DispatchResult(True, "Command passed software checks")
