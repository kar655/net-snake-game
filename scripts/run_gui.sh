#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Usage $0 port"
  exit 1
fi

cd gui || exit 1
make

./gui2 "$1"

echo -e "Got $?"

exit 0
