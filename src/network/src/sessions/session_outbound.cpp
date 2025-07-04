// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/network/sessions/session_outbound.hpp>

#include <cstddef>
#include <functional>
#include <kth/domain.hpp>
#include <kth/network/p2p.hpp>
#include <kth/network/protocols/protocol_address_31402.hpp>
#include <kth/network/protocols/protocol_ping_31402.hpp>
#include <kth/network/protocols/protocol_ping_60001.hpp>
#include <kth/network/protocols/protocol_reject_70002.hpp>
#include <kth/network/protocols/protocol_version_31402.hpp>
#include <kth/network/protocols/protocol_version_70002.hpp>

namespace kth::network {

#define CLASS session_outbound

using namespace std::placeholders;

session_outbound::session_outbound(p2p& network, bool notify_on_connect)
    : session_batch(network, notify_on_connect)
    , CONSTRUCT_TRACK(session_outbound)
{}

// Start sequence.
// ----------------------------------------------------------------------------

void session_outbound::start(result_handler handler) {
    if (settings_.outbound_connections == 0) {
        LOG_INFO(LOG_NETWORK, "Not configured for generating outbound connections.");
        handler(error::success);
        return;
    }

    LOG_INFO(LOG_NETWORK, "Starting outbound session.");

    session::start(CONCURRENT_DELEGATE2(handle_started, _1, handler));
}

void session_outbound::handle_started(code const& ec, result_handler handler) {
    if (ec) {
        handler(ec);
        return;
    }

    for (size_t peer = 0; peer < settings_.outbound_connections; ++peer) {
        new_connection(error::success);
    }

    // This is the end of the start sequence.
    handler(error::success);
}

// Connnect cycle.
// ----------------------------------------------------------------------------

void session_outbound::new_connection(code const&) {
    if (stopped()) {
        LOG_DEBUG(LOG_NETWORK, "Suspended outbound connection.");
        return;
    }

    session_batch::connect(BIND2(handle_connect, _1, _2));
}

void session_outbound::handle_connect(code const& ec, channel::ptr channel) {
    if (ec) {
        LOG_DEBUG(LOG_NETWORK, "Failure connecting outbound: ", ec.message());
        // Retry with conditional delay in case of network error.
        dispatch_delayed(cycle_delay(ec), BIND1(new_connection, _1));
        return;
    }

    register_channel(channel,
        BIND2(handle_channel_start, _1, channel),
        BIND2(handle_channel_stop, _1, channel));
}

void session_outbound::handle_channel_start(code const& ec, channel::ptr channel) {
    // The start failure is also caught by handle_channel_stop.
    if (ec) {
        LOG_DEBUG(LOG_NETWORK
           , "Outbound channel failed to start ["
           , channel->authority(), "] ", ec.message());
        return;
    }

    LOG_DEBUG(LOG_NETWORK, "Connected outbound channel [", channel->authority(), "] (", connection_count(), ")");
    attach_protocols(channel);
}

void session_outbound::attach_protocols(channel::ptr channel) {
    auto const version = channel->negotiated_version();

    if (version >= domain::message::version::level::bip31) {
        attach<protocol_ping_60001>(channel)->start();
    } else {
        attach<protocol_ping_31402>(channel)->start();
    }

    if (version >= domain::message::version::level::bip61) {
        attach<protocol_reject_70002>(channel)->start();
    }

    attach<protocol_address_31402>(channel)->start();
}

void session_outbound::attach_handshake_protocols(channel::ptr channel, result_handler handle_started) {
    using serve = domain::message::version::service;
    auto const relay = settings_.relay_transactions;
    auto const own_version = settings_.protocol_maximum;
    auto const own_services = settings_.services;
    auto const invalid_services = settings_.invalid_services;
    auto const minimum_version = settings_.protocol_minimum;

    auto const minimum_services = serve::node_network;

    // Reject messages are not handled until bip61 (70002).
    // The negotiated_version is initialized to the configured maximum.
    if (channel->negotiated_version() >= domain::message::version::level::bip61) {
        attach<protocol_version_70002>(channel, own_version, own_services,
            invalid_services, minimum_version, minimum_services, relay)
            ->start(handle_started);
    } else {
        attach<protocol_version_31402>(channel, own_version, own_services,
            invalid_services, minimum_version, minimum_services)
            ->start(handle_started);
    }
}

void session_outbound::handle_channel_stop(code const& ec, channel::ptr channel) {
    LOG_DEBUG(LOG_NETWORK
       , "Outbound channel stopped [", channel->authority(), "] "
       , ec.message());

    new_connection(error::success);
}

// Channel start sequence.
// ----------------------------------------------------------------------------
// Pend outgoing connections so we can detect connection to self.

void session_outbound::start_channel(channel::ptr channel, result_handler handle_started) {
    const result_handler unpend_handler = BIND3(do_unpend, _1, channel, handle_started);

    auto const ec = pend(channel);

    if (ec) {
        unpend_handler(ec);
        return;
    }

    session::start_channel(channel, unpend_handler);
}

void session_outbound::do_unpend(code const& ec, channel::ptr channel, result_handler handle_started) {
    unpend(channel);
    handle_started(ec);
}

} // namespace kth::network