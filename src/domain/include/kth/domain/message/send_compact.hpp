// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_SEND_COMPACT_BLOCKS_HPP
#define KTH_DOMAIN_MESSAGE_SEND_COMPACT_BLOCKS_HPP

#include <istream>
#include <memory>
#include <string>

#include <kth/domain/define.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API send_compact {
public:
    using ptr = std::shared_ptr<send_compact>;
    using const_ptr = std::shared_ptr<const send_compact>;

    static
    size_t satoshi_fixed_size(uint32_t version);

    send_compact() = default;
    send_compact(bool high_bandwidth_mode, uint64_t version);

    // send_compact(send_compact const& x) = default;
    // send_compact(send_compact&& x) = default;
    // /// This class is move assignable but not copy assignable.
    // send_compact& operator=(send_compact&& x) = default;
    // send_compact& operator=(send_compact const&) = default;

    bool operator==(send_compact const& x) const;
    bool operator!=(send_compact const& x) const;


    [[nodiscard]]
    bool high_bandwidth_mode() const;

    void set_high_bandwidth_mode(bool mode);

    [[nodiscard]]
    uint64_t version() const;

    void set_version(uint64_t version);

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) {
        reset();

        auto const mode = source.read_byte();

        if (mode > 1) {
            source.invalidate();
        }

        high_bandwidth_mode_ = (mode == 1);
        this->version_ = source.read_8_bytes_little_endian();

        if (version < send_compact::version_minimum) {
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
        sink.write_byte(static_cast<uint8_t>(high_bandwidth_mode_));
        sink.write_8_bytes_little_endian(this->version_);
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
    bool high_bandwidth_mode_{false};
    uint64_t version_{0};
};

} // namespace kth::domain::message

#endif
