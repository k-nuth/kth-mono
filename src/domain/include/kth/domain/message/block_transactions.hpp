// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_BLOCK_TRANSACTIONS_HPP
#define KTH_DOMAIN_MESSAGE_BLOCK_TRANSACTIONS_HPP

#include <istream>

#include <kth/domain/chain/transaction.hpp>
#include <kth/domain/constants.hpp>
#include <kth/domain/define.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API block_transactions {
public:
    using ptr = std::shared_ptr<block_transactions>;
    using const_ptr = std::shared_ptr<const block_transactions>;

    block_transactions();
    block_transactions(hash_digest const& block_hash, chain::transaction::list const& transactions);
    block_transactions(hash_digest const& block_hash, chain::transaction::list&& transactions);

    // block_transactions(block_transactions const& x) = default;
    // block_transactions(block_transactions&& x) = default;
    // // This class is move assignable but not copy assignable.
    // block_transactions& operator=(block_transactions&& x) = default;
    // block_transactions& operator=(block_transactions const&) = default;

    bool operator==(block_transactions const& x) const;
    bool operator!=(block_transactions const& x) const;


    hash_digest& block_hash();

    [[nodiscard]]
    hash_digest const& block_hash() const;

    void set_block_hash(hash_digest const& value);

    chain::transaction::list& transactions();

    [[nodiscard]]
    chain::transaction::list const& transactions() const;

    void set_transactions(chain::transaction::list const& x);
    void set_transactions(chain::transaction::list&& x);

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) {
        reset();

        block_hash_ = source.read_hash();
        auto const count = source.read_size_little_endian();

        // Guard against potential for arbitary memory allocation.
        if (count > static_absolute_max_block_size()) {
            source.invalidate();
        } else {
            transactions_.resize(count);
        }

        // Order is required.
        for (auto& tx : transactions_) {
            if ( ! entity_from_data(tx, source, true, witness_default())) {
                break;
            }
        }

        if (version < block_transactions::version_minimum) {
            source.invalidate();
        }

        if ( ! source) {
            reset();
        }

        return source;
    }

    [[nodiscard]]
    data_chunk to_data(uint32_t version) const;

    void to_data(uint32_t version, data_sink& stream) const;

    template <typename W>
    void to_data(uint32_t /*version*/, W& sink) const {
        sink.write_hash(block_hash_);
        sink.write_variable_little_endian(transactions_.size());

        for (auto const& element : transactions_) {
            element.to_data(sink, /*wire*/ true, witness_default()
#ifdef KTH_CACHED_RPC_DATA
                            , /*unconfirmed*/ false
#endif
                            );
        }
    }

    //void to_data(uint32_t version, writer& sink) const;
    [[nodiscard]]
    bool is_valid() const;

    void reset();

    [[nodiscard]]
    size_t serialized_size(uint32_t version) const;

    static
    std::string const command;

    static
    uint32_t const version_minimum;

    static
    uint32_t const version_maximum;

private:
    hash_digest block_hash_;
    chain::transaction::list transactions_;
};

} // namespace kth::domain::message

#endif
