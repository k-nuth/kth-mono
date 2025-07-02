# Copyright (c) 2016-2025 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.


function(_check_has_64bit_asm)
    if (DEFINED HAS_64BIT_ASM)
        return()
    endif()

    set(_filename "${CMAKE_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/_has_64bit_asm.c")
    file(WRITE ${_filename}
            "#include <stdint.h>
    int main() {
      uint64_t a = 11, tmp;
      __asm__ __volatile__(\"movq 0x100000000,%1; mulq %%rsi\" : \"+a\"(a) : \"S\"(tmp) : \"cc\", \"%rdx\");
    }")
    try_compile(HAS_64BIT_ASM "${CMAKE_BINARY_DIR}" ${_filename})
    set(HAS_64BIT_ASM ${HAS_64BIT_ASM} PARENT_SCOPE)
    file(REMOVE ${_filename})
endfunction()

function(_check_has_int128)
    if (DEFINED HAS_INT128)
        return()
    endif()

    set(_filename "${CMAKE_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/_has_int128_test.c")
    file(WRITE ${_filename}
            "int main() { __int128 x = 0; }")
    try_compile(HAS_INT128 "${CMAKE_BINARY_DIR}" ${_filename})
    set(HAS_INT128 ${HAS_INT128} PARENT_SCOPE)
    file(REMOVE ${_filename})
endfunction()

function(_check_has_gmp)
    if (DEFINED HAS_GMP)
        return()
    endif()

    find_package(GMP)
    set(HAS_GMP ${GMP_FOUND} PARENT_SCOPE)
endfunction()

