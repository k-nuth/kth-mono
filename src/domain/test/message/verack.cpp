// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;

// Start Test Suite: verack tests

TEST_CASE("verack - roundtrip to data factory from data chunk", "[verack]") {
    const message::verack expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    auto const result = create<message::verack>(
        message::version::level::minimum, data);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

TEST_CASE("verack - roundtrip to data factory from data stream", "[verack]") {
    const message::verack expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    auto const result = create<message::verack>(
        message::version::level::minimum, istream);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

TEST_CASE("verack - roundtrip to data factory from data reader", "[verack]") {
    const message::verack expected{};
    auto const data = expected.to_data(message::version::level::minimum);
    data_source istream(data);
    istream_reader source(istream);
    auto const result = create<message::verack>(
        message::version::level::minimum, source);

    REQUIRE(0u == data.size());
    REQUIRE(result.is_valid());
    REQUIRE(0u == result.serialized_size(message::version::level::minimum));
}

// End Test Suite
