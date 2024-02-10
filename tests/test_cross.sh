#!/usr/bin/bash
set -euo pipefail
shopt -s inherit_errexit

rm -rf build
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../tests/$2.cmake -DCMAKE_BUILD_TYPE=$1
cmake --build . -j
ctest
