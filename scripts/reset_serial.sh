#!/bin/sh
set -eu
# Software restart only. NEVER power-cycle motor supply through USB.
sudo systemctl stop aether.service
sudo udevadm trigger --subsystem-match=tty
sudo udevadm settle
sudo systemctl start aether.service
