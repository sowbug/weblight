#!/bin/bash

# This script is meant to be hooked up via udev to a workstation designed for
# mass programming. When the workstation is configured this way, it'll program
# any device that gets plugged in.
#
# /etc/udev/rules.d/50-weblight-mass-program.rules
#
# ATTRS{idVendor}=="1209", ATTRS{idProduct}=="a800", RUN+="/work/src/weblight/firmware/tools/mp.sh"
# 
# TODO(miket): Deal with many devices simultaneously plugged in.
#
# TODO(miket): THIS DOESN'T WORK because the firmware update causes a
# reinsertion event.

pushd /work/src/weblight/firmware
make upload
popd
sleep 1
  
wlctl --urls https://sowbug.github.io/weblight \
  https://sowbug.github.io http://localhost:8000

wlctl 000100 000000
wlctl --save

wlctl --regenerate-serial
