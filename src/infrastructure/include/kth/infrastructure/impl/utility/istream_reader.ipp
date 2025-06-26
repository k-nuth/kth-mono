// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ISTREAM_READER_IPP
#define KTH_INFRASTRUCTURE_ISTREAM_READER_IPP

#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/endian.hpp>

namespace kth {

template <unsigned Size>
byte_array<Size> istream_reader::read_forward()
{
    byte_array<Size> out;
    auto buffer = reinterpret_cast<char*>(out.data());
    stream_.read(buffer, Size);
    return out;
}

template <unsigned Size>
byte_array<Size> istream_reader::read_reverse()
{
    byte_array<Size> out;

    for (unsigned i = 0; i < Size; i++) {
        out[Size - (i + 1)] = read_byte();
}

    return out;
}

template <typename Integer>
Integer istream_reader::read_big_endian()
{
    return from_big_endian_stream_unsafe<Integer>(stream_);
}

template <typename Integer>
Integer istream_reader::read_little_endian()
{
    return from_little_endian_stream_unsafe<Integer>(stream_);
}

} // namespace kth

#endif
