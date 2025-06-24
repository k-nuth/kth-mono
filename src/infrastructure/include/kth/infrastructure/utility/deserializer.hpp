// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_DESERIALIZER_HPP
#define KTH_INFRASTRUCTURE_DESERIALIZER_HPP

#include <cstddef>
#include <cstdint>
#include <string>

#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/hash_define.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>

namespace kth {

/// Reader to wrap arbitrary iterator.
template <typename Iterator, bool CheckSafe>
class deserializer { // : public reader/*, noncopyable*/
public:
    explicit
    deserializer(Iterator begin);

    deserializer(Iterator begin, Iterator end);

    template <unsigned Size>
    byte_array<Size> read_forward();

    template <unsigned Size>
    byte_array<Size> read_reverse();

    template <typename Integer>
    Integer read_big_endian();

    template <typename Integer>
    Integer read_little_endian();

    /// Context.
    // implicit
    operator bool() const;

    bool operator!() const;
    bool is_exhausted() const;
    void invalidate();

    /// Read hashes.
    hash_digest read_hash();
    short_hash read_short_hash();
    mini_hash read_mini_hash();

    /// Read big endian integers.
    uint16_t read_2_bytes_big_endian();
    uint32_t read_4_bytes_big_endian();
    uint64_t read_8_bytes_big_endian();
    uint64_t read_variable_big_endian();
    size_t read_size_big_endian();

    /// Read little endian integers.
    code read_error_code();
    uint16_t read_2_bytes_little_endian();
    uint32_t read_4_bytes_little_endian();
    uint64_t read_8_bytes_little_endian();
    uint64_t read_variable_little_endian();
    size_t read_size_little_endian();

    /// Read/peek one byte.
    uint8_t peek_byte();
    uint8_t read_byte();

    /// Read all remaining bytes (always safe).
    data_chunk read_bytes();

    /// Read required size buffer.
    data_chunk read_bytes(size_t size);

    /// Read variable length string.
    std::string read_string();

    /// Read required size string and trim nulls.
    std::string read_string(size_t size);

    /// Advance iterator without reading.
    void skip(size_t size);

    /// Advance iterator without reading.
    void skip_remaining();

private:
    // True if is a safe deserializer and size does not exceed remaining bytes.
    bool safe(size_t size) const;

    // The number of bytes remaining in the buffer.
    size_t remaining() const;

    bool valid_;
    Iterator iterator_;
    const Iterator end_;
};

// Factories.
//-----------------------------------------------------------------------------

/// Slower deserializer (with bounds checking).
/// Safe for use with public data, caller should check object state.
template <typename Iterator>
deserializer<Iterator, true> make_safe_deserializer(Iterator begin, Iterator end);

/// Faster deserializer (without bounds checking).
/// Intended for use with internal/protected buffers only.
template <typename Iterator>
deserializer<Iterator, false> make_unsafe_deserializer(Iterator begin);

} // namespace kth

#include <kth/infrastructure/impl/utility/deserializer.ipp>

#endif
