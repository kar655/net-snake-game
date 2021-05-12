#!/bin/bash

if [[ ! -d build ]]
then
  mkdir build
  cd build || exit 1
  cmake ..
  make
else
  cd build || exit 1
  make
fi

./screen-worms-client localhost -n AmazingName -p 1111 -i localhost -r 2137

echo -e "Got $?"

exit 0
