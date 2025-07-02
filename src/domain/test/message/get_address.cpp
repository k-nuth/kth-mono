// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;

// Start Test Suite: get address tests

TEST_CASE("get address - roundtrip to data factory from data chunk", "[get address]") {
    const message::get_address expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    auto const result = create<message::get_address>(
        message::version::level::minimum, data);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

TEST_CASE("get address - roundtrip to data factory from data stream", "[get address]") {
    const message::get_address expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    auto const result = create<message::get_address>(
        message::version::level::minimum, istream);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

TEST_CASE("get address - roundtrip to data factory from data reader", "[get address]") {
    const message::get_address expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<message::get_address>(
        message::version::level::minimum, source);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

// End Test Suite
