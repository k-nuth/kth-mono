// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;
using namespace kth::infrastructure::config;

#define BASE58_ENCODED_A "vYxp6yFC7qiVtK1RcGQQt3L6EqTc8YhEDLnSMLqDvp8D"
#define BASE58_DECODED_A \
{{ \
    0x03, 0x1b, 0xab, 0x84, 0xe6, 0x87, 0xe3, 0x65, 0x14, 0xee, 0xaf, 0x5a, \
    0x01, 0x7c, 0x30, 0xd3, 0x2c, 0x1f, 0x59, 0xdd, 0x4e, 0xa6, 0x62, 0x9d, \
    0xa7, 0x97, 0x0c, 0xa3, 0x74, 0x51, 0x3d, 0xd0,  0x06 \
}}

// Start Test Suite: base58 tests

TEST_CASE("base58  constructor  default  does not throw", "[base58 tests]") {
    REQUIRE_NOTHROW(base58());
}

TEST_CASE("base58  constructor  valid string cast  decodes", "[base58 tests]") {
    data_chunk const original(BASE58_DECODED_A);
    data_chunk const instance(base58(BASE58_ENCODED_A));
    REQUIRE(original == instance);
}

////TEST_CASE("base58  constructor  bogus string  throws invalid option", "[base58 tests]")
////{
////    //BX_REQUIRE_THROW_INVALID_OPTION_VALUE(base58("bo-gus"));
////}

////TEST_CASE("base58  constructor  copy address primitives  round trips", "[base58 tests]")
////{
////    //BX_SERIALIZE_COPY_ROUND_TRIP(base58, BASE58_ENCODED_A);
////}

// End Test Suite
