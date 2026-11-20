"""AETHER Pi VCP heartbeat and optional loopback smartphone service."""
import logging
import os
import signal
import threading
from rover.comms.transport import HeartbeatLink, SerialTransport
from rover.phone_control import from_environment, make_server

def main():
    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
    stop = threading.Event()
    def request_stop(signum, frame):
        del signum, frame
        stop.set()
    signal.signal(signal.SIGTERM, request_stop)
    signal.signal(signal.SIGINT, request_stop)
    link = HeartbeatLink(SerialTransport())
    web = None
    web_thread = None
    token = os.environ.get("AETHER_PHONE_TOKEN")
    if token:
        controller = from_environment(link)
        # Never bind directly to LAN/Internet; use HTTPS reverse proxy/VPN to phone.
        web = make_server(controller, token, port=int(os.getenv("AETHER_PHONE_PORT", "8080")))
        web_thread = threading.Thread(target=web.serve_forever, daemon=True, name="aether-phone-ui")
        web_thread.start()
        logging.info("Phone UI ready on loopback port %s; ARM enabled: %s",
                     web.server_address[1], controller.motion_enabled)
    else:
        logging.info("Phone UI disabled: configure AETHER_PHONE_TOKEN")
    link.start()
    logging.info("STM32 heartbeat started; motor actuation disabled until hardware signoff")
    try:
        while not stop.wait(1.0):
            if link.transport.port is None:
                logging.warning("STM32 USB serial disconnected; waiting for reconnect")
    finally:
        if web is not None:
            web.shutdown()
            web.server_close()
            if web_thread:
                web_thread.join(timeout=2)
        try:
            link.disarm()
        except (OSError, ConnectionError):
            pass
        link.close()
        logging.info("AETHER service stopped")

if __name__ == "__main__":
    main()
