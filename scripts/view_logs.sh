#!/bin/sh
set -eu
sudo journalctl -u aether.service --no-pager -n 100
