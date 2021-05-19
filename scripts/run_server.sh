#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Usage $0 port"
  exit 1
fi

if [[ ! -d build ]]; then
  mkdir build
  cd build || exit 1
  cmake ..
  make
else
  cd build || exit 1
  make
fi

./screen-worms-server -p "$1" -s 100 -v 1 -w 100 -h 80

echo -e "Got $?"

exit 0
