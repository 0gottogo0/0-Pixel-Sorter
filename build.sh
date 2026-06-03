#!/usr/bin/env bash

set -e

rm -rf build

mkdir -p build
cd build

cmake ..
cmake --build .