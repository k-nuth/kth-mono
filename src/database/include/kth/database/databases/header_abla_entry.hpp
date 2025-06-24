// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DATABASE_HEADER_ABLA_ENTRY_HPP_
#define KTH_DATABASE_HEADER_ABLA_ENTRY_HPP_

#include <kth/domain.hpp>
#include <kth/database/define.hpp>

namespace kth::database {

using header_with_abla_state_t = std::tuple<domain::chain::header, uint64_t, uint64_t, uint64_t>;

data_chunk to_data_with_abla_state(domain::chain::block const& block);
void to_data_with_abla_state(std::ostream& stream, domain::chain::block const& block);

template <typename W, KTH_IS_WRITER(W)>
void to_data_with_abla_state(W& sink, domain::chain::block const& block) {
    block.header().to_data(sink, true);

    if (block.validation.state) {
        sink.write_8_bytes_little_endian(block.validation.state->abla_state().block_size);
        sink.write_8_bytes_little_endian(block.validation.state->abla_state().control_block_size);
        sink.write_8_bytes_little_endian(block.validation.state->abla_state().elastic_buffer_size);
    } else {
        sink.write_8_bytes_little_endian(0);
        sink.write_8_bytes_little_endian(0);
        sink.write_8_bytes_little_endian(0);
    }
}

std::optional<header_with_abla_state_t> get_header_and_abla_state_from_data(data_chunk const& data);
std::optional<header_with_abla_state_t> get_header_and_abla_state_from_data(std::istream& stream);

template <typename R, KTH_IS_READER(R)>
std::optional<header_with_abla_state_t> get_header_and_abla_state_from_data(R& source) {
    domain::chain::header header;
    header.from_data(source, true);

    if ( ! source) {
        return {};
    }

    uint64_t block_size = source.read_8_bytes_little_endian();
    uint64_t control_block_size = source.read_8_bytes_little_endian();
    uint64_t elastic_buffer_size = source.read_8_bytes_little_endian();

    if ( ! source) {
        return std::make_tuple(std::move(header), 0, 0, 0);
    }

    return std::make_tuple(std::move(header), block_size, control_block_size, elastic_buffer_size);
}

} // namespace kth::database

#endif // KTH_DATABASE_HEADER_ABLA_ENTRY_HPP_
