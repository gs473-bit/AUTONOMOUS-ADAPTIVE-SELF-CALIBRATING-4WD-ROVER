#!/bin/sh
set -eu
# CAUTION: only physical E-stop cuts motor supply; check that first.
sudo systemctl stop aether.service
sudo shutdown -h now
