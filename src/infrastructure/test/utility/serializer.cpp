// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;
using namespace kth::infrastructure;

// Start Test Suite: serializer tests

TEST_CASE("serializer - roundtrip serialize deserialize", "[serializer tests]") {
    data_chunk data(1 + 2 + 4 + 8 + 4 + 4 + 3 + 7);
    auto writer = make_unsafe_serializer(data.begin());
    writer.write_byte(0x80);
    writer.write_2_bytes_little_endian(0x8040);
    writer.write_4_bytes_little_endian(0x80402010);
    writer.write_8_bytes_little_endian(0x8040201011223344);
    writer.write_4_bytes_big_endian(0x80402010);
    writer.write_variable_little_endian(1234);
    writer.write_bytes(to_chunk(to_little_endian<uint32_t>(0xbadf00d)));
    writer.write_string("hello");

    auto reader = make_safe_deserializer(data.begin(), data.end());
    REQUIRE(reader.read_byte() == 0x80u);
    REQUIRE(reader.read_2_bytes_little_endian() == 0x8040u);
    REQUIRE(reader.read_4_bytes_little_endian() == 0x80402010u);
    REQUIRE(reader.read_8_bytes_little_endian() == 0x8040201011223344u);
    REQUIRE(reader.read_4_bytes_big_endian() == 0x80402010u);
    REQUIRE(reader.read_variable_little_endian() == 1234u);
    REQUIRE(from_little_endian_unsafe<uint32_t>(reader.read_bytes(4).begin()) == 0xbadf00du);
    REQUIRE(reader.read_string() == "hello");
    REQUIRE(reader.read_byte() == 0u);
    REQUIRE(reader.is_exhausted());
}

TEST_CASE("serializer - deserializer exhaustion", "[serializer tests]") {
    data_chunk data(42);
    auto reader = make_safe_deserializer(data.begin(), data.end());
    reader.read_bytes(42);
    REQUIRE(reader);
    REQUIRE(reader.is_exhausted());
    REQUIRE(reader.read_byte() == 0u);
    REQUIRE( ! reader);
}

TEST_CASE("serializer - is exhausted initialized empty stream returns true", "[serializer tests]") {
    data_chunk data(0);
    auto source = make_safe_deserializer(data.begin(), data.end());
    REQUIRE(source.is_exhausted());
    REQUIRE((bool)source);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - is exhausted initialized nonempty stream returns false", "[serializer tests]") {
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    REQUIRE( ! source.is_exhausted());
    REQUIRE((bool)source);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - peek byte nonempty stream does not advance", "[serializer tests]") {
    uint8_t const expected = 0x42;
    data_chunk data({ expected, 0x00 });
    auto source = make_safe_deserializer(data.begin(), data.end());
    REQUIRE(source.peek_byte() == expected);
    REQUIRE(source.peek_byte() == expected);
    REQUIRE(source.peek_byte() == expected);
    REQUIRE((bool)source);
}

TEST_CASE("serializer - roundtrip byte", "[serializer tests]") {
    uint8_t const expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_byte(expected);
    auto const result = source.read_byte();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip error code", "[serializer tests]") {
    code const expected(error::futuristic_timestamp);
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_error_code(expected);
    auto const result = source.read_error_code();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip 2 bytes little endian", "[serializer tests]") {
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_2_bytes_little_endian(expected);
    auto const result = source.read_2_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);

}

TEST_CASE("serializer - roundtrip 4 bytes little endian", "[serializer tests]") {
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_4_bytes_little_endian(expected);
    auto const result = source.read_4_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip 8 bytes little endian", "[serializer tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_8_bytes_little_endian(expected);
    auto const result = source.read_8_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip 2 bytes big endian", "[serializer tests]") {
    const uint16_t expected = 43707;
    data_chunk data(2);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_2_bytes_big_endian(expected);
    auto const result = source.read_2_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip 4 bytes big endian", "[serializer tests]") {
    const uint32_t expected = 2898120443;
    data_chunk data(4);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_4_bytes_big_endian(expected);
    auto const result = source.read_4_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip 8 bytes big endian", "[serializer tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    data_chunk data(8);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_8_bytes_big_endian(expected);
    auto const result = source.read_8_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}


TEST_CASE("serializer - roundtrip variable uint little endian 1 byte", "[serializer tests]") {
    uint64_t const expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint little endian 2 bytes", "[serializer tests]") {
    uint64_t const expected = 43707;
    data_chunk data(3);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint little endian 4 bytes", "[serializer tests]") {
    uint64_t const expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint little endian 8 bytes", "[serializer tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_little_endian(expected);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint big endian 1 byte", "[serializer tests]") {
    uint64_t const expected = 0xAA;
    data_chunk data(1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint big endian 2 bytes", "[serializer tests]") {
    uint64_t const expected = 43707;
    data_chunk data(sizeof(uint16_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint big endian 4 bytes", "[serializer tests]") {
    uint64_t const expected = 2898120443;
    data_chunk data(sizeof(uint32_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip variable uint big endian 8 bytes", "[serializer tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    data_chunk data(sizeof(uint64_t) + 1);
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_variable_big_endian(expected);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip data chunk", "[serializer tests]") {
    static data_chunk const expected
    {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_bytes(expected);

    auto const result = source.read_bytes(expected.size());

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip hash", "[serializer tests]") {
    static hash_digest const expected
    {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_hash(expected);

    auto const result = source.read_hash();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip short hash", "[serializer tests]") {
    static short_hash const expected
    {
        0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79, 0x90, 0xab, 0x62, 0x04,
        0xb5, 0x2c, 0x6a, 0x40 , 0xdc, 0x6d, 0xa5, 0xfe
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_short_hash(expected);

    auto const result = source.read_short_hash();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip fixed string", "[serializer tests]") {
    std::string const expected = "my string data";

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_string(expected, 10);

    auto const result = source.read_string(10);

    REQUIRE(expected.substr(0, 10) == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - roundtrip string", "[serializer tests]") {
    std::string const expected = "my string data";

    data_chunk data((expected.length() + message::variable_uint_size(expected.length())));
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_string(expected);

    auto const result = source.read_string();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("serializer - read bytes to eof", "[serializer tests]") {
    static data_chunk const expected
    {
        0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
        0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
        0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
    };

    data_chunk data(expected.size());
    auto source = make_safe_deserializer(data.begin(), data.end());
    auto sink = make_unsafe_serializer(data.begin());

    sink.write_bytes(expected);

    auto const result = source.read_bytes();

    REQUIRE(expected == result);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

// End Test Suite
