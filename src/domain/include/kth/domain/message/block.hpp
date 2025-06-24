// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_BLOCK_HPP
#define KTH_DOMAIN_MESSAGE_BLOCK_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>

#include <kth/domain/chain/block.hpp>
#include <kth/domain/chain/header.hpp>
#include <kth/domain/chain/transaction.hpp>
#include <kth/domain/define.hpp>
#include <kth/domain/message/version.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API block : public chain::block {
public:
    using ptr = std::shared_ptr<block>;
    using const_ptr = std::shared_ptr<const block>;
    using ptr_list = std::vector<ptr>;
    using const_ptr_list = std::vector<const_ptr>;
    using const_ptr_list_ptr = std::shared_ptr<const_ptr_list>;
    using const_ptr_list_const_ptr = std::shared_ptr<const const_ptr_list>;

    block() = default;

    block(chain::block const& x);
    block(chain::block&& x);

    block(chain::header const& header, chain::transaction::list&& transactions);
    block(chain::header const& header, chain::transaction::list const& transactions);

    block& operator=(chain::block&& x);

    // block(block const& x) = default;
    // block(block&& x) = default;
    // // This class is move assignable but not copy assignable.
    // block& operator=(block&& x) = default;
    // block& operator=(block const&) = default;

    bool operator==(chain::block const& x) const;
    bool operator!=(chain::block const& x) const;
    bool operator==(block const& x) const;
    bool operator!=(block const& x) const;

    // Witness is always deserialized if present.
    // NOTE: Witness on BCH is dissabled on the chain::block class
    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t /*version*/) {
        return chain::block::from_data(source, true);
    }

    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, data_sink& stream) const;

    template <typename W>
    void to_data(uint32_t /*version*/, W& sink) const {
        chain::block::to_data(sink, true);
    }

    //void to_data(uint32_t version, writer& sink) const;
    size_t serialized_size(uint32_t version) const;


    static
    std::string const command;

    static
    uint32_t const version_minimum;

    static
    uint32_t const version_maximum;
};

//TODO(fernando): check this family of functions: to_data_header_nonce
template <typename W>
void to_data_header_nonce(block const& block, uint64_t nonce, W& sink) {
    block.header().to_data(sink);
    sink.write_8_bytes_little_endian(nonce);
}
// void to_data_header_nonce(block const& block, uint64_t nonce, writer& sink);

// void to_data_header_nonce(block const& block, uint64_t nonce, std::ostream& stream);
void to_data_header_nonce(block const& block, uint64_t nonce, data_sink& stream);

data_chunk to_data_header_nonce(block const& block, uint64_t nonce);

hash_digest hash(block const& block, uint64_t nonce);

} // namespace kth::domain::message

#endif
