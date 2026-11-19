"""Mirror of firmware/stm32/src/protocol/packet.c; mock/desktop tests only."""
import binascii
import struct
from dataclasses import dataclass

SOF=b"\xA5\x5A"
MAX_PAYLOAD=128
DRIVE,STOP,ARM,DISARM,SET_PID,SET_MODE,STATUS_REQ,HEARTBEAT,CLEAR_ESTOP,FAULT_REQ = range(1,11)
TELEMETRY,ACK,NACK,FAULT_LOG=0x81,0x82,0x83,0x84
@dataclass(frozen=True)
class Packet:
    sequence: int
    type: int
    payload: bytes=b""

def encode(packet: Packet)->bytes:
    if not 0<=packet.sequence<65536 or not 0<=packet.type<256 or len(packet.payload)>MAX_PAYLOAD:
        raise ValueError("invalid packet")
    frame=struct.pack("<HHB",len(packet.payload),packet.sequence,packet.type)+packet.payload
    return SOF+frame+struct.pack("<H",binascii.crc_hqx(frame,0xFFFF))

class Parser:
    def __init__(self):
        self.buf=bytearray()
        self.next_sequence=None
        self.bad_crc=0
        self.bad_length=0
        self.bad_sequence=0
    def feed(self,data:bytes)->list[Packet]:
        self.buf.extend(data)
        out=[]
        while True:
            idx=self.buf.find(SOF)
            if idx<0:
                self.buf[:]=b"\xA5" if self.buf[-1:]==b"\xA5" else b""
                break
            del self.buf[:idx]
            if len(self.buf)<7:break
            n,seq,typ=struct.unpack("<HHB",self.buf[2:7])
            if n>MAX_PAYLOAD:
                self.bad_length+=1
                del self.buf[0]
                continue
            if len(self.buf)<9+n:break
            payload=bytes(self.buf[7:7+n])
            crc=struct.unpack("<H",self.buf[7+n:9+n])[0]
            actual=binascii.crc_hqx(bytes(self.buf[2:7+n]),0xFFFF)
            if crc!=actual:
                self.bad_crc+=1
                del self.buf[0]
                continue
            del self.buf[:9+n]
            if self.next_sequence is not None and seq!=self.next_sequence:
                self.bad_sequence+=1
                if typ not in (STOP,DISARM):
                    continue
            self.next_sequence=(seq+1)&65535
            out.append(Packet(seq,typ,payload))
        return out
