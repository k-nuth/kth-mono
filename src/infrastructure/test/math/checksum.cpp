// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;

// Start Test Suite: checksum tests

// TODO: add append_checksum<> tests.
// TODO: add build_checked_array<> tests.

TEST_CASE("checksum  append checksum  size  increased by checksum size", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto const data_size = data.size();
    append_checksum(data);
    REQUIRE(data.size() == data_size + checksum_size);
}

TEST_CASE("checksum  append checksum  empty  valid", "[checksum tests]") {
    data_chunk data = {};
    auto checksum = data.size();
    append_checksum(data);
    REQUIRE(data[checksum++] == 0x5du);
    REQUIRE(data[checksum++] == 0xf6u);
    REQUIRE(data[checksum++] == 0xe0u);
    REQUIRE(data[checksum++] == 0xe2u);
}

TEST_CASE("checksum  append checksum  not empty  valid", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    append_checksum(data);
    REQUIRE(data[checksum++] == 0x79u);
    REQUIRE(data[checksum++] == 0x01u);
    REQUIRE(data[checksum++] == 0xafu);
    REQUIRE(data[checksum++] == 0x93u);
}

TEST_CASE("checksum  bitcoin checksum  always  valid", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto const result = bitcoin_checksum(data);
    REQUIRE(result == 0x93af0179u);
}

TEST_CASE("checksum  build checked array  empty  valid", "[checksum tests]") {
    data_chunk data = {};
    auto checksum = data.size();
    byte_array<checksum_size> out;
    auto const result = build_checked_array(out,
    {
        data
    });
    REQUIRE(result);
    REQUIRE(out[checksum++] == 0x5du);
    REQUIRE(out[checksum++] == 0xf6u);
    REQUIRE(out[checksum++] == 0xe0u);
    REQUIRE(out[checksum++] == 0xe2u);
}

TEST_CASE("checksum  build checked array  not empty  valid", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    byte_array<checksum_size + 5> out;
    auto const result = build_checked_array(out,
    {
        data
    });
    REQUIRE(result);
    REQUIRE(out[checksum++] == 0x79u);
    REQUIRE(out[checksum++] == 0x01u);
    REQUIRE(out[checksum++] == 0xafu);
    REQUIRE(out[checksum++] == 0x93u);
}

TEST_CASE("checksum  build checked array  overflow  false", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    byte_array<checksum_size> out;
    auto const result = build_checked_array(out,
    {
        data
    });
    REQUIRE( ! result);
}

TEST_CASE("checksum  insert checksum  empty  valid", "[checksum tests]") {
    data_chunk data = {};
    auto checksum = data.size();
    byte_array<checksum_size> out;
    REQUIRE(insert_checksum(out));
    REQUIRE(out[checksum++] == 0x5du);
    REQUIRE(out[checksum++] == 0xf6u);
    REQUIRE(out[checksum++] == 0xe0u);
    REQUIRE(out[checksum++] == 0xe2u);
}

TEST_CASE("checksum  insert checksum  not empty  valid", "[checksum tests]") {
    byte_array<checksum_size + 5> out{ { 0, 0, 0, 0, 0 } };
    auto checksum = out.size() - checksum_size;
    REQUIRE(insert_checksum(out));
    REQUIRE(out[checksum++] == 0x79u);
    REQUIRE(out[checksum++] == 0x01u);
    REQUIRE(out[checksum++] == 0xafu);
    REQUIRE(out[checksum++] == 0x93u);
}

TEST_CASE("checksum  insert checksum  underflow  false", "[checksum tests]") {
    byte_array<checksum_size - 1> out;
    REQUIRE( ! insert_checksum(out));
}

TEST_CASE("checksum  verify checksum  underflow  false", "[checksum tests]") {
    data_chunk const data = { 0, 0, 0 };
    REQUIRE( ! verify_checksum(data));
}

TEST_CASE("checksum  verify checksum  not set  false", "[checksum tests]") {
    data_chunk const data = { 0, 0, 0, 0, 0 };
    REQUIRE( ! verify_checksum(data));
}

TEST_CASE("checksum  verify checksum  added  true", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    append_checksum(data);
    REQUIRE(verify_checksum(data));
}

TEST_CASE("checksum  verify checksum  invalidated  false", "[checksum tests]") {
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto const data_size = data.size();
    append_checksum(data);
    data[data_size] = 42;
    REQUIRE( ! verify_checksum(data));
}

// End Test Suite
