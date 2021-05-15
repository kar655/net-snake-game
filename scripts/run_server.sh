#!/bin/bash

if [[ ! -d build ]]; then
  mkdir build
  cd build || exit 1
  cmake ..
  make
else
  cd build || exit 1
  make
fi

./screen-worms-server

echo -e "Got $?"

exit 0
