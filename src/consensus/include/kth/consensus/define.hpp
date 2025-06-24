// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_CONSENSUS_DEFINE_HPP
#define KTH_CONSENSUS_DEFINE_HPP

// See http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _MSC_VER || defined __CYGWIN__
    #define BCK_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BCK_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BCK_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define BCK_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define BCK_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define BCK_HELPER_DLL_LOCAL  __attribute__ ((visibility ("internal")))
    #else
        #define BCK_HELPER_DLL_IMPORT
        #define BCK_HELPER_DLL_EXPORT
        #define BCK_HELPER_DLL_LOCAL
    #endif
#endif

// Now we use the generic helper definitions above to
// define BCK_API and BCK_INTERNAL.
// BCK_API is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)
// BCK_INTERNAL is used for non-api symbols.

#if defined BCK_STATIC
    #define BCK_API
    #define BCK_INTERNAL
#elif defined BCK_DLL
    #define BCK_API      BCK_HELPER_DLL_EXPORT
    #define BCK_INTERNAL BCK_HELPER_DLL_LOCAL
#else
    #define BCK_API      BCK_HELPER_DLL_IMPORT
    #define BCK_INTERNAL BCK_HELPER_DLL_LOCAL
#endif

#endif
