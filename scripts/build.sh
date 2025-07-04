#!/bin/bash
set -x

if [ -z "$1" ]; then
    echo "Usage: $0 <version>"
    exit 1
fi
VERSION="$1"

echo "Building version: ${VERSION}"

rm -rf build
rm -rf conan.lock

conan lock create conanfile.py --version="${VERSION}" --update
conan lock create conanfile.py --version "${VERSION}" --lockfile=conan.lock --lockfile-out=build/conan.lock
conan install conanfile.py --lockfile=build/conan.lock -of build --build=missing

cmake --preset conan-release \
         -DCMAKE_VERBOSE_MAKEFILE=ON \
         -DGLOBAL_BUILD=ON \
         -DENABLE_TEST=ON \
         -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

cmake --build --preset conan-release -j4

if [ $? -ne 0 ]; then
    echo "CMake build failed"
    exit 1
fi

# Run tests after build
echo "Running tests..."
cd build/build/Release

# Run tests with ctest (CTestTestfile.cmake is now generated automatically by CMake)
ctest --output-on-failure --parallel 4
