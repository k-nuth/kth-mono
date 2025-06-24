// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTUCTURE_MACHINE_NUMBER_IPP
#define KTH_INFRASTUCTURE_MACHINE_NUMBER_IPP

#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include <kth/infrastructure/compat.hpp>
#include <kth/infrastructure/constants.hpp>
#include <kth/infrastructure/utility/assert.hpp>
#include <kth/infrastructure/utility/limits.hpp>

namespace kth::infrastructure::machine {

static uint64_t const negative_bit = number::negative_mask;
static uint64_t const unsigned_max_int64 = kth::max_int64;
static uint64_t const absolute_min_int64 = kth::min_int64;

inline
bool is_negative(data_chunk const& data) {
    return (data.back() & number::negative_mask) != 0;
}

inline
number::number()
    : number(0)
{}

inline
number::number(int64_t value)
    : value_(value)
{}

// Properties
//-----------------------------------------------------------------------------

// The data is interpreted as little-endian.
inline
bool number::set_data(data_chunk const& data, size_t max_size) {
    if (data.size() > max_size) {
        return false;
    }

    value_ = 0;

    if (data.empty()) {
        return true;
    }

    // This is "from little endian" with a variable buffer.
    for (size_t i = 0; i != data.size(); ++i) {
        value_ |= static_cast<int64_t>(data[i]) << (8 * i);
    }

    if (is_negative(data)) {
        auto const last_shift = 8 * (data.size() - 1);
        auto const mask = ~(negative_bit << last_shift);
        value_ = -1 * (static_cast<int64_t>(value_ & mask));
    }

    return true;
}

// The result is little-endian.
inline
data_chunk number::data() const {
    if (value_ == 0) {
        return {};
    }

    data_chunk data;
    bool const set_negative = value_ < 0;
    uint64_t absolute = set_negative ? -value_ : value_;

    // This is "to little endian" with a minimal buffer.
    while (absolute != 0) {
        data.push_back(static_cast<uint8_t>(absolute));
        absolute >>= 8;
    }

    auto const negative_bit_set = is_negative(data);

    // If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.
    if (negative_bit_set && set_negative) {
        data.push_back(number::negative_mask);

    // If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.
    } else if (negative_bit_set) {
        data.push_back(0);

    // If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    } else if (set_negative) {
        data.back() |= number::negative_mask;
    }

    return data;
}

inline
int32_t number::int32() const {
    return domain_constrain<int32_t>(value_);
}

inline
int64_t number::int64() const {
    return value_;
}

// Stack Helpers
//-----------------------------------------------------------------------------

inline
bool number::is_true() const {
    return value_ != 0;
}

inline
bool number::is_false() const {
    return value_ == 0;
}

// Operators
//-----------------------------------------------------------------------------

inline
bool number::operator>(int64_t value) const {
    return value_ > value;
}

inline
bool number::operator<(int64_t value) const {
    return value_ < value;
}

inline
bool number::operator>=(int64_t value) const {
    return value_ >= value;
}

inline
bool number::operator<=(int64_t value) const {
    return value_ <= value;
}

inline
bool number::operator==(int64_t value) const {
    return value_ == value;
}

inline
bool number::operator!=(int64_t value) const {
    return value_ != value;
}

inline
bool number::operator>(number const& x) const {
    return operator>(x.value_);
}

inline
bool number::operator<(number const& x) const {
    return operator<(x.value_);
}

inline
bool number::operator>=(number const& x) const {
    return operator>=(x.value_);
}

inline
bool number::operator<=(number const& x) const {
    return operator<=(x.value_);
}

inline
bool number::operator==(number const& x) const {
    return operator==(x.value_);
}

inline
bool number::operator!=(number const& x) const {
    return operator!=(x.value_);
}

inline
number number::operator+(int64_t value) const {
    KTH_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    return number(value_ + value);
}

inline
number number::operator-(int64_t value) const {
    KTH_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    return number(value_ - value);
}

inline
number number::operator+(number const& x) const {
    return operator+(x.value_);
}

inline
number number::operator-(number const& x) const {
    return operator-(x.value_);
}

inline
number number::operator+() const {
    return *this;
}

inline
number number::operator-() const {
    KTH_ASSERT_MSG(value_ != min_int64, "out of range");

    return number(-value_);
}

inline
number& number::operator+=(number const& x) {
    return operator+=(x.value_);
}

inline
number& number::operator-=(number const& x) {
    return operator-=(x.value_);
}

inline
number& number::operator+=(int64_t value) {
    KTH_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    value_ += value;
    return *this;
}

inline
number& number::operator-=(int64_t value) {
    KTH_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    value_ -= value;
    return *this;
}

} // namespace kth::infrastructure::machine

#endif
