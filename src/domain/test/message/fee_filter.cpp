// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;
using namespace kth::domain::message;

// Start Test Suite: fee filter tests

TEST_CASE("fee filter  constructor 1  always invalid", "[fee filter]") {
    const fee_filter instance;
    REQUIRE( ! instance.is_valid());
}

TEST_CASE("fee filter  constructor 2  always  equals params", "[fee filter]") {
    uint64_t const value = 6434u;
    const fee_filter instance(value);
    REQUIRE(instance.is_valid());
    REQUIRE(value == instance.minimum_fee());
}

TEST_CASE("fee filter  constructor 3  always  equals params", "[fee filter]") {
    uint64_t const fee = 6434u;
    const fee_filter value(fee);
    const fee_filter instance(value);
    REQUIRE(instance.is_valid());
    REQUIRE(fee == instance.minimum_fee());
    REQUIRE(value == instance);
}

TEST_CASE("fee filter  constructor 4  always  equals params", "[fee filter]") {
    uint64_t const fee = 6434u;
    const fee_filter value(fee);
    const fee_filter instance(std::move(value));
    REQUIRE(instance.is_valid());
    REQUIRE(fee == instance.minimum_fee());
}

TEST_CASE("fee filter  from data  insufficient bytes failure", "[fee filter]") {
    data_chunk const raw = {0xab, 0x11};
    fee_filter instance;
    REQUIRE( ! entity_from_data(instance, raw, version::level::maximum));
}

TEST_CASE("fee filter  from data  insufficient version failure", "[fee filter]") {
    const fee_filter expected{1};
    auto const data = expected.to_data(fee_filter::version_maximum);
    fee_filter instance;
    REQUIRE( ! entity_from_data(instance, data, filter_add::version_minimum - 1));
}

TEST_CASE("fee filter  factory from data 1  roundtrip  success", "[fee filter]") {
    const fee_filter expected{123};
    auto const data = expected.to_data(fee_filter::version_maximum);
    auto const result = create<fee_filter>(fee_filter::version_maximum, data);
    REQUIRE(result.is_valid());
    REQUIRE(expected == result);

    auto const size = result.serialized_size(version::level::maximum);
    REQUIRE(data.size() == size);
    REQUIRE(expected.serialized_size(version::level::maximum) == size);
}

TEST_CASE("fee filter  factory from data 2  roundtrip  success", "[fee filter]") {
    const fee_filter expected{325};
    auto const data = expected.to_data(fee_filter::version_maximum);
    data_source istream(data);
    auto const result = create<fee_filter>(fee_filter::version_maximum, istream);
    REQUIRE(result.is_valid());
    REQUIRE(expected == result);

    auto const size = result.serialized_size(version::level::maximum);
    REQUIRE(data.size() == size);
    REQUIRE(expected.serialized_size(version::level::maximum) == size);
}

TEST_CASE("fee filter  factory from data 3  roundtrip  success", "[fee filter]") {
    const fee_filter expected{58246};
    auto const data = expected.to_data(fee_filter::version_maximum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<fee_filter>(fee_filter::version_maximum, source);
    REQUIRE(result.is_valid());
    REQUIRE(expected == result);

    auto const size = result.serialized_size(version::level::maximum);
    REQUIRE(data.size() == size);
    REQUIRE(expected.serialized_size(version::level::maximum) == size);
}

TEST_CASE("fee filter  minimum fee  roundtrip  success", "[fee filter]") {
    uint64_t const value = 42134u;
    fee_filter instance;
    REQUIRE(instance.minimum_fee() != value);

    instance.set_minimum_fee(value);
    REQUIRE(value == instance.minimum_fee());
}

TEST_CASE("fee filter  operator assign equals  always  matches equivalent", "[fee filter]") {
    fee_filter value(2453u);
    REQUIRE(value.is_valid());

    fee_filter instance;
    REQUIRE( ! instance.is_valid());

    instance = std::move(value);
    REQUIRE(instance.is_valid());
}

TEST_CASE("fee filter  operator boolean equals  duplicates  returns true", "[fee filter]") {
    const fee_filter expected(2453u);
    fee_filter instance(expected);
    REQUIRE(instance == expected);
}

TEST_CASE("fee filter  operator boolean equals  differs  returns false", "[fee filter]") {
    const fee_filter expected(2453u);
    fee_filter instance;
    REQUIRE( ! (instance == expected));
}

TEST_CASE("fee filter  operator boolean not equals  duplicates  returns false", "[fee filter]") {
    const fee_filter expected(2453u);
    fee_filter instance(expected);
    REQUIRE( ! (instance != expected));
}

TEST_CASE("fee filter  operator boolean not equals  differs  returns true", "[fee filter]") {
    const fee_filter expected(2453u);
    fee_filter instance;
    REQUIRE(instance != expected);
}

// End Test Suite
