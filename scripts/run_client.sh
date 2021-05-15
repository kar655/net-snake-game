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

#HOST=students.mimuw.edu.pl
HOST=localhost

./screen-worms-client $HOST -n AmazingName -p 1111 -i $HOST -r "$1"

echo -e "Got $?"

exit 0
