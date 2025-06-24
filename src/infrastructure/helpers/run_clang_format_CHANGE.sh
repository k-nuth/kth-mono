#!/bin/bash
# Copyright (c) 2016-2023 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

hpps=$(find include -name \*\.hpp)
cpps=$(find src -name \*\.cpp)

for f in ${hpps}
do
  clang-format -i -style=file ${f}
done

for f in ${cpps}
do
  clang-format -i  -style=file ${f}
done

