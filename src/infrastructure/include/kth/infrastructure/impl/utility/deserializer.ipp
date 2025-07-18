// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_DESERIALIZER_IPP
#define KTH_INFRASTRUCTURE_DESERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>

#include <kth/infrastructure/constants.hpp>
#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/utility/assert.hpp>
#include <kth/infrastructure/utility/endian.hpp>

namespace kth {

// Since the end is not used just use begin.
template <typename Iterator, bool CheckSafe>
deserializer<Iterator, CheckSafe>::deserializer(Iterator begin)
    : iterator_(begin), end_(begin), valid_(true)
{}

template <typename Iterator, bool CheckSafe>
deserializer<Iterator, CheckSafe>::deserializer(Iterator begin, Iterator end)
    : iterator_(begin), end_(end), valid_(true)
{}

// Context.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
deserializer<Iterator, CheckSafe>::operator bool() const {
    return valid_;
}

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::operator!() const {
    return !valid_;
}

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::is_exhausted() const {
    // This is always true in an unsafe reader.
    return !valid_ || remaining() == 0;
}

template <typename Iterator, bool CheckSafe>
void deserializer<Iterator, CheckSafe>::invalidate() {
    valid_ = false;
}

// Hashes.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
hash_digest deserializer<Iterator, CheckSafe>::read_hash() {
    return read_forward<hash_size>();
}

template <typename Iterator, bool CheckSafe>
short_hash deserializer<Iterator, CheckSafe>::read_short_hash() {
    return read_forward<short_hash_size>();
}

template <typename Iterator, bool CheckSafe>
mini_hash deserializer<Iterator, CheckSafe>::read_mini_hash() {
    return read_forward<mini_hash_size>();
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
uint16_t deserializer<Iterator, CheckSafe>::read_2_bytes_big_endian() {
    return read_big_endian<uint16_t>();
}

template <typename Iterator, bool CheckSafe>
uint32_t deserializer<Iterator, CheckSafe>::read_4_bytes_big_endian() {
    return read_big_endian<uint32_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_8_bytes_big_endian() {
    return read_big_endian<uint64_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_variable_big_endian() {
    auto const value = read_byte();

    switch (value) {
        case varint_eight_bytes:
            return read_8_bytes_big_endian();
        case varint_four_bytes:
            return read_4_bytes_big_endian();
        case varint_two_bytes:
            return read_2_bytes_big_endian();
        default:
            return value;
    }
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::read_size_big_endian() {
    auto const size = read_variable_big_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t) {
        return static_cast<size_t>(size);
    }

    invalidate();
    return 0;
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
code deserializer<Iterator, CheckSafe>::read_error_code() {
    auto const value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

template <typename Iterator, bool CheckSafe>
uint16_t deserializer<Iterator, CheckSafe>::read_2_bytes_little_endian() {
    return read_little_endian<uint16_t>();
}

template <typename Iterator, bool CheckSafe>
uint32_t deserializer<Iterator, CheckSafe>::read_4_bytes_little_endian() {
    return read_little_endian<uint32_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_8_bytes_little_endian() {
    return read_little_endian<uint64_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_variable_little_endian() {
    auto const value = read_byte();

    switch (value) {
        case varint_eight_bytes:
            return read_8_bytes_little_endian();
        case varint_four_bytes:
            return read_4_bytes_little_endian();
        case varint_two_bytes:
            return read_2_bytes_little_endian();
        default:
            return value;
    }
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::read_size_little_endian() {
    auto const size = read_variable_little_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size <= max_size_t) {
        return static_cast<size_t>(size);
    }

    invalidate();
    return 0;
}

// Bytes.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
uint8_t deserializer<Iterator, CheckSafe>::peek_byte() {
    if ( ! safe(1)) {
        invalidate();
    }

    return valid_ ? *iterator_ : 0;
}

template <typename Iterator, bool CheckSafe>
uint8_t deserializer<Iterator, CheckSafe>::read_byte() {
    ////return read_forward<1>()[0];

    if ( ! safe(1)) {
        invalidate();
    }

    return valid_ ? *iterator_++ : 0;
}

template <typename Iterator, bool CheckSafe>
data_chunk deserializer<Iterator, CheckSafe>::read_bytes() {
    // This read is always safe but always reads zero bytes in unsafe reader.
    return read_bytes(remaining());
}

// Return size is guaranteed.
// This is a memory exhaustion risk if caller does not control size.
template <typename Iterator, bool CheckSafe>
data_chunk deserializer<Iterator, CheckSafe>::read_bytes(size_t size) {
    // TODO: avoid unnecessary default zero fill using
    // the allocator adapter here: stackoverflow.com/a/21028912/1172329.
    data_chunk out(size);

    if ( ! safe(size)) {
        invalidate();
    }

    if ( ! valid_ || size == 0) {
        return out;
    }

    auto const begin = iterator_;
    iterator_ += size;
    std::copy_n(begin, size, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
std::string deserializer<Iterator, CheckSafe>::read_string() {
    return read_string(read_size_little_endian());
}

// Removes trailing zeros, required for bitcoin string comparisons.
template <typename Iterator, bool CheckSafe>
std::string deserializer<Iterator, CheckSafe>::read_string(size_t size) {
    if ( ! safe(size)) {
        invalidate();
    }

    if ( ! valid_) {
        return {};
    }

    std::string out;
    out.reserve(size);

    // Read up to size characters, stopping at the first null (may be many).
    for (size_t index = 0; index < size; ++index) {
        auto const character = iterator_[index];

        if (character == string_terminator) {
            break;
        }

        out.push_back(character);
    }

    // Consume all size characters from the buffer.
    iterator_ += size;

    // Reduce the allocation to the number of characters pushed.
    out.shrink_to_fit();
    return out;
}

template <typename Iterator, bool CheckSafe>
void deserializer<Iterator, CheckSafe>::skip(size_t size) {
    if ( ! safe(size)) {
        invalidate();
    }

    if ( ! valid_) {
        return;
    }

    iterator_ += size;
}

template <typename Iterator, bool CheckSafe>
void deserializer<Iterator, CheckSafe>::skip_remaining() {
    return skip(remaining());
}

template <typename Iterator, bool CheckSafe>
template <unsigned Size>
byte_array<Size> deserializer<Iterator, CheckSafe>::read_forward() {
    if ( ! safe(Size)) {
        invalidate();
    }

    if ( ! valid_ || Size == 0) {
        return {};
    }

    byte_array<Size> out;
    auto const begin = iterator_;
    iterator_ += Size;
    std::copy_n(begin, Size, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
template <unsigned Size>
byte_array<Size> deserializer<Iterator, CheckSafe>::read_reverse() {
    if ( ! safe(Size)) {
        invalidate();
    }

    if ( ! valid_) {
        return {};
    }

    byte_array<Size> out;
    auto const begin = iterator_;
    iterator_ += Size;
    std::reverse_copy(begin, iterator_, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
template <typename Integer>
Integer deserializer<Iterator, CheckSafe>::read_big_endian() {
    if ( ! safe(sizeof(Integer))) {
        invalidate();
    }

    if ( ! valid_) {
        return {};
    }

    auto const begin = iterator_;
    iterator_ += sizeof(Integer);
    return from_big_endian_unsafe<Integer>(begin);
}

template <typename Iterator, bool CheckSafe>
template <typename Integer>
Integer deserializer<Iterator, CheckSafe>::read_little_endian() {
    if ( ! safe(sizeof(Integer))) {
        invalidate();
    }

    if ( ! valid_) {
        return {};
    }

    auto const begin = iterator_;
    iterator_ += sizeof(Integer);
    return from_little_endian_unsafe<Integer>(begin);
}

// private

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::safe(size_t size) const {
    // Bounds checking is disabled for unsafe deserializers.
    if ( ! CheckSafe) {
        return true;
    }

    return size <= remaining();
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::remaining() const {
    return std::distance(iterator_, end_);
}

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
deserializer<Iterator, true> make_safe_deserializer(Iterator begin, Iterator end) {
    return deserializer<Iterator, true>(begin, end);
}

template <typename Iterator>
deserializer<Iterator, false> make_unsafe_deserializer(Iterator begin) {
    return deserializer<Iterator, false>(begin);
}

} // namespace kth

#endif
