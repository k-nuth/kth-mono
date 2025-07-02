// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;
using namespace kth::domain::message;

// Start Test Suite: filter clear tests

TEST_CASE("filter clear - from data insufficient version failure", "[filter clear]") {
    static const filter_clear expected{};
    auto const raw = expected.to_data(version::level::maximum);
    filter_clear instance{};

    REQUIRE( ! entity_from_data(instance, filter_clear::version_minimum - 1, raw));
    REQUIRE( ! instance.is_valid());
}

TEST_CASE("filter clear - roundtrip to data factory from data chunk", "[filter clear]") {
    static const filter_clear expected{};
    auto const data = expected.to_data(version::level::maximum);
    auto const result = create<filter_clear>(version::level::maximum, data);

    REQUIRE(data.size() == 0u);
    REQUIRE(result.is_valid());
    REQUIRE(result.serialized_size(version::level::maximum) == 0u);
}

TEST_CASE("filter clear - roundtrip to data factory from data stream", "[filter clear]") {
    static const filter_clear expected{};
    auto const data = expected.to_data(version::level::maximum);
    data_source istream(data);
    auto const result = create<filter_clear>(version::level::maximum, istream);

    REQUIRE(data.size() == 0u);
    REQUIRE(result.is_valid());
    REQUIRE(result.serialized_size(version::level::maximum) == 0u);
}

TEST_CASE("filter clear - roundtrip to data factory from data reader", "[filter clear]") {
    static const filter_clear expected{};
    auto const data = expected.to_data(version::level::maximum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<filter_clear>(version::level::maximum, source);

    REQUIRE(data.size() == 0u);
    REQUIRE(result.is_valid());
    REQUIRE(result.serialized_size(version::level::maximum) == 0u);
}

// End Test Suite
