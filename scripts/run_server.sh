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

./screen-worms-server -p "$1" -s 1000 -v 10 -w 1000 -h 800

echo -e "Got $?"

exit 0
