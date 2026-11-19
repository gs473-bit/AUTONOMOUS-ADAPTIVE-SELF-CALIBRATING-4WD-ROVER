#!/bin/sh
set -eu
sudo systemctl restart aether.service
sudo systemctl --no-pager status aether.service
