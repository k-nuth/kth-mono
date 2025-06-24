// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTUCTURE_MINI_KEYS_HPP
#define KTH_INFRASTUCTURE_MINI_KEYS_HPP

#include <string>

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/math/elliptic_curve.hpp>

namespace kth::infrastructure::wallet {

/**
 * Convert Cascasius minikey to secret parameter.
 */
KI_API bool minikey_to_secret(ec_secret& out_secret, std::string const& key);

} // namespace kth::infrastructure::wallet

#endif
