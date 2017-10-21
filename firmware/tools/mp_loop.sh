#!/bin/bash

while true; do

  echo "Press a key..."
  read -n 1 -s

  echo -n "Updating firmware..."
  make upload
  sleep 2
  echo "done"

  echo -n "Setting origins..."
  ../commandline/wlctl --urls https://sowbug.github.io/weblight http://localhost:8000
  echo "done"

  echo -n "Updating sequence..."
  ../commandline/wlctl 000100 000000
  ../commandline/wlctl --save
  echo "done"

  echo -n "Regenerating serial number..."
  ../commandline/wlctl --regenerate-serial
  echo "done"

  echo "OK to unplug!"
  echo
  echo

done
