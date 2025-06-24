// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/database/databases/header_abla_entry.hpp>

#include <cstddef>
#include <cstdint>

#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::database {

data_chunk to_data_with_abla_state(domain::chain::block const& block) {
    data_chunk data;
    auto const size = block.header().serialized_size(true) + 8 + 8 + 8;
    data.reserve(size);
    data_sink ostream(data);
    to_data_with_abla_state(ostream, block);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void to_data_with_abla_state(std::ostream& stream, domain::chain::block const& block) {
    ostream_writer sink(stream);
    to_data_with_abla_state(sink, block);
}

std::optional<header_with_abla_state_t> get_header_and_abla_state_from_data(data_chunk const& data) {
    data_source istream(data);
    return get_header_and_abla_state_from_data(istream);
}

std::optional<header_with_abla_state_t> get_header_and_abla_state_from_data(std::istream& stream) {
    istream_reader source(stream);
    return get_header_and_abla_state_from_data(source);
}

} // namespace kth::database
