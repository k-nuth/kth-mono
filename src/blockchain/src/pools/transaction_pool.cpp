// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/blockchain/pools/transaction_pool.hpp>

#include <cstddef>
#include <memory>

#include <kth/blockchain/settings.hpp>
#include <kth/domain.hpp>

namespace kth::blockchain {

// Duplicate tx hashes are disallowed in a block and therefore same in pool.
// A transaction hash that exists unspent in the chain is still not acceptable
// even if the original becomes spent in the same block, because the BIP30
// exmaple implementation simply tests all txs in a new block against
// transactions in previous blocks.

transaction_pool::transaction_pool(settings const& settings)
  ////: reject_conflicts_(settings.reject_conflicts),
  ////  minimum_fee_(settings.minimum_fee_satoshis)
{}

// TODO(legacy): implement block template discovery.
void transaction_pool::fetch_template(merkle_block_fetch_handler handler) const {
    size_t const height = max_size_t;
    auto const block = std::make_shared<domain::message::merkle_block>();
    handler(error::success, block, height);
}

// TODO(legacy): implement mempool message payload discovery.
void transaction_pool::fetch_mempool(size_t maximum,
    inventory_fetch_handler handler) const {
    auto const empty = std::make_shared<domain::message::inventory>();
    handler(error::success, empty);
}

} // namespace kth::blockchain
