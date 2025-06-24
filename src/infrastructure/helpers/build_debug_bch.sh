#!/bin/bash

mkdir -p build_bch
cd build_bch

conan install .. -s build_type=Debug -o infrastructure:tests=True
cmake .. -GNinja -DUSE_CONAN=ON -DCMAKE_VERBOSE_MAKEFILE=ON -DLOG_LIBRARY="boost" -DWITH_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
ninja -j4
# cmake --build .


# conan install .. -s build_type=Debug -o infrastructure:tests=True
# cmake .. -DUSE_CONAN=ON -DCMAKE_VERBOSE_MAKEFILE=ON -DLOG_LIBRARY="boost" -DWITH_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
# # make
