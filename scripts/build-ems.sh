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

conan lock create conanfile.py --version="${VERSION}" --update -pr ems2
conan lock create conanfile.py --version "${VERSION}" --lockfile=conan.lock --lockfile-out=build/conan.lock -pr ems2
conan install conanfile.py --lockfile=build/conan.lock -of build --build=missing -pr ems2

cmake --preset conan-release \
         -DCMAKE_VERBOSE_MAKEFILE=ON \
         -DGLOBAL_BUILD=ON \
         -DCMAKE_BUILD_TYPE=Release

cmake --build --preset conan-release -j8 \
         -DCMAKE_VERBOSE_MAKEFILE=ON \
         -DGLOBAL_BUILD=ON \
         -DCMAKE_BUILD_TYPE=Release
