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

./screen-worms-client localhost -n AmazingName -p 2137 -i notlocalhost -r 69696

echo -e "Got $?"

exit 0
