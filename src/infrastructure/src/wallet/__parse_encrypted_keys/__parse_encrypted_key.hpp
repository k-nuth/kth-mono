// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_CRYPTO_PARSE_ENCRYPTED_KEY_HPP
#define KTH_CRYPTO_PARSE_ENCRYPTED_KEY_HPP

#include <cstdint>
#include <cstddef>

#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/wallet/encrypted_keys.hpp>

#include "parse_encrypted_prefix.hpp"

namespace kth::infrastructure::wallet {

// Swap not defined.
template <size_t PrefixSize>
class parse_encrypted_key
  : public parse_encrypted_prefix<PrefixSize>
{
public:
    parse_encrypted_key(const byte_array<PrefixSize>& prefix,
        const one_byte& flags, const ek_salt& salt, const ek_entropy& entropy);

    bool compressed() const;
    bool lot_sequence() const;
    data_chunk owner_salt() const;

    uint8_t flags() const;
    ek_salt salt() const;
    ek_entropy entropy() const;

private:
    const one_byte flags_;
    const ek_salt salt_;
    const ek_entropy entropy_;
};

} // namespace kth::infrastructure::wallet

#include "parse_encrypted_key.ipp"

#endif
