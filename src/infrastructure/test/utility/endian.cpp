// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;

// Start Test Suite: endian tests

// Stream Tests

TEST_CASE("endian  from big endian stream unsafe  insufficient data  stream failure", "[endian tests]") {
    static data_chunk const data{ 0xFF };
    data_source stream(data);
    REQUIRE( ! stream.eof());

    auto const value = from_little_endian_stream_unsafe<uint64_t>(stream);
    REQUIRE( ! stream);
    REQUIRE(stream.eof());
}

TEST_CASE("endian  from big endian stream unsafe  eof  stream partial read", "[endian tests]") {
    static uint8_t const content = 0xFF;
    static auto const shift = (sizeof(uint32_t) - sizeof(uint8_t)) * kth::byte_bits;
    const uint32_t expected = static_cast<uint32_t>(content) << shift;
    std::stringstream stream;
    stream.put(content);
    REQUIRE( ! stream.eof());

    auto const value = from_big_endian_stream_unsafe<uint32_t>(stream);
    REQUIRE(value == expected);
    REQUIRE(stream.fail());
    REQUIRE(stream.eof());
}

TEST_CASE("endian  from big endian stream unsafe  valid  expected", "[endian tests]") {
    static uint64_t const expected = 4234531234u;
    std::stringstream stream;
    auto bytes = to_big_endian(expected);
    auto const action = [&stream](uint8_t& value) { stream.put(value); };
    std::for_each(bytes.begin(), bytes.end(), action);
    auto const value = from_big_endian_stream_unsafe<uint64_t>(stream);
    REQUIRE(stream);
    REQUIRE(value == expected);
}

TEST_CASE("endian  from little endian stream unsafe  insufficient data  stream failure", "[endian tests]") {
    static data_chunk const data{ 0xFF };
    data_source stream(data);
    REQUIRE( ! stream.eof());

    auto const value = from_little_endian_stream_unsafe<uint64_t>(stream);
    REQUIRE( ! stream);
    REQUIRE(stream.eof());
}

TEST_CASE("endian  from little endian stream unsafe  eof  stream partial read", "[endian tests]") {
    static uint8_t const content = 0xFF;
    auto const expected = static_cast<uint32_t>(content);
    std::stringstream stream;
    stream.put(content);
    REQUIRE( ! stream.eof());

    auto const value = from_little_endian_stream_unsafe<uint32_t>(stream);
    REQUIRE(value == expected);
    REQUIRE(stream.fail());
    REQUIRE(stream.eof());
}

TEST_CASE("endian  from little endian stream unsafe  valid  expected", "[endian tests]") {
    static uint64_t const expected = 4234531234u;
    std::stringstream stream;
    auto bytes = to_little_endian(expected);
    auto const action = [&stream](uint8_t& value) { stream.put(value); };
    std::for_each(bytes.begin(), bytes.end(), action);
    auto const value = from_little_endian_stream_unsafe<uint64_t>(stream);
    REQUIRE(stream);
    REQUIRE(value == expected);
}


// Non-Stream Tests

TEST_CASE("endian  from little endian unsafe  one byte  expected", "[endian tests]") {
    static uint8_t const expected = 0xff;
    static auto const bytes = data_chunk{ expected };
    REQUIRE(from_little_endian_unsafe<uint8_t>(bytes.begin()) == expected);
}

TEST_CASE("endian  from big endian unsafe  one byte  expected", "[endian tests]") {
    static uint8_t const expected = 0xff;
    static auto const bytes = data_chunk{ expected };
    REQUIRE(from_big_endian_unsafe<uint8_t>(bytes.begin()) == expected);
}

TEST_CASE("endian  round trip32  little to big  expected", "[endian tests]") {
    static const uint32_t expected = 123456789u;
    auto little_endian = to_little_endian<uint32_t>(expected);
    REQUIRE(from_little_endian_unsafe<uint32_t>(little_endian.begin()) == expected);

    std::reverse(little_endian.begin(), little_endian.end());
    REQUIRE(from_big_endian_unsafe<uint32_t>(little_endian.begin()) == expected);
}

TEST_CASE("endian  round trip32  big to little  expected", "[endian tests]") {
    static const uint32_t expected = 123456789u;
    auto big_endian = to_big_endian<uint32_t>(expected);
    REQUIRE(from_big_endian_unsafe<uint32_t>(big_endian.begin()) == expected);

    std::reverse(big_endian.begin(), big_endian.end());
    REQUIRE(from_little_endian_unsafe<uint32_t>(big_endian.begin()) == expected);
}

TEST_CASE("endian  round trip32  big to big  expected", "[endian tests]") {
    static const uint32_t expected = 123456789u;
    auto const big_endian = to_big_endian<uint32_t>(expected);
    REQUIRE(from_big_endian_unsafe<uint32_t>(big_endian.begin()) == expected);
}

TEST_CASE("endian  round trip32  little to little  expected", "[endian tests]") {
    static const uint32_t expected = 123456789u;
    auto const little_endian = to_little_endian<uint32_t>(expected);
    REQUIRE(from_little_endian_unsafe<uint32_t>(little_endian.begin()) == expected);
}

TEST_CASE("endian  round trip64  little to little  expected", "[endian tests]") {
    static uint64_t const expected = 0x1122334455667788;
    auto const little_endian = to_little_endian<uint64_t>(expected);
    REQUIRE(from_little_endian_unsafe<uint64_t>(little_endian.begin()) == expected);
}

TEST_CASE("endian  round trip64  big to big  expected", "[endian tests]") {
    static uint64_t const expected = 0x1122334455667788;
    auto const big_endian = to_big_endian<uint64_t>(expected);
    REQUIRE(from_big_endian_unsafe<uint64_t>(big_endian.begin()) == expected);
}

// End Test Suite
