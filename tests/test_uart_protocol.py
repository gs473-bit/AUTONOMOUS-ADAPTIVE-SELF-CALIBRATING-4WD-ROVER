"""Host-mocked parity tests for the STM32 byte-level wire format."""
import struct
from rover.comms.protocol import Parser, Packet, DRIVE, STOP, encode

def test_wire_format_crc_and_payload():
    p = Packet(1, DRIVE, struct.pack("<fff", 0.1, 0.2, -0.3))
    raw = encode(p)
    assert raw[:2] == b"\xa5\x5a"
    assert struct.unpack("<H", raw[2:4])[0] == 12
    assert Parser().feed(raw) == [p]

def test_split_frames_and_noise():
    p = Parser()
    data = encode(Packet(1, STOP))
    assert p.feed(b"noise" + data[:3]) == []
    assert p.feed(data[3:]) == [Packet(1, STOP)]

def test_bad_crc_and_sequence_gap_drop():
    p = Parser()
    p.feed(encode(Packet(0, DRIVE, b"")))
    broken = bytearray(encode(Packet(1, DRIVE, b"")));broken[-1] ^= 1
    assert not p.feed(broken)
    assert p.bad_crc == 1
    assert not p.feed(encode(Packet(2, DRIVE, b"")))
    assert p.bad_sequence == 1
    assert p.feed(encode(Packet(3, STOP))) == [Packet(3, STOP)]
