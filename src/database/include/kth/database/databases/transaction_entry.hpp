// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DATABASE_TRANSACTION_ENTRY_HPP_
#define KTH_DATABASE_TRANSACTION_ENTRY_HPP_

#include <kth/domain.hpp>

#include <kth/database/currency_config.hpp>
#include <kth/database/define.hpp>

namespace kth::database {


template <typename W, KTH_IS_WRITER(W)>
void write_position(W& serial, uint32_t position) {
    serial.KTH_POSITION_WRITER(position);
}

template <typename Deserializer>
uint32_t read_position(Deserializer& deserial) {
    return deserial.KTH_POSITION_READER();
}

class KD_API transaction_entry {
public:

    transaction_entry() = default;

    transaction_entry(domain::chain::transaction const& tx, uint32_t height, uint32_t median_time_past, uint32_t position);

    // Getters
    domain::chain::transaction const& transaction() const;
    uint32_t height() const;
    uint32_t median_time_past() const;
    uint32_t position() const;

    bool is_valid() const;

    //TODO(fernando): make this constexpr
    // constexpr
    static
    size_t serialized_size(domain::chain::transaction const& tx);

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;


    template <typename W, KTH_IS_WRITER(W)>
    void to_data(W& sink) const {
        factory_to_data(sink, transaction_, height_, median_time_past_, position_ );
    }

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);


    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source) {
        reset();

#if defined(KTH_CACHED_RPC_DATA)
        transaction_.from_data(source, false, true, false);
#else
        transaction_.from_data(source, false, true);
#endif
        height_ = source.read_4_bytes_little_endian();
        median_time_past_ = source.read_4_bytes_little_endian();
        position_ = read_position(source);

        if ( ! source) {
            reset();
        }

        return source;
    }

    bool confirmed() const;

    //TODO(kth): we don't have spent information
    //bool is_spent(size_t fork_height) const;

    static
    data_chunk factory_to_data(domain::chain::transaction const& tx, uint32_t height, uint32_t median_time_past, uint32_t position);

    static
    void factory_to_data(std::ostream& stream, domain::chain::transaction const& tx, uint32_t height, uint32_t median_time_past, uint32_t position);

    template <typename W, KTH_IS_WRITER(W)>
    static
    void factory_to_data(W& sink, domain::chain::transaction const& tx, uint32_t height, uint32_t median_time_past, uint32_t position) {
#if defined(KTH_CACHED_RPC_DATA)
        tx.to_data(sink, false, true, false);
#else
        tx.to_data(sink, false, true);
#endif

        sink.write_4_bytes_little_endian(height);
        sink.write_4_bytes_little_endian(median_time_past);
        write_position(sink, position);
    }

private:
    void reset();

    domain::chain::transaction transaction_;
    uint32_t height_;
    uint32_t median_time_past_;
    uint32_t position_;
};

} // namespace kth::database


#endif // KTH_DATABASE_TRANSACTION_ENTRY_HPP_
