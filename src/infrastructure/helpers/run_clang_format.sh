#!/bin/bash
# Copyright (c) 2016-2023 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

hpps=$(find include -name \*\.hpp)
cpps=$(find src -name \*\.cpp)

for f in ${hpps}
do
#   echo $f
  diff -u <(cat ${f}) <(clang-format -style=file ${f})
done

for f in ${cpps}
do
#   echo $f
  diff -u <(cat ${f}) <(clang-format -style=file ${f})
done



# diff -u <(cat src/math/secp256k1_initializer.cpp) <(clang-format -style=file src/math/secp256k1_initializer.cpp)


# diff -u <(cat ${cpps}) <(clang-format ${cpps})
# diff -u <(cat src/*) <(clang-format src/*)
# diff -u <(cat include/*) <(clang-format include/*)

