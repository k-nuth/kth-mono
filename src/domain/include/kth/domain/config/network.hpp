// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_CONFIG_NETWORK_HPP_
#define KTH_DOMAIN_CONFIG_NETWORK_HPP_

#include <string>

namespace kth::domain::config {

// For configuration network initialization in other libraries.
enum class network {
     mainnet
    , testnet
    , regtest
#if defined(KTH_CURRENCY_BCH)
    , testnet4
    , scalenet
    , chipnet
#endif
};

inline constexpr
std::string_view name(network net) {
    switch (net) {
        case network::testnet:
            return "Testnet";
        case network::regtest:
            return "Regtest";
#if defined(KTH_CURRENCY_BCH)
        case network::testnet4:
            return "Testnet4";
        case network::scalenet:
            return "Scalenet";
        case network::chipnet:
            return "Chipnet";
#endif
        default:
        case network::mainnet:
            return "Mainnet";
    }
}

} // namespace kth::domain::config

#endif // KTH_DOMAIN_CONFIG_NETWORK_HPP_
