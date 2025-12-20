#!/bin/bash
set -e

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

echo "---- Running tests ----"
ctest --test-dir build --output-on-failure

echo "---- Running app ----"
./build/app

