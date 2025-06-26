// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/blockchain/populate/populate_transaction.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>

#include <kth/blockchain/define.hpp>
#include <kth/blockchain/interface/fast_chain.hpp>
#include <kth/blockchain/pools/branch.hpp>
#include <kth/domain.hpp>

#include <kth/infrastructure/utility/synchronizer.hpp>

namespace kth::blockchain {

using namespace kd::chain;
using namespace std::placeholders;

#define NAME "populate_transaction"

// Database access is limited to calling populate_base.

#if defined(KTH_WITH_MEMPOOL)
populate_transaction::populate_transaction(dispatcher& dispatch, fast_chain const& chain, mining::mempool const& mp)
    : populate_base(dispatch, chain)
    , mempool_(mp)
{}
#else
populate_transaction::populate_transaction(dispatcher& dispatch, fast_chain const& chain)
    : populate_base(dispatch, chain)
{}
#endif

void populate_transaction::populate(transaction_const_ptr tx, result_handler&& handler) const {
    auto const state = tx->validation.state;
    KTH_ASSERT(state);

    // Chain state is for the next block, so always > 0.
    KTH_ASSERT(tx->validation.state->height() > 0);
    auto const chain_height = tx->validation.state->height() - 1u;

    //*************************************************************************
    // CONSENSUS:
    // It is OK for us to restrict *pool* transactions to those that do not
    // collide with any in the chain (as well as any in the pool) as collision
    // will result in monetary destruction and we don't want to facilitate it.
    // We must allow collisions in *block* validation if that is configured as
    // otherwise will will not follow the chain when a collision is mined.
    //*************************************************************************
    populate_base::populate_duplicate(chain_height, *tx, false);

    // Because txs include no proof of work we much short circuit here.
    // Otherwise a peer can flood us with repeat transactions to validate.
    if (tx->validation.duplicate) {
        handler(error::unspent_duplicate);
        return;
    }

    auto const total_inputs = tx->inputs().size();

    // Return if there are no inputs to validate (will fail later).
    if (total_inputs == 0) {
        handler(error::success);
        return;
    }

    auto const buckets = std::min(dispatch_.size(), total_inputs);
    auto const join_handler = synchronize(std::move(handler), buckets, NAME);
    KTH_ASSERT(buckets != 0);

    for (size_t bucket = 0; bucket < buckets; ++bucket) {
        dispatch_.concurrent(&populate_transaction::populate_inputs, this, tx, chain_height, bucket, buckets, join_handler);
    }
}

void populate_transaction::populate_inputs(transaction_const_ptr tx, size_t chain_height, size_t bucket, size_t buckets, result_handler handler) const {
    KTH_ASSERT(bucket < buckets);
    auto const& inputs = tx->inputs();

    for (auto input_index = bucket; input_index < inputs.size(); input_index = ceiling_add(input_index, buckets)) {
        auto const& input = inputs[input_index];
        auto const& prevout = input.previous_output();
        populate_prevout(chain_height, prevout, false);

#if defined(KTH_WITH_MEMPOOL)
        if ( ! prevout.validation.cache.is_valid()) {
            // BUSCAR EN UTXO DEL MEMPOOL y marcar
            prevout.validation.cache = mempool_.get_utxo(prevout);
            if (prevout.validation.cache.is_valid()) {
                prevout.validation.from_mempool = true;
            }
        }
#endif

    }

    handler(error::success);
}

} // namespace kth::blockchain
