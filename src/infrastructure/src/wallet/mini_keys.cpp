// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/wallet/mini_keys.hpp>

#include <cstdint>
#include <string>

#include <kth/infrastructure/math/elliptic_curve.hpp>
#include <kth/infrastructure/math/hash.hpp>

namespace kth::infrastructure::wallet {

bool check_minikey(std::string const& minikey)
{
    // Legacy minikeys are 22 chars long
    bool valid = minikey.size() == 22 || minikey.size() == 30;
    return valid && sha256_hash(to_chunk(minikey + "?"))[0] == 0x00;
}

bool minikey_to_secret(ec_secret& out_secret, std::string const& key)
{
    if ( ! check_minikey(key)) {
        return false;
}

    out_secret = sha256_hash(to_chunk(key));
    return true;
}

} // namespace kth::infrastructure::wallet
