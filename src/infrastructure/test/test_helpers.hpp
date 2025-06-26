// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// https://github.com/catchorg/Catch2/blob/master/docs/tostring.md

#ifndef KTH_INFRASTRUCTURE_TEST_HELPERS_HPP
#define KTH_INFRASTRUCTURE_TEST_HELPERS_HPP

#include <ostream>

#include <kth/infrastructure/hash_define.hpp>
#include <kth/infrastructure/formats/base_16.hpp>

namespace kth {

inline
std::ostream& operator<<(std::ostream& os, hash_digest const& x) {
    os << encode_hash(x);
    return os;
}

} // namespace kth

#include <catch2/catch_test_macros.hpp>

#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK(cond); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

// #include <kth/infrastructure.hpp>

#endif // KTH_INFRASTRUCTURE_TEST_HELPERS_HPP
