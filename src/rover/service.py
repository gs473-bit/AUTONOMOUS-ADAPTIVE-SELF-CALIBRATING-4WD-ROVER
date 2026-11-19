"""AETHER VCP heartbeat-only service; never sends ARM/DRIVE on startup."""
import logging
import signal
import threading
from rover.comms.transport import HeartbeatLink, SerialTransport

def main():
    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
    stop = threading.Event()
    def request_stop(signum, frame):
        del signum, frame
        stop.set()
    signal.signal(signal.SIGTERM, request_stop)
    signal.signal(signal.SIGINT, request_stop)
    link = HeartbeatLink(SerialTransport())
    link.start()
    logging.info("AETHER serial heartbeat service started: motor arming disabled by design")
    try:
        while not stop.wait(1.0):
            if link.transport.port is None:
                logging.warning("STM32 USB serial disconnected; retrying")
    finally:
        try:
            link.disarm()
        except (OSError, ConnectionError):
            pass
        link.close()
        logging.info("AETHER service stopped")
if __name__ == "__main__":
    main()
