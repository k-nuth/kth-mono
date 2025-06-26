// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ASSERT_HPP
#define KTH_INFRASTRUCTURE_ASSERT_HPP

#ifdef NDEBUG
    #define KTH_ASSERT(expression)
    #define KTH_ASSERT_MSG(expression, text)
    #define DEBUG_ONLY(expression)
#else
    #include <cassert>
    #define KTH_ASSERT(expression) assert(expression)
    #define KTH_ASSERT_MSG(expression, text) assert((expression)&&(text))
    #define DEBUG_ONLY(expression) expression
#endif

// This is used to prevent bogus compiler warnings about unused variables.
#define UNUSED(expression) (void)(expression)

#endif
