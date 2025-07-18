// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_CRYPTO_PARSE_ENCRYPTED_PREFIX_IPP
#define KTH_CRYPTO_PARSE_ENCRYPTED_PREFIX_IPP

#include <cstdint>
#include <cstddef>

#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/wallet/encrypted_keys.hpp>

namespace kth::infrastructure::wallet {

template <size_t Size>
parse_encrypted_prefix<Size>::parse_encrypted_prefix(
    const byte_array<Size>& value)
  : prefix_(value), valid_(false)
{
}

template <size_t Size>
uint8_t parse_encrypted_prefix<Size>::context() const {
    return prefix_.back();
}

template <size_t Size>
byte_array<Size> parse_encrypted_prefix<Size>::prefix() const {
    return prefix_;
}

template <size_t Size>
bool parse_encrypted_prefix<Size>::valid() const {
    return valid_;
}

template <size_t Size>
void parse_encrypted_prefix<Size>::valid(bool value)
{
    valid_ = value;
}

} // namespace kth::infrastructure::wallet

#endif
