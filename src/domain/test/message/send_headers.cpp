// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;

// Start Test Suite: send headers tests

TEST_CASE("send headers - roundtrip to data factory from data chunk", "[send headers]") {
    const message::send_headers expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    auto const result = create<message::send_headers>(
        message::version::level::maximum, data);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

TEST_CASE("send headers - roundtrip to data factory from data stream", "[send headers]") {
    const message::send_headers expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    data_source istream(data);
    auto const result = create<message::send_headers>(
        message::version::level::maximum, istream);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

TEST_CASE("send headers - roundtrip to data factory from data reader", "[send headers]") {
    const message::send_headers expected{};
    auto const data = expected.to_data(message::version::level::maximum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<message::send_headers>(
        message::version::level::maximum, source);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::maximum));
}

TEST_CASE("from data reader version prior bip130 failure", "[send headers]") {
    data_chunk data{};
    data_source istream(data);
    istream_reader source(istream);
    message::send_headers instance{};
    auto const result = entity_from_data(instance, message::version::level::bip130 - 1, source);

    REQUIRE( ! result);
    REQUIRE( ! instance.is_valid());
}

TEST_CASE("from data reader version at least bip130 success", "[send headers]") {
    data_chunk data{};
    data_source istream(data);
    istream_reader source(istream);
    message::send_headers instance{};
    auto const result = entity_from_data(instance, message::version::level::bip130, source);

    REQUIRE(result);
    REQUIRE(instance.is_valid());
}

// End Test Suite
