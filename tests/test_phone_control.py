"""Host-mocked mobile control authorization and command safety gates."""
import http.client
import threading
import pytest
from rover.phone_control import PhoneController, ManualLimits, MotionLocked, make_server

class FakeLink:
    def __init__(self):
        self.transport=type("T", (), {"port": object()})()
        self.connection_generation=1
        self.sent=[]
    def arm(self): self.sent.append(("ARM",))
    def drive(self,x,y,z): self.sent.append(("DRIVE",x,y,z))
    def disarm(self): self.sent.append(("DISARM",))

def test_disabled_motion_even_with_link():
    controller=PhoneController(FakeLink())
    with pytest.raises(MotionLocked): controller.arm()
    with pytest.raises(MotionLocked):
        controller.drive({"forward":1,"lateral":0,"turn":0})
    assert controller.disarm()["message"] == "DISARM request sent"

def test_arm_requires_explicit_request_and_calibrated_limits():
    link=FakeLink()
    c=PhoneController(link,motion_enabled=True,limits=ManualLimits(.15,.12,.25))
    with pytest.raises(MotionLocked):
        c.drive({"forward":1,"lateral":0,"turn":0})
    assert c.arm()["message"].startswith("ARM request")
    assert c.drive({"forward":1,"lateral":-1,"turn":.5})["message"].startswith("DRIVE sent")
    assert link.sent[-1][0] == "DRIVE"
    assert link.sent[-1][1:] == pytest.approx((.15,-.12,.125),rel=1e-4)
    with pytest.raises(ValueError):
        c.drive({"forward":float("nan"),"lateral":0,"turn":0})
    with pytest.raises(ValueError):
        c.drive({"forward":2,"lateral":0,"turn":0})
    c.disarm()
    with pytest.raises(MotionLocked):
        c.drive({"forward":1,"lateral":0,"turn":0})

def test_usb_reconnect_invalidates_arm():
    link=FakeLink()
    c=PhoneController(link,motion_enabled=True,limits=ManualLimits(.1,.1,.1))
    c.arm()
    link.connection_generation+=1
    with pytest.raises(MotionLocked):
        c.drive({"forward":1,"lateral":0,"turn":0})
    assert not c.status()["arm_requested"]

def test_credentials_and_ui_static_assets():
    link=FakeLink()
    server=make_server(PhoneController(link), "a-long-secret-token-for-local-use",port=0)
    th=threading.Thread(target=server.serve_forever,daemon=True)
    th.start()
    try:
        c=http.client.HTTPConnection("127.0.0.1",server.server_port,timeout=2)
        c.request("GET","/phone.js")
        r=c.getresponse()
        assert r.status==200 and b"pointerdown" in r.read()
        c.request("GET","/phone.css")
        r=c.getresponse()
        assert r.status==200 and b"body{" in r.read()
        c.request("POST","/api/drive","{}",{"Content-Type":"application/json"})
        r=c.getresponse()
        assert r.status==401
        r.read()
        c.request("POST","/api/arm","{}",{"Content-Type":"application/json",
             "Authorization":"Bearer a-long-secret-token-for-local-use"})
        r=c.getresponse()
        assert r.status==423
        r.read()
        c.request("POST","/api/stop","{}",{"Content-Type":"application/json",
             "Authorization":"Bearer a-long-secret-token-for-local-use"})
        r=c.getresponse()
        assert r.status==202
        r.read()
        assert ("DISARM",) in link.sent
        c.close()
    finally:
        server.shutdown()
        server.server_close()
        th.join(timeout=2)

def test_reject_weak_token_remote_bind_and_invalid_limits():
    with pytest.raises(ValueError):
        make_server(PhoneController(FakeLink()),"weak",port=0)
    with pytest.raises(ValueError):
        make_server(PhoneController(FakeLink()),"a-long-secret-token-for-local-use",
                    host="0.0.0.0",port=0)
    for limit in (0,-1,float("inf"),float("nan")):
        with pytest.raises(ValueError):
            ManualLimits(limit,.1,.1)
