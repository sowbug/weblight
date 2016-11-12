#!/bin/bash

while true; do

  echo "Press a key..."
  read -n 1 -s
  
  echo -n "Updating firmware..."
  make upload
  sleep 1
  echo "done"
  
  echo -n "Setting origins..."
  wlctl --urls https://sowbug.github.io/weblight https://sowbug.github.io http://localhost:8000
  echo "done"

  echo -n "Updating sequence..."
  wlctl 000100 000000
  wlctl --save
  echo "done"

  echo -n "Regenerating serial number..."
  wlctl --regenerate-serial
  echo "done"
  
  echo "OK to unplug!"
  echo
  echo

done
