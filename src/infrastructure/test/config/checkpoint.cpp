// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>

#if ! defined(__EMSCRIPTEN__)
#include <boost/program_options.hpp>
#endif

#include <test_helpers.hpp>

#include <kth/infrastructure.hpp>
#include <kth/infrastructure/utility/collection.hpp>

using namespace kth;
using namespace kth::infrastructure::config;
#if ! defined(__EMSCRIPTEN__)
using namespace boost::program_options;
#endif

// Start Test Suite: checkpoint tests

#define CHECKPOINT_HASH_A "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"
#define CHECKPOINT_A CHECKPOINT_HASH_A ":0"
#define CHECKPOINT_B "0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d:11111"
#define CHECKPOINT_C "000000002dd5588a74784eaa7ab0507a18ad16a236e7b1ce69f00d7ddfb5d0a6:33333"
#define CHECKPOINT_ABC CHECKPOINT_A "\n" CHECKPOINT_B "\n" CHECKPOINT_C

// ------------------------------------------------------------------------- //

// Start Test Suite: checkpoint construct

TEST_CASE("checkpoint construct  default  null hash", "[checkpoint construct]") {
    checkpoint const check;
    REQUIRE(check.hash() == null_hash);
    REQUIRE(check.height() == 0u);
}

TEST_CASE("checkpoint construct  copy  expected", "[checkpoint construct]") {
    checkpoint const check1(CHECKPOINT_C);
    checkpoint const check2(check1);
    REQUIRE(check2.height() == check1.height());
    REQUIRE(encode_hash(check2.hash()) == encode_hash(check1.hash()));
}

TEST_CASE("checkpoint construct  string  expected", "[checkpoint construct]") {
    checkpoint const genesis(CHECKPOINT_B);
    REQUIRE(genesis.height() == 11111u);
    REQUIRE(genesis.to_string() == CHECKPOINT_B);
}

TEST_CASE("checkpoint construct  digest  expected", "[checkpoint construct]") {
    size_t const expected_height = 42;
    auto const expected_hash = CHECKPOINT_HASH_A;
    hash_digest digest;
    kth::decode_hash(digest, expected_hash);
    checkpoint const genesis(digest, expected_height);
    REQUIRE(genesis.height() == expected_height);
    REQUIRE(encode_hash(genesis.hash()) == expected_hash);
}


//TODO(fernando): fix these tests
// TEST_CASE("checkpoint construct  invalid height value  throws invalid option value", "[checkpoint construct]") {
//     // 2^64
//     // REQUIRE_THROWS_AS([](){checkpoint(CHECKPOINT_HASH_A ":18446744073709551616");}, invalid_option_value);

//     auto cp = checkpoint(CHECKPOINT_HASH_A ":18446744073709551616");
//     cp.height();
// }

// TEST_CASE("checkpoint construct  invalid height characters  throws invalid option value", "[checkpoint construct]") {
//     // 21 characters
//     REQUIRE_THROWS_AS([](){checkpoint(CHECKPOINT_HASH_A ":1000000000100000000001");}, invalid_option_value);
// }

// TEST_CASE("checkpoint construct  bogus height characters  throws invalid option value", "[checkpoint construct]") {
//     REQUIRE_THROWS_AS([](){checkpoint(CHECKPOINT_HASH_A ":xxx");}, invalid_option_value);
// }

// TEST_CASE("checkpoint construct  bogus line hash  throws invalid option value", "[checkpoint construct]") {
//     REQUIRE_THROWS_AS([](){checkpoint("bogus:42");}, invalid_option_value);
// }

// TEST_CASE("checkpoint construct  bogus hash  throws invalid option value", "[checkpoint construct]") {
//     REQUIRE_THROWS_AS([](){checkpoint("bogus", 42);}, invalid_option_value);
// }

// End Test Suite

// ------------------------------------------------------------------------- //

// Start Test Suite: checkpoint istream

TEST_CASE("checkpoint istream  empty  expected", "[checkpoint istream]") {
    checkpoint deserialized;
    std::stringstream serialized(CHECKPOINT_A);
    serialized >> deserialized;
    REQUIRE(deserialized.to_string() == CHECKPOINT_A);
}

// End Test Suite

// ------------------------------------------------------------------------- //

// Start Test Suite: checkpoint  ostream

static
checkpoint::list const test_checkpoints_list({
    checkpoint{CHECKPOINT_A},
    checkpoint{CHECKPOINT_B},
    checkpoint{CHECKPOINT_C}
});

TEST_CASE("checkpoint  ostream  empty  expected", "[checkpoint  ostream]") {
    std::stringstream serialized;
    serialized << checkpoint::list();
    REQUIRE(serialized.str() == "");
}

TEST_CASE("checkpoint  ostream  populated  expected", "[checkpoint  ostream]") {
    std::stringstream serialized;
    serialized << test_checkpoints_list;
    REQUIRE(serialized.str() == CHECKPOINT_ABC);
}

TEST_CASE("checkpoint  ostream  boost lexical cast  expected", "[checkpoint  ostream]") {
    auto const serialized = boost::lexical_cast<std::string>(test_checkpoints_list);
    REQUIRE(serialized == CHECKPOINT_ABC);
}

// End Test Suite

// End Test Suite
