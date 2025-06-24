// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;

// Start Test Suite: memory pool tests

TEST_CASE("memory pool - from data insufficient version failure", "[memory pool]") {
    const message::memory_pool expected;
    data_chunk const data = expected.to_data(message::version::level::maximum);
    message::memory_pool instance{};
    REQUIRE( ! entity_from_data(instance, message::memory_pool::version_minimum - 1, data));
    REQUIRE( ! instance.is_valid());
}

TEST_CASE("memory pool - roundtrip to data factory from data chunk", "[memory pool]") {
    const message::memory_pool expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    auto const result = create<message::memory_pool>(
        message::version::level::maximum, data);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

TEST_CASE("memory pool - roundtrip to data factory from data stream", "[memory pool]") {
    const message::memory_pool expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    data_source istream(data);
    auto const result = create<message::memory_pool>(
        message::version::level::maximum, istream);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

TEST_CASE("memory pool - roundtrip to data factory from data reader", "[memory pool]") {
    const message::memory_pool expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<message::memory_pool>(
        message::version::level::maximum, source);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

// End Test Suite
