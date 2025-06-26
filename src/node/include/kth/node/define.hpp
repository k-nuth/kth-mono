// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_NODE_DEFINE_HPP
#define KTH_NODE_DEFINE_HPP

#include <kth/domain.hpp>

// We use the generic helper definitions to define BCN_API
// and BCN_INTERNAL. BCN_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BCN_INTERNAL is
// used for non-api symbols.

#if defined BCN_STATIC
    #define BCN_API
    #define BCN_INTERNAL
#elif defined BCN_DLL
    #define BCN_API      BC_HELPER_DLL_EXPORT
    #define BCN_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BCN_API      BC_HELPER_DLL_IMPORT
    #define BCN_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Log name.
#define LOG_NODE "[node] "

#endif
