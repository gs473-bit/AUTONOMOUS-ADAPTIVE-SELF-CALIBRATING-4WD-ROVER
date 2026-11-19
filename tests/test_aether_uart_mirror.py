import pytest
from rover.comms.protocol import Packet, encode, Parser, DRIVE

def test_frame_roundtrip():
    frame=encode(Packet(42,DRIVE,b"abc"))
    p=Parser()
    assert p.feed(frame[:4])==[]
    assert p.feed(frame[4:])==[Packet(42,DRIVE,b"abc")]

def test_corrupt_and_gap():
    p=Parser()
    assert p.feed(encode(Packet(0,DRIVE)))==[Packet(0,DRIVE)]
    corrupted=bytearray(encode(Packet(1,DRIVE)));corrupted[-1]^=1
    assert p.feed(corrupted)==[]
    assert p.bad_crc==1
    assert p.feed(encode(Packet(2,DRIVE)))==[]
    assert p.bad_sequence==1
