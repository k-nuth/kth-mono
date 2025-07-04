// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/capi/libconfig/libconfig.h>

#include <kth/capi/node/settings.h>
#include <kth/capi/version.h>

#define KTH_QUOTE(name) #name
#define KTH_STR(macro) KTH_QUOTE(macro)

extern "C" {

kth_libconfig_t kth_libconfig_get() {
    kth_libconfig_t res;

#if defined(KTH_LOG_LIBRARY_BOOST)
    res.log_library = kth_libconfig_log_library_boost;
#elif defined(KTH_LOG_LIBRARY_SPDLOG)
    res.log_library = kth_libconfig_log_library_spdlog;
#elif defined(KTH_LOG_LIBRARY_BINLOG)
    res.log_library = kth_libconfig_log_library_binlog;
#endif

    res.version = KTH_CAPI_VERSION;
    res.microarchitecture_id = KTH_STR(KTH_MARCH_ID);

    res.currency = kth_node_settings_get_currency();

#if defined(KTH_WITH_MEMPOOL)
    res.mempool = 1;
#else
    res.mempool = 0;
#endif

// #if defined(KTH_DB_NEW_FULL)
//     res.db_mode = kth_libconfig_db_mode_full_indexed;
// #elif defined(KTH_DB_NEW_BLOCKS)
//     res.db_mode = kth_libconfig_db_mode_normal;
// #elif defined(KTH_DB_NEW)
//     res.db_mode = kth_libconfig_db_mode_pruned;
// #endif

#if defined(KTH_DB_READONLY)
    res.db_readonly = 1;
#else
    res.db_readonly = 0;
#endif

#if defined(DEBUG)
    res.debug_mode = 1;
#else
    res.debug_mode = 0;
#endif

    res.architecture =
#if defined(__EMSCRIPTEN__)
        "WASM";
#elif defined(__x86_64__)
        "x86_64";
#elif defined(__aarch64__)
        "ARM64";
#else
        "Unknown";
#endif

    res.os_name =
#if defined(__EMSCRIPTEN__)
        "WebAssembly Host";
#elif defined(_WIN32)
        "Windows";
#elif defined(__linux__)
        "Linux";
#elif defined(__APPLE__)
        "macOS";
#elif defined(__FreeBSD__)
        "FreeBSD";
#elif defined(__NetBSD__)
        "NetBSD";
#elif defined(__OpenBSD__)
        "OpenBSD";
#else
        "Unknown";
#endif

    res.compiler_name =
#if defined(__EMSCRIPTEN__)
        "Emscripten";
#elif defined(__GNUC__)
        "GCC";
#elif defined(__clang__)
        "Clang";
#elif defined(_MSC_VER)
        "MSVC";
#else
        "Unknown";
#endif

    res.compiler_version = __VERSION__;

    res.optimization_level =
#if defined(__OPTIMIZE__)
        "-O2";
#elif defined(__OPTIMIZE_SIZE__)
        "-Os";
#else
        "-O0";
#endif

#ifdef KTH_CAPI_BUILD_TIMESTAMP
    res.build_timestamp = uint32_t(KTH_CAPI_BUILD_TIMESTAMP);
#else
    res.build_timestamp = 0;
#endif

#ifdef KTH_CAPI_BUILD_GIT_HASH
    res.build_git_hash = KTH_CAPI_BUILD_GIT_HASH;
#else
    res.build_git_hash = "";
#endif

    res.endianness =
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        "Little-endian";
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        "Big-endian";
#else
        "Unknown";
#endif

    res.type_sizes.size_int = sizeof(int);
    res.type_sizes.size_long = sizeof(long);
    res.type_sizes.size_pointer = sizeof(void*);

    return res;
}

#undef KTH_QUOTE
#undef KTH_STR

} // extern "C"
