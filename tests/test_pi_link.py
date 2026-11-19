import struct
import pytest
from rover.comms.transport import FakeSerial, HeartbeatLink, SerialTransport
from rover.comms.protocol import DRIVE, STOP, HEARTBEAT, Parser

def test_usb_mock_transport():
    trans = SerialTransport(factory=FakeSerial)
    trans.connect()
    trans.port.rx.extend(b"noise")
    assert trans.receive() == []
    trans.disconnect()

def test_stale_lease_forces_stop_even_while_heartbeat_alive():
    now = [1.0]
    transport = SerialTransport(factory=FakeSerial)
    transport.connect()
    service = HeartbeatLink(transport, clock=lambda:now[0])
    service.drive(0.2, 0, 0)
    now[0] = 1.10
    service.heartbeat_once()
    now[0] = 1.16
    service.heartbeat_once()
    packets=Parser().feed(bytes(transport.port.tx))
    assert [p.type for p in packets] == [DRIVE, HEARTBEAT, STOP, HEARTBEAT]
    assert struct.unpack("<fff", packets[0].payload)[0] == pytest.approx(0.2)

def test_refuses_nan_velocity():
    trans = SerialTransport(factory=FakeSerial)
    trans.connect()
    service=HeartbeatLink(trans)
    with pytest.raises(ValueError):
        service.drive(float("nan"),0,0)
