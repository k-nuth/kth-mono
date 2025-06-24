// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <sstream>

#include <test_helpers.hpp>

#include <kth/infrastructure.hpp>

using namespace kth;
using namespace kth::infrastructure;

// Start Test Suite: stream tests

TEST_CASE("stream - is exhausted initialized empty stream returns true", "[stream tests]") {
    std::stringstream stream;
    istream_reader source(stream);
    REQUIRE(source.is_exhausted());
}

TEST_CASE("stream - is exhausted initialized nonempty stream returns false", "[stream tests]") {
    std::stringstream stream("abc");
    istream_reader source(stream);
    REQUIRE( ! source.is_exhausted());
}

TEST_CASE("stream - peek byte nonempty stream does not advance", "[stream tests]") {
    uint8_t const expected = 'a';
    std::stringstream stream("ab");
    istream_reader source(stream);
    REQUIRE(source.peek_byte() == expected);
    REQUIRE(source.peek_byte() == expected);
    REQUIRE(source.peek_byte() == expected);
    REQUIRE((bool)source);
}

TEST_CASE("stream - roundtrip byte", "[stream tests]") {
    uint8_t const expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_byte(expected);
    auto const result = source.read_byte();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 2 bytes little endian", "[stream tests]") {
    const uint16_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_2_bytes_little_endian(expected);
    auto const result = source.read_2_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 4 bytes little endian", "[stream tests]") {
    const uint32_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_4_bytes_little_endian(expected);
    auto const result = source.read_4_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 8 bytes little endian", "[stream tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_8_bytes_little_endian(expected);
    auto const result = source.read_8_bytes_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 2 bytes big endian", "[stream tests]") {
    const uint16_t expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_2_bytes_big_endian(expected);
    auto const result = source.read_2_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 4 bytes big endian", "[stream tests]") {
    const uint32_t expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_4_bytes_big_endian(expected);
    auto const result = source.read_4_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip 8 bytes big endian", "[stream tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_8_bytes_big_endian(expected);
    auto const result = source.read_8_bytes_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}


TEST_CASE("stream - roundtrip variable uint little endian 1 byte", "[stream tests]") {
    uint64_t const expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_little_endian(expected);

    REQUIRE(stream.str().length() == 1u);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint little endian 2 bytes", "[stream tests]") {
    uint64_t const expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_little_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint16_t) + 1);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint little endian 4 bytes", "[stream tests]") {
    uint64_t const expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_little_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint32_t) + 1);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint little endian 8 bytes", "[stream tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_little_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint64_t) + 1);

    auto const result = source.read_variable_little_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint big endian 1 byte", "[stream tests]") {
    uint64_t const expected = 0xAA;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    REQUIRE(stream.str().length() == 1u);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint big endian 2 bytes", "[stream tests]") {
    uint64_t const expected = 43707;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);

    sink.write_variable_big_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint16_t) + 1);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint big endian 4 bytes", "[stream tests]") {
    uint64_t const expected = 2898120443;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint32_t) + 1);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip variable uint big endian 8 bytes", "[stream tests]") {
    uint64_t const expected = 0xd4b14be5d8f02abe;
    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_variable_big_endian(expected);

    REQUIRE(stream.str().length() == sizeof(uint64_t) + 1);

    auto const result = source.read_variable_big_endian();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip data chunk", "[stream tests]") {
    data_chunk const expected
    {
        0xfb, 0x44, 0x68, 0x84, 0xc6, 0xbf, 0x33, 0xc6, 0x27, 0x54, 0x73, 0x92,
        0x52, 0xa7, 0xb0, 0xf7, 0x47, 0x87, 0x89, 0x28, 0xf2, 0xf4, 0x18, 0x1d,
        0x01, 0x3f, 0xb7, 0xa2, 0xe9, 0x66, 0x69, 0xbf, 0x06, 0x83, 0x45, 0x34,
        0x8e, 0xc2, 0x9b, 0x3c, 0x86, 0xa9, 0xb8, 0x5f, 0xf7, 0x11, 0xa2, 0x00,
        0x5a, 0xa8
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_bytes(expected);

    REQUIRE(stream.str().length() == expected.size());

    auto const result = source.read_bytes(expected.size());

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip hash", "[stream tests]") {
    hash_digest const expected
    {
        {
            0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
            0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
            0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_hash(expected);

    REQUIRE(stream.str().length() == expected.size());

    auto const result = source.read_hash();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip short hash", "[stream tests]") {
    short_hash const expected
    {
        {
            0xed, 0x36, 0x48, 0xaf, 0x53, 0xc2, 0x8a, 0x79, 0x90, 0xab, 0x62, 0x04,
            0xb5, 0x2c, 0x6a, 0x40, 0xdc, 0x6d, 0xa5, 0xfe
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_short_hash(expected);

    REQUIRE(stream.str().length() == expected.size());

    auto const result = source.read_short_hash();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip fixed string", "[stream tests]") {
    std::string const expected = "my string data";

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_string(expected, 10);

    REQUIRE(stream.str().length() == 10u);

    auto const result = source.read_string(10);

    REQUIRE(expected.substr(0, 10) == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - roundtrip string", "[stream tests]") {
    std::string const expected = "my string data";

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_string(expected);

    REQUIRE(stream.str().length() == (expected.length() + message::variable_uint_size(expected.length())));

    auto const result = source.read_string();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

TEST_CASE("stream - read bytes to eof", "[stream tests]") {
    data_chunk const expected
    {
        {
            0x4d, 0xc9, 0x32, 0x18, 0x4d, 0x86, 0xa0, 0xb2, 0xe4, 0xba, 0x65, 0xa8,
            0x36, 0x1f, 0xea, 0x05, 0xf0, 0x26, 0x68, 0xa5, 0x09, 0x69, 0x10, 0x39,
            0x08, 0x95, 0x00, 0x7d, 0xa4, 0x2e, 0x7c, 0x12
        }
    };

    std::stringstream stream;
    ostream_writer sink(stream);
    istream_reader source(stream);
    sink.write_bytes(expected);

    REQUIRE(stream.str().length() == expected.size());

    auto const result = source.read_bytes();

    REQUIRE(expected == result);
    REQUIRE(stream);
    REQUIRE((bool)sink);
    REQUIRE((bool)source);
    REQUIRE( ! sink == false);
    REQUIRE( ! source == false);
}

// End Test Suite
