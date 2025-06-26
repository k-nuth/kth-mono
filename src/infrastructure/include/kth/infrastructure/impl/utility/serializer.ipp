// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_SERIALIZER_IPP
#define KTH_INFRASTRUCTURE_SERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <kth/infrastructure/constants.hpp>
#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/utility/endian.hpp>

namespace kth {

template <typename Iterator>
serializer<Iterator>::serializer(Iterator begin)
    : iterator_(begin), valid_(true)
{}

// Context.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator>::operator bool() const {
    return valid_;
}

template <typename Iterator>
bool serializer<Iterator>::operator!() const {
    return !valid_;
}

// Hashes.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_hash(hash_digest const& hash) {
    write_forward(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_short_hash(short_hash const& hash) {
    write_forward(hash);
}

template <typename Iterator>
void serializer<Iterator>::write_mini_hash(mini_hash const& hash) {
    write_forward(hash);
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_2_bytes_big_endian(uint16_t value) {
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_4_bytes_big_endian(uint32_t value) {
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_8_bytes_big_endian(uint64_t value) {
    write_big_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_variable_big_endian(uint64_t value) {
    if (value < varint_two_bytes) {
        write_byte(static_cast<uint8_t>(value));
    } else if (value <= max_uint16) {
        write_byte(varint_two_bytes);
        write_2_bytes_big_endian(static_cast<uint16_t>(value));
    } else if (value <= max_uint32) {
        write_byte(varint_four_bytes);
        write_4_bytes_big_endian(static_cast<uint32_t>(value));
    } else {
        write_byte(varint_eight_bytes);
        write_8_bytes_big_endian(value);
    }
}

template <typename Iterator>
void serializer<Iterator>::write_size_big_endian(size_t value) {
    write_variable_big_endian(value);
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_error_code(code const& ec) {
    write_4_bytes_little_endian(static_cast<uint32_t>(ec.value()));
}

template <typename Iterator>
void serializer<Iterator>::write_2_bytes_little_endian(uint16_t value) {
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_4_bytes_little_endian(uint32_t value) {
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_8_bytes_little_endian(uint64_t value) {
    write_little_endian(value);
}

template <typename Iterator>
void serializer<Iterator>::write_variable_little_endian(uint64_t value) {
    if (value < varint_two_bytes) {
        write_byte(static_cast<uint8_t>(value));
    } else if (value <= max_uint16) {
        write_byte(varint_two_bytes);
        write_2_bytes_little_endian(static_cast<uint16_t>(value));
    } else if (value <= max_uint32) {
        write_byte(varint_four_bytes);
        write_4_bytes_little_endian(static_cast<uint32_t>(value));
    } else {
        write_byte(varint_eight_bytes);
        write_8_bytes_little_endian(value);
    }
}

template <typename Iterator>
void serializer<Iterator>::write_size_little_endian(size_t value) {
    write_variable_little_endian(value);
}

// Bytes (unchecked).
//-----------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_byte(uint8_t value) {
    *iterator_++ = value;
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(data_chunk const& data) {
    write_forward(data);
}

template <typename Iterator>
void serializer<Iterator>::write_bytes(uint8_t const* data, size_t size) {
    iterator_ = std::copy_n(data, size, iterator_);
}

template <typename Iterator>
void serializer<Iterator>::write_string(std::string const& value) {
    write_variable_little_endian(value.size());
    write_forward(value);
}

template <typename Iterator>
void serializer<Iterator>::write_string(std::string const& value, size_t size) {
    auto const length = std::min(size, value.size());
    write_bytes(reinterpret_cast<uint8_t const*>(value.data()), length);
    data_chunk padding(floor_subtract(size, length), string_terminator);
    write_bytes(padding);
}

template <typename Iterator>
void serializer<Iterator>::skip(size_t size) {
    iterator_ += size;
}

// non-interface
//-------------------------------------------------------------------------

template <typename Iterator>
void serializer<Iterator>::write_delegated(functor write) {
    write(*this);
}

template <typename Iterator>
template <typename Buffer>
void serializer<Iterator>::write_forward(const Buffer& data) {
    iterator_ = std::copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Buffer>
void serializer<Iterator>::write_reverse(const Buffer& data) {
    iterator_ = std::reverse_copy(data.begin(), data.end(), iterator_);
}

template <typename Iterator>
template <typename Integer>
void serializer<Iterator>::write_big_endian(Integer value) {
    return write_forward(to_big_endian(value));
}

template <typename Iterator>
template <typename Integer>
void serializer<Iterator>::write_little_endian(Integer value) {
    return write_forward(to_little_endian(value));
}

template <typename Iterator>
size_t serializer<Iterator>::read_size_big_endian() {
    static_assert(sizeof(size_t) >= sizeof(uint32_t), "unexpected size_t size");
    auto const prefix = *iterator_++;
    auto const begin = iterator_;
    uint64_t size;

    switch (prefix) {
        case varint_eight_bytes:
            iterator_ += sizeof(uint64_t);
            size = from_big_endian_unsafe<uint64_t>(begin);
        case varint_four_bytes:
            iterator_ += sizeof(uint32_t);
            return from_big_endian_unsafe<uint32_t>(begin);
        case varint_two_bytes:
            iterator_ += sizeof(uint16_t);
            return from_big_endian_unsafe<uint16_t>(begin);
        default:
            return prefix;
    }

    // This facilitates safely passing the size into a follow-on writer.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t) {
        return static_cast<size_t>(size);
    }

    valid_ = false;
    return 0;
}

template <typename Iterator>
size_t serializer<Iterator>::read_size_little_endian() {
    static_assert(sizeof(size_t) >= sizeof(uint32_t), "unexpected size_t size");
    auto const prefix = *iterator_++;
    auto const begin = iterator_;
    uint64_t size;

    switch (prefix) {
        case varint_eight_bytes:
            iterator_ += sizeof(uint64_t);
            size = from_little_endian_unsafe<uint64_t>(begin);
        case varint_four_bytes:
            iterator_ += sizeof(uint32_t);
            return from_little_endian_unsafe<uint32_t>(begin);
        case varint_two_bytes:
            iterator_ += sizeof(uint16_t);
            return from_little_endian_unsafe<uint16_t>(begin);
        default:
            return prefix;
    }

    // This facilitates safely passing the size into a follow-on writer.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t) {
        return static_cast<size_t>(size);
    }

    valid_ = false;
    return 0;
}

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
serializer<Iterator> make_unsafe_serializer(Iterator begin) {
    return serializer<Iterator>(begin);
}

} // namespace kth

#endif
