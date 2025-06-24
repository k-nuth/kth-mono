// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/user_agent.hpp>
#include <kth/node/version.hpp>

namespace kth::node {

std::string get_user_agent() {
#if defined(KTH_CURRENCY_BCH)
    return "/kth-bch:" KTH_NODE_VERSION "/";
#elif defined(KTH_CURRENCY_BTC)
    return "/kth-btc:" KTH_NODE_VERSION "/";
#elif defined(KTH_CURRENCY_LTC)
    return "/kth-ltc:" KTH_NODE_VERSION "/";
#endif
}

} // namespace kth::node
