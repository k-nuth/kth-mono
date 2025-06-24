// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DATABASE_TRANSACTION_UNCONFIRMED_ENTRY_HPP_
#define KTH_DATABASE_TRANSACTION_UNCONFIRMED_ENTRY_HPP_

#include <kth/domain.hpp>

#include <kth/database/currency_config.hpp>
#include <kth/database/define.hpp>

namespace kth::database {


class KD_API transaction_unconfirmed_entry {
public:

    transaction_unconfirmed_entry() = default;

    transaction_unconfirmed_entry(domain::chain::transaction const& tx, uint32_t arrival_time, uint32_t height);

    // Getters
    domain::chain::transaction const& transaction() const;
    uint32_t arrival_time() const;
    uint32_t height() const;

    bool is_valid() const;

    //TODO(fernando): make this constexpr
    // constexpr
    static
    size_t serialized_size(domain::chain::transaction const& tx);

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;


    template <typename W, KTH_IS_WRITER(W)>
    void to_data(W& sink) const {
        factory_to_data(sink, transaction_, arrival_time_, height_);
    }

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);


    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source) {
        reset();

#if defined(KTH_CACHED_RPC_DATA)
        transaction_.from_data(source, false, true, true);
#else
        transaction_.from_data(source, false, true);
#endif
        arrival_time_ = source.read_4_bytes_little_endian();

        height_ = source.read_4_bytes_little_endian();

        if ( ! source) {
            reset();
        }

        return source;
    }

    static
    data_chunk factory_to_data(domain::chain::transaction const& tx, uint32_t arrival_time, uint32_t height);

    static
    void factory_to_data(std::ostream& stream, domain::chain::transaction const& tx, uint32_t arrival_time, uint32_t height);

    template <typename W, KTH_IS_WRITER(W)>
    static
    void factory_to_data(W& sink, domain::chain::transaction const& tx, uint32_t arrival_time, uint32_t height) {

#if defined(KTH_CACHED_RPC_DATA)
        tx.to_data(sink, false, true, true);
#else
        tx.to_data(sink, false, true);
#endif

        sink.write_4_bytes_little_endian(arrival_time);
        sink.write_4_bytes_little_endian(height);

    }

private:
    void reset();

    domain::chain::transaction transaction_;
    uint32_t arrival_time_;
    uint32_t height_;
};

} // namespace kth::database


#endif // KTH_DATABASE_TRANSACTION_UNCONFIRMED_ENTRY_HPP_
