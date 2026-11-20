"""Authenticated local phone teleoperation endpoint (disabled until commissioned).

A browser is never a safety interlock: STM32 owns the command timeout and E-stop.
Bind to loopback behind a trusted TLS reverse proxy or authenticated private tunnel.
"""
from dataclasses import dataclass
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import hmac
import json
import math
import os
import pathlib


class MotionLocked(Exception):
    """Physical and software commissioning gates are not satisfied."""


@dataclass(frozen=True)
class ManualLimits:
    forward_m_s: float
    lateral_m_s: float
    turn_rad_s: float

    def __post_init__(self):
        values = (self.forward_m_s, self.lateral_m_s, self.turn_rad_s)
        if any(isinstance(v, bool) or not isinstance(v, (int, float))
               or not math.isfinite(v) or v <= 0 for v in values):
            raise ValueError("All manual velocity limits must be measured positive numbers")


class PhoneController:
    def __init__(self, link, *, motion_enabled=False, limits=None):
        self.link = link
        self.motion_enabled = bool(motion_enabled)
        self.limits = limits
        self.arm_requested = False
        self.arm_connection = None

    def arm(self):
        if not self.motion_enabled or self.limits is None:
            raise MotionLocked("Motor actuation is not hardware-authorized")
        if self.link.transport.port is None:
            raise ConnectionError("STM32 USB link is disconnected")
        self.link.arm()
        self.arm_requested = True
        self.arm_connection = self.link.connection_generation
        return {"message": "ARM request sent; STM32 acceptance not verified"}

    def drive(self, axes):
        if not self.motion_enabled or self.limits is None:
            raise MotionLocked("Phone DRIVE is disabled pending hardware acceptance")
        if (not self.arm_requested or self.arm_connection != self.link.connection_generation
                or self.link.transport.port is None):
            raise MotionLocked("Explicit ARM required after each connection")
        if not isinstance(axes, dict) or set(axes) != {"forward", "lateral", "turn"}:
            raise ValueError("Expected normalized forward, lateral and turn axes")
        values = []
        for key in ("forward", "lateral", "turn"):
            v = axes[key]
            if isinstance(v, bool) or not isinstance(v, (int, float)) or not math.isfinite(v) or abs(v) > 1:
                raise ValueError("Axes must be finite numbers in [-1, 1]")
            values.append(float(v))
        self.link.drive(values[0] * self.limits.forward_m_s,
                        values[1] * self.limits.lateral_m_s,
                        values[2] * self.limits.turn_rad_s)
        return {"message": "DRIVE sent; STM32 motion state not verified"}

    def disarm(self):
        # Always permit a stop request, even if the motion gate is off.
        self.arm_requested = False
        self.arm_connection = None
        self.link.disarm()
        return {"message": "DISARM request sent"}

    def status(self):
        return {"motion_configured": self.motion_enabled and self.limits is not None,
                "arm_requested": self.arm_requested,
                "connected": self.link.transport.port is not None,
                "hardware_verified": False}


def make_server(control, token, host="127.0.0.1", port=8080):
    if not isinstance(token, str) or len(token) < 24:
        raise ValueError("AETHER_PHONE_TOKEN must contain at least 24 characters")
    if host not in ("127.0.0.1", "::1", "localhost"):
        raise ValueError("Bind loopback only; expose via authenticated HTTPS proxy / VPN")

    class Handler(BaseHTTPRequestHandler):
        def _send(self, status, payload, content_type="application/json; charset=utf-8"):
            content = payload if isinstance(payload, bytes) else json.dumps(payload).encode()
            self.send_response(status)
            self.send_header("Content-Type", content_type)
            self.send_header("Content-Length", str(len(content)))
            self.send_header("Cache-Control", "no-store")
            self.send_header("X-Content-Type-Options", "nosniff")
            self.send_header("Content-Security-Policy",
                             "default-src 'self'; script-src 'self'; style-src 'self'")
            self.end_headers()
            self.wfile.write(content)

        def _authorized(self):
            supplied = self.headers.get("Authorization", "")
            return hmac.compare_digest(supplied, "Bearer " + token)

        def do_GET(self):
            if self.path == "/":
                webpage = pathlib.Path(__file__).with_name("phone_ui.html").read_bytes()
                self._send(200, webpage, "text/html; charset=utf-8")
            elif self.path == "/api/status":
                self._send(200, control.status()) if self._authorized() else self._send(401, {"error": "unauthorized"})
            else:
                self._send(404, {"error": "not found"})

        def do_POST(self):
            if not self._authorized():
                self._send(401, {"error": "unauthorized"})
                return
            size = self.headers.get("Content-Length", "")
            if not size.isdigit() or not 0 <= int(size) <= 512:
                self._send(413, {"error": "invalid body length"})
                return
            if self.headers.get("Content-Type", "").split(";")[0].strip() != "application/json":
                self._send(415, {"error": "JSON required"})
                return
            try:
                data = json.loads(self.rfile.read(int(size)))
                if not isinstance(data, dict):
                    raise ValueError("JSON object required")
                if self.path == "/api/drive":
                    output = control.drive(data)
                elif self.path == "/api/arm":
                    if data:
                        raise ValueError("ARM takes no payload")
                    output = control.arm()
                elif self.path in ("/api/disarm", "/api/stop"):
                    if data:
                        raise ValueError("STOP takes no payload")
                    output = control.disarm()
                else:
                    self._send(404, {"error": "not found"})
                    return
            except (ValueError, TypeError) as exc:
                self._send(400, {"error": str(exc)})
                return
            except MotionLocked as exc:
                self._send(423, {"error": str(exc)})
                return
            except (OSError, ConnectionError):
                self._send(503, {"error": "STM32 link unavailable"})
                return
            self._send(202, output)

        def log_message(self, format, *args):
            pass  # Avoid logging the auth header or other secrets.

    return ThreadingHTTPServer((host, port), Handler)


def from_environment(link):
    enabled = os.environ.get("AETHER_PHONE_ENABLE_MOTION") == "1"
    speeds = [os.environ.get(k) for k in ("AETHER_PHONE_MAX_VX",
                                        "AETHER_PHONE_MAX_VY",
                                        "AETHER_PHONE_MAX_WZ")]
    limits = ManualLimits(*(float(v) for v in speeds)) if all(speeds) else None
    return PhoneController(link, motion_enabled=enabled, limits=limits)
