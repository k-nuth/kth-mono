// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <kth/domain/chain/token_data.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::chain::token::encoding {

inline
data_chunk to_data(fungible const& x) {
    data_chunk data;
    auto const size = serialized_size(x);
    data.reserve(size);
    data_sink ostream(data);
    ostream_writer sink_w(ostream);
    to_data(sink_w, x);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

inline
data_chunk to_data(non_fungible const& x) {
    data_chunk data;
    auto const size = serialized_size(x);
    data.reserve(size);
    data_sink ostream(data);
    ostream_writer sink_w(ostream);
    to_data(sink_w, x);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

inline
data_chunk to_data(both_kinds const& x) {
    data_chunk data;
    auto const size = serialized_size(x);
    data.reserve(size);
    data_sink ostream(data);
    ostream_writer sink_w(ostream);
    to_data(sink_w, x);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

inline
data_chunk to_data(token_data_t const& x) {
    data_chunk data;
    auto const size = serialized_size(x);
    data.reserve(size);
    data_sink ostream(data);
    ostream_writer sink_w(ostream);
    to_data(sink_w, x);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

} // namespace kth::domain::chain::encoding

