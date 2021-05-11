#!/bin/bash

cd gui || exit 1
make

./gui2 2137

echo -e "Got $?"

exit 0
