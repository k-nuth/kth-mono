// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/node/protocols/protocol_transaction_in.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

#if ! defined(__EMSCRIPTEN__)
#include <kth/network.hpp>
#endif
#include <kth/node/define.hpp>
#include <kth/node/full_node.hpp>

namespace kth::node {

#define NAME "transaction_in"
#define CLASS protocol_transaction_in

using namespace kth::blockchain;
using namespace kth::domain::message;
using namespace kth::network;
using namespace std::placeholders;

inline
uint64_t to_relay_fee(float minimum_byte_fee) {
    // Spending one standard prevout with one output is nominally 189 bytes.
    static size_t const small_transaction_size = 189;
    return static_cast<uint64_t>(minimum_byte_fee * small_transaction_size);
}

protocol_transaction_in::protocol_transaction_in(full_node& node, channel::ptr channel, safe_chain& chain)
    : protocol_events(node, channel, NAME)
    , chain_(chain)

    // TODO: move fee_filter to a derived class protocol_transaction_in_70013.
    , minimum_relay_fee_(negotiated_version() >= version::level::bip133 ?
        to_relay_fee(node.chain_settings().byte_fee_satoshis) : 0)

    // TODO: move relay to a derived class protocol_transaction_in_70001.
    // In the mean time, restrict by negotiated protocol level.
    // Because of inconsistent implementation by version we must allow relay
    // at bip37 version or below. Enforcement starts above bip37 version.
    , relay_from_peer_(negotiated_version() <= version::level::bip37 ||
        node.network_settings().relay_transactions)

    // TODO: move memory_pool to a derived class protocol_transaction_in_60002.
    , refresh_pool_(negotiated_version() >= version::level::bip35 &&
        node.node_settings().refresh_transactions)

    , CONSTRUCT_TRACK(protocol_transaction_in)
{}

// Start.
//-----------------------------------------------------------------------------

void protocol_transaction_in::start() {

    protocol_events::start(BIND1(handle_stop, _1));

    SUBSCRIBE2(inventory, handle_receive_inventory, _1, _2);
    SUBSCRIBE2(transaction, handle_receive_transaction, _1, _2);

    // TODO: move fee_filter to a derived class protocol_transaction_in_70013.
    if (minimum_relay_fee_ != 0) {
        // Have the peer filter the transactions it announces to us.
        SEND2(fee_filter{minimum_relay_fee_}, handle_send, _1, fee_filter::command);
    }

    // TODO: move memory_pool to a derived class protocol_transaction_in_60002.
    if (refresh_pool_ && relay_from_peer_ && !chain_.is_stale()) {
        // Refresh transaction pool on connect.
        SEND2(memory_pool{}, handle_send, _1, memory_pool::command);
    }
}

// Receive inventory sequence.
//-----------------------------------------------------------------------------

bool protocol_transaction_in::handle_receive_inventory(code const& ec, inventory_const_ptr message) {
    if (stopped(ec)) {
        return false;
    }

    auto const response = std::make_shared<get_data>();

    // Copy the transaction inventories into a get_data instance.
    message->reduce(response->inventories(), inventory::type_id::transaction);

    // TODO: move relay to a derived class protocol_transaction_in_70001.
    // Prior to this level transaction relay is not configurable.
    if ( ! relay_from_peer_ && ! response->inventories().empty()) {
        LOG_WARNING(LOG_NODE, "Unexpected transaction inventory from [", authority(), "]");
        stop(error::channel_stopped);
        return false;
    }

    // TODO: manage channel relay at the service layer.
    // Do not process tx inventory while chain is stale.
    if (chain_.is_stale()) {
        return true;
    }

    // Remove hashes of (unspent) transactions that we already have.
    // BUGBUG: this removes spent transactions which it should not (see BIP30).

    // LOG_INFO(LOG_NODE, "send_get_transactions() - before filter_transactions - 1");
    chain_.filter_transactions(response, BIND2(send_get_data, _1, response));
    return true;
}

void protocol_transaction_in::send_get_data(code const& ec, get_data_ptr message) {
    if (stopped(ec) || message->inventories().empty()) {
        return;
    }

    if (ec) {
        LOG_ERROR(LOG_NODE
           , "Internal failure filtering transaction hashes for ["
           , authority(), "] ", ec.message());
        stop(ec);
        return;
    }

    // inventory->get_data[transaction]
    SEND2(*message, handle_send, _1, message->command);
}

// Receive transaction sequence.
//-----------------------------------------------------------------------------

// A transaction is acceptable whether solicited or broadcast.
bool protocol_transaction_in::handle_receive_transaction(code const& ec, transaction_const_ptr message) {

    if (stopped(ec)) {
        return false;
    }

    // TODO: move relay to a derived class protocol_transaction_in_70001.
    // Prior to this level transaction relay is not configurable.
    if ( ! relay_from_peer_) {
        LOG_DEBUG(LOG_NODE, "Unexpected transaction relay from [", authority(), "]");
        stop(error::channel_stopped);
        return false;
    }

    // TODO: manage channel relay at the service layer.
    // Do not process transactions while chain is stale.
    if (chain_.is_stale()) {
        return true;
    }

    message->validation.originator = nonce();
    chain_.organize(message, BIND2(handle_store_transaction, _1, message));
    return true;
}

// The transaction has been saved to the memory pool (or not).
// This will be picked up by subscription in transaction_out and will cause
// the transaction to be announced to non-originating relay-accepting peers.
void protocol_transaction_in::handle_store_transaction(code const& ec, transaction_const_ptr message) {
    if (stopped(ec)) {
        return;
    }

    // Ask the peer for ancestor txs if this one is an orphan.
    // We may not get this transaction back, but that is a fair tradeoff for
    // not having to store potentially invalid transactions.
    if (ec == error::orphan_transaction) {
        send_get_transactions(message);
    }

    auto const encoded = encode_hash(message->hash());

    // It is okay for us to receive a duplicate or a missing outputs tx but it
    // is not generally okay to receive an otherwise invalid transaction.
    // Below-fee transactions can be sent prior to fee_filter receipt or due to
    // a negotiated version below BIP133 (7013).

    // TODO: differentiate failure conditions and send reject as applicable.

    if (ec) {
        // This should not happen with a single peer since we filter inventory.
        // However it will happen when a block or another peer's tx intervenes.
        LOG_DEBUG(LOG_NODE
           , "Dropped transaction [", encoded, "] from [", authority()
           , "] ", ec.message());
        return;
    }

    LOG_DEBUG(LOG_NODE
       , "Stored transaction [", encoded, "] from [", authority()
       , "].");
}

// This will get chatty if the peer sends mempool response out of order.
// This requests the next level of missing tx, but those may be orphans as
// well. Those would also be discarded, until arriving at connectable txs.
// There is no facility to re-obtain the dropped transactions. They must be
// obtained coincidentally by peers, by a mempool message (new channel) or as a
// result of transaction resends.
void protocol_transaction_in::send_get_transactions(transaction_const_ptr message) {
    inventory::type_id type = inventory::type_id::transaction;

    auto missing = message->missing_previous_transactions();

    if (missing.empty()) {
        return;
    }

    auto const request = std::make_shared<get_data>(std::move(missing), type);

    // Remove hashes of (unspent) transactions that we already have.
    // This removes spent transactions which is not correnct, however given the
    // treatment of duplicate hashes by other nodes and the fact that this is
    // a set of pool transactions only, this is okay.

    // LOG_INFO(LOG_NODE, "send_get_transactions() - before filter_transactions - 2");
    chain_.filter_transactions(request, BIND2(send_get_data, _1, request));
}

// Stop.
//-----------------------------------------------------------------------------

void protocol_transaction_in::handle_stop(code const&) {
    LOG_DEBUG(LOG_NETWORK, "Stopped transaction_in protocol for [", authority(), "].");
}

} // namespace kth::node