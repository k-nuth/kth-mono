// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_DEFINE_HPP
#define KTH_BLOCKCHAIN_DEFINE_HPP

#include <kth/domain.hpp>

// Now we use the generic helper definitions to
// define BCB_API and BCB_INTERNAL.
// BCB_API is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)
// BCB_INTERNAL is used for non-api symbols.

#if defined BCB_STATIC
    #define BCB_API
    #define BCB_INTERNAL
#elif defined BCB_DLL
    #define BCB_API      BC_HELPER_DLL_EXPORT
    #define BCB_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BCB_API      BC_HELPER_DLL_IMPORT
    #define BCB_INTERNAL BC_HELPER_DLL_LOCAL
#endif

//// Now we use the generic helper definitions to
//// define BCD_API and BCD_INTERNAL.
//// BCD_API is used for the public API symbols. It either DLL imports or
//// DLL exports (or does nothing for static build)
//// BCD_INTERNAL is used for non-api symbols.
//
//#if defined BCB_STATIC
//    #define BCD_API
//    #define BCD_INTERNAL
//#elif defined BCB_DLL
//    #define BCD_API      BC_HELPER_DLL_EXPORT
//    #define BCD_INTERNAL BC_HELPER_DLL_LOCAL
//#else
//    #define BCD_API      BC_HELPER_DLL_IMPORT
//    #define BCD_INTERNAL BC_HELPER_DLL_LOCAL
//#endif

// Log name.
#define LOG_BLOCKCHAIN "[blockchain] "

#endif
