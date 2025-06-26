# Copyright (c) 2016-2025 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

cmake_minimum_required(VERSION 3.15)

add_subdirectory(secp256k1)
add_subdirectory(infrastructure)
add_subdirectory(domain)
add_subdirectory(consensus)
add_subdirectory(network)
add_subdirectory(database)
add_subdirectory(blockchain)
add_subdirectory(node)

if (BUILD_NODE_EXE)
    add_subdirectory(node-exe)
endif()

if (BUILD_C_API)
    add_subdirectory(c-api)
endif()