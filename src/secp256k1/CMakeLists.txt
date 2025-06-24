# Copyright (c) 2016-2023 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

cmake_minimum_required(VERSION 3.15)

project(secp256k1 VERSION 0 LANGUAGES C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_SHARED "" OFF)
option(ENABLE_POSITION_INDEPENDENT_CODE "Enable POSITION_INDEPENDENT_CODE property" ON)

option(ENABLE_BENCHMARK "Compile benchmark." OFF)
option(ENABLE_TESTS "Compile tests." ON)
option(ENABLE_OPENSSL_TESTS "Enable OpenSSL tests, if OpenSSL is available" OFF)

option(ENABLE_EXPERIMENTAL "Alloc experimental configure options" OFF)
option(ENABLE_ENDOMORPHISM "Enable endomorphism" OFF)
option(ENABLE_ECMULT_STATIC_PRECOMPUTATION "Enable precomputed ecmult table for signing" ON)
option(ENABLE_MODULE_ECDH "Enable ECDH shared secret computation (experimental)." OFF)
option(ENABLE_MODULE_SCHNORR "Enable Schnorr signature module (experimental)." ON)
option(ENABLE_MODULE_RECOVERY "Enable ECDSA pubkey recovery module." ON)
option(ENABLE_MODULE_MULTISET "Enable multiset operations (experimental)." ON)


# Make the emult window size customizable.
set(SECP256K1_ECMULT_WINDOW_SIZE 15 CACHE STRING "Window size for ecmult precomputation for verification, specified as integer in range [2..24].")
if(${SECP256K1_ECMULT_WINDOW_SIZE} LESS 2 OR ${SECP256K1_ECMULT_WINDOW_SIZE} GREATER 24)
	message(FATAL_ERROR "SECP256K1_ECMULT_WINDOW_SIZE must be an integer in range [2..24]")
endif()
add_definitions(-DECMULT_WINDOW_SIZE=${SECP256K1_ECMULT_WINDOW_SIZE})

set(SECP256K1_ECMULT_GEN_PRECISION 4 CACHE STRING "Precision bits to tune the precomputed table size for signing.")
set(VALID_PRECISIONS 2 4 8)
if(NOT ${SECP256K1_ECMULT_GEN_PRECISION} IN_LIST VALID_PRECISIONS)
	message(FATAL_ERROR "SECP256K1_ECMULT_GEN_PRECISION not 2, 4, 8")
endif()
add_definitions(-DECMULT_GEN_PREC_BITS=${SECP256K1_ECMULT_GEN_PRECISION})

set(WITH_ASM "auto" CACHE STRING "Specify assembly optimizations to use (x86_64|arm|no|auto).")
set(WITH_FIELD "auto" CACHE STRING "Specify Field Implementation (64bit|32bit|auto).")
set(WITH_SCALAR "auto" CACHE STRING "Specify scalar implementation (64bit|32bit|auto).")
set(WITH_BIGNUM "auto" CACHE STRING "Specify Bignum Implementation (gmp|mpir|no|auto).")

set(MARCH_ID "" CACHE STRING "Specify the Microarchitecture ID (x86_64|...).")
message( STATUS "Knuth: Compiling for Microarchitecture ID ${MARCH_ID}")

message( STATUS "Knuth: CONAN_CXX_FLAGS ${CONAN_CXX_FLAGS}")
message( STATUS "Knuth: CONAN_C_FLAGS ${CONAN_C_FLAGS}")

find_package(gmp REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/ci_utils/cmake)

include(Secp256k1Tools)


#------------------------------------------------------------------------------
if (ENABLE_ENDOMORPHISM)
  add_definitions(-DUSE_ENDOMORPHISM)
endif()

if (ENABLE_ECMULT_STATIC_PRECOMPUTATION)
  # add_definitions(-DUSE_ECMULT_STATIC_PRECOMPUTATION)

	set(USE_ECMULT_STATIC_PRECOMPUTATION 1)

  if(EXISTS ${CMAKE_SOURCE_DIR}/ci_utils/cmake/NativeExecutable.cmake)
    include(${CMAKE_SOURCE_DIR}/ci_utils/cmake/NativeExecutable.cmake)
  else()
    message( STATUS "NativeExecutable.cmake doent exists")
  endif()

	# # include(NativeExecutable)
	add_native_executable(gen_context src/gen_context.c)

  target_include_directories(gen_context PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  # target_include_directories(gen_context PUBLIC
  # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  # $<INSTALL_INTERFACE:include>)

	add_custom_command(
		OUTPUT ecmult_static_context.h
		COMMAND gen_context
	)

	# target_sources(${PROJECT_NAME} PRIVATE ecmult_static_context.h)

endif()

# Implement --with-asm
#------------------------------------------------------------------------------
if (WITH_ASM STREQUAL "auto")
  _check_has_64bit_asm()
  if (HAS_64BIT_ASM)
    set(WITH_ASM "x86_64")
  else()
    set(WITH_ASM "no")
  endif()
elseif (WITH_ASM STREQUAL "x86_64")
  _check_has_64bit_asm()
  if (NOT HAS_64BIT_ASM)
    message(FATAL_ERROR "x86_64 assembly optimization requested but not available")
  endif()
elseif (NOT WITH_ASM STREQUAL "arm" AND NOT WITH_ASM STREQUAL "no")
  message(FATAL_ERROR "Invalid assembly optimization selection: ${WITH_ASM}")
endif()

# Implement --with-field
#------------------------------------------------------------------------------
if (WITH_FIELD STREQUAL "auto")
  if (WITH_ASM STREQUAL "x86_64")
    set (WITH_FIELD "64bit")
  else()
    _check_has_int128()
    if (HAS_INT128)
      set(WITH_FIELD "64bit")
    else()
      set(WITH_FIELD "32bit")
    endif()
  endif()
elseif (WITH_FIELD STREQUAL "64bit")
  if (NOT WITH_ASM STREQUAL "x86_64")
    _check_has_int128()
    if (NOT HAS_INT128)
      message(FATAL_ERROR "64bit field explicitly requested but neither __int128 support or x86_64 assembly available")
    endif()
  endif()
elseif (NOT WITH_FIELD STREQUAL "32bit")
  message(FATAL_ERROR "Invalid field implementation: ${WITH_FIELD}")
endif()

# Implement --with-scalar
#------------------------------------------------------------------------------
if (WITH_SCALAR STREQUAL "auto")
  _check_has_int128()
  if (HAS_INT128)
    set(WITH_SCALAR "64bit")
  else()
    set(WITH_SCALAR "32bit")
  endif()
elseif (WITH_FIELD STREQUAL "64bit")
  _check_has_int128()
  if (NOT HAS_INT128)
    message(FATAL_ERROR "64bit scalar explicitly requested but __int128 support not available")
  endif()
elseif (NOT WITH_FIELD STREQUAL "32bit")
  message(FATAL_ERROR "Invalid scalar implementation: ${WITH_SCALAR}")
endif()

if (ENABLE_SHARED)
  set(MODE SHARED)
else()
  set(MODE STATIC)
endif()


# Build
#==============================================================================
add_library(${PROJECT_NAME} ${MODE} include/secp256k1.h   src/secp256k1.c)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (ENABLE_POSITION_INDEPENDENT_CODE)
  set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(ENABLE_POSITION_INDEPENDENT_CODE)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

_check_has_int128()
if (HAS_INT128)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE___INT128=1)
endif()

if (WITH_ASM STREQUAL "x86_64")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_ASM_X86_64=1)
elseif (WITH_FIELD STREQUAL "arm")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_EXTERNAL_ASM=1)
  target_sources(${PROJECT_NAME} PRIVATE "src/asm/field_10x26_arm.s")
endif()

if (WITH_FIELD STREQUAL "64bit")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_FIELD_5X52=1)
elseif (WITH_FIELD STREQUAL "32bit")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_FIELD_10X26=1)
endif()

if (WITH_SCALAR STREQUAL "64bit")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_SCALAR_4X64=1)
elseif (WITH_SCALAR STREQUAL "32bit")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_SCALAR_8X32=1)
endif()

if (${WITH_BIGNUM} STREQUAL "gmp")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_LIBGMP=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_NUM_GMP=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_FIELD_INV_NUM=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_SCALAR_INV_NUM=1)
elseif (${WITH_BIGNUM} STREQUAL "mpir")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_LIBGMP=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_NUM_GMP=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_FIELD_INV_NUM=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_SCALAR_INV_NUM=1)
elseif (${WITH_BIGNUM} STREQUAL "no")
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_NUM_NONE=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_FIELD_INV_BUILTIN=1)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_SCALAR_INV_BUILTIN=1)
endif()

if (ENABLE_ECMULT_STATIC_PRECOMPUTATION)
	target_sources(${PROJECT_NAME} PRIVATE ecmult_static_context.h)
endif()

target_link_libraries(${PROJECT_NAME} gmp::gmp)

if (ENABLE_MODULE_ECDH)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DENABLE_MODULE_ECDH=1)
endif()

if (ENABLE_MODULE_SCHNORR)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DENABLE_MODULE_SCHNORR=1)
endif()

if (ENABLE_MODULE_RECOVERY)
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DENABLE_MODULE_RECOVERY=1)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "secp256k1")

# Benchmark
#==============================================================================
if (ENABLE_BENCHMARK)
  add_executable(bench_verify src/bench_verify.c)
  target_link_libraries(bench_verify ${PROJECT_NAME})
  set_target_properties(bench_verify PROPERTIES FOLDER "secp256k1")

  if (ENABLE_OPENSSL_TESTS)
    target_compile_definitions(bench_verify PRIVATE -DENABLE_OPENSSL_TESTS)
  endif()



  add_executable(bench_sign src/bench_sign.c)
  target_link_libraries(bench_sign ${PROJECT_NAME})
  set_target_properties(bench_sign PROPERTIES FOLDER "secp256k1")

  add_executable(bench_internal src/bench_internal.c)
  target_link_libraries(bench_internal ${PROJECT_NAME})
  set_target_properties(bench_internal PROPERTIES FOLDER "secp256k1")

  if (ENABLE_MODULE_ECDH)
    add_executable(bench_ecdh src/bench_ecdh.c)
    target_link_libraries(bench_ecdh ${PROJECT_NAME})
    set_target_properties(bench_ecdh PROPERTIES FOLDER "secp256k1")
  endif()

  if (ENABLE_MODULE_SCHNORR)
    add_executable(bench_schnorr_verify src/bench_schnorr_verify.c)
    target_link_libraries(bench_schnorr_verify ${PROJECT_NAME})
    set_target_properties(bench_schnorr_verify PROPERTIES FOLDER "secp256k1")
  endif()

  if (ENABLE_MODULE_RECOVERY)
    add_executable(bench_recover src/bench_recover.c)
    target_link_libraries(bench_recover ${PROJECT_NAME})
    set_target_properties(bench_recover PROPERTIES FOLDER "secp256k1")
  endif()

  if (ENABLE_MODULE_MULTISET)
    add_executable(bench_multiset src/bench_multiset.c)
    target_link_libraries(bench_multiset ${PROJECT_NAME})
    set_target_properties(bench_multiset PROPERTIES FOLDER "secp256k1")
  endif()
endif()

# Tests
#==============================================================================
if (ENABLE_TESTS)
  add_executable(tests
    src/tests.c)

  if (ENABLE_OPENSSL_TESTS)
    target_compile_definitions(tests PRIVATE -DENABLE_OPENSSL_TESTS)
  endif()

  target_compile_definitions(tests PRIVATE -DVERIFY)
  target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

  target_link_libraries(tests ${PROJECT_NAME})

  enable_testing()
  add_test(
    NAME test
    COMMAND tests)

  set_target_properties(tests PROPERTIES FOLDER "secp256k1")
endif()



# Install
# ------------------------------------------------------------------------------

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Generate and install a CMake package configuration file
install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-config.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
