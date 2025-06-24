// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_PREFILLED_TRANSACTION_HPP
#define KTH_DOMAIN_MESSAGE_PREFILLED_TRANSACTION_HPP

#include <istream>

#include <kth/domain/chain/transaction.hpp>
#include <kth/domain/define.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API prefilled_transaction {
public:
    using list = std::vector<prefilled_transaction>;
    using const_ptr = std::shared_ptr<const prefilled_transaction>;

    prefilled_transaction();
    prefilled_transaction(uint64_t index, chain::transaction const& tx);
    prefilled_transaction(uint64_t index, chain::transaction&& tx);
    prefilled_transaction(prefilled_transaction const& x) = default;
    prefilled_transaction(prefilled_transaction&& x) = default;

    prefilled_transaction& operator=(prefilled_transaction&& x) = default;
    prefilled_transaction& operator=(prefilled_transaction const& x) = default;

    bool operator==(prefilled_transaction const& x) const;
    bool operator!=(prefilled_transaction const& x) const;


    [[nodiscard]]
    uint64_t index() const;

    void set_index(uint64_t value);

    chain::transaction& transaction();

    [[nodiscard]]
    chain::transaction const& transaction() const;

    void set_transaction(chain::transaction const& tx);
    void set_transaction(chain::transaction&& tx);

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t /*version*/) {
        reset();

        index_ = source.read_variable_little_endian();
        transaction_.from_data(source, true, witness_default());

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
        sink.write_variable_little_endian(index_);
        transaction_.to_data(sink, /*wire*/ true, witness_default()
#ifdef KTH_CACHED_RPC_DATA
                             , /*unconfirmed*/ false
#endif
                             );
    }

    [[nodiscard]]
    bool is_valid() const;

    void reset();

    [[nodiscard]]
    size_t serialized_size(uint32_t version) const;


private:
    uint64_t index_;
    chain::transaction transaction_;
};

} // namespace kth::domain::message

#endif
