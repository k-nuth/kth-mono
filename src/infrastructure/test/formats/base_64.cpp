// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

#include <kth/infrastructure.hpp>

using namespace kth;

// Start Test Suite: base 64 tests

#define BASE64_MURRAY "TXVycmF5IFJvdGhiYXJk"
#define BASE64_DATA_MURRAY \
{ \
    0x4d, 0x75, 0x72, 0x72, 0x61, 0x79, 0x20, \
    0x52, 0x6f, 0x74, 0x68, 0x62, 0x61, 0x72, 0x64 \
}

#define BASE64_BOOK "TWFuLCBFY29ub215IGFuZCBTdGF0ZQ=="
#define BASE64_DATA_BOOK \
{ \
    0x4d, 0x61, 0x6e, 0x2c, 0x20, 0x45, 0x63, \
    0x6f, 0x6e, 0x6f, 0x6d, 0x79, 0x20, 0x61, \
    0x6e, 0x64, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65 \
}

TEST_CASE("encode base64 empty test", "[base 64 tests]") {
    data_chunk decoded;
    REQUIRE(encode_base64(decoded) == "");
}

TEST_CASE("decode base64 empty test", "[base 64 tests]") {
    data_chunk result;
    REQUIRE(decode_base64(result, ""));
    REQUIRE(result == data_chunk());
}

TEST_CASE("encode base64 test", "[base 64 tests]") {
    data_chunk decoded(BASE64_DATA_MURRAY);
    REQUIRE(encode_base64(decoded) == BASE64_MURRAY);
}

TEST_CASE("decode base64 valid test", "[base 64 tests]") {
    data_chunk result;
    REQUIRE(decode_base64(result, BASE64_MURRAY));
    REQUIRE(result == data_chunk(BASE64_DATA_MURRAY));
}

TEST_CASE("encode base64 padded test", "[base 64 tests]") {
    data_chunk decoded(BASE64_DATA_BOOK);
    REQUIRE(encode_base64(decoded) == BASE64_BOOK);
}

TEST_CASE("decode base64 padded valid test", "[base 64 tests]") {
    data_chunk result;
    REQUIRE(decode_base64(result, BASE64_BOOK));
    REQUIRE(result == data_chunk(BASE64_DATA_BOOK));
}

TEST_CASE("decode base64 invalid test", "[base 64 tests]") {
    data_chunk result;
    REQUIRE( ! decode_base64(result, "!@#$%^&*()"));
}

// End Test Suite
