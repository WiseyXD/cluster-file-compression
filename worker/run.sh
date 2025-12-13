#!/bin/bash
set -e

mkdir -p build
cd build
cmake ..
make

echo "---- Running binary ----"
./app

