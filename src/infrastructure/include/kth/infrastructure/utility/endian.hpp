// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ENDIAN_HPP
#define KTH_INFRASTRUCTURE_ENDIAN_HPP

#include <istream>

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/utility/data.hpp>

namespace kth {

template <typename Integer, typename Iterator>
constexpr
Integer from_big_endian(Iterator start, Iterator end);

template <typename Integer, typename Iterator>
constexpr
Integer from_little_endian(Iterator start, Iterator end);

template <typename Integer, typename Iterator>
constexpr
Integer from_big_endian_unsafe(Iterator start);

template <typename Integer, typename Iterator>
constexpr
Integer from_little_endian_unsafe(Iterator start);

template <typename Integer>
constexpr
Integer from_big_endian_stream_unsafe(std::istream& stream);

template <typename Integer>
constexpr
Integer from_little_endian_stream_unsafe(std::istream& stream);

template <typename Integer>
byte_array<sizeof(Integer)> to_big_endian(Integer value);

template <typename Integer>
byte_array<sizeof(Integer)> to_little_endian(Integer value);

} // namespace kth

#include <kth/infrastructure/impl/utility/endian.ipp>

#endif
