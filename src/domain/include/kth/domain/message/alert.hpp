// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_ALERT_HPP
#define KTH_DOMAIN_MESSAGE_ALERT_HPP

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

class KD_API alert {
public:
    using ptr = std::shared_ptr<alert>;
    using const_ptr = std::shared_ptr<const alert>;

    alert() = default;
    alert(data_chunk const& payload, data_chunk const& signature);
    alert(data_chunk&& payload, data_chunk&& signature);

    // alert(alert const& x) = default;
    // alert(alert&& x) = default;
    // /// This class is move assignable but not copy assignable.
    // alert& operator=(alert&& x) = default;
    // alert& operator=(alert const&) = default;

    bool operator==(alert const& x) const;
    bool operator!=(alert const& x) const;

    data_chunk& payload();

    [[nodiscard]]
    data_chunk const& payload() const;

    void set_payload(data_chunk const& value);
    void set_payload(data_chunk&& value);

    data_chunk& signature();

    [[nodiscard]]
    data_chunk const& signature() const;

    void set_signature(data_chunk const& value);
    void set_signature(data_chunk&& value);

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t /*version*/) {
        reset();

        payload_ = source.read_bytes(source.read_size_little_endian());
        signature_ = source.read_bytes(source.read_size_little_endian());

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
        sink.write_variable_little_endian(payload_.size());
        sink.write_bytes(payload_);
        sink.write_variable_little_endian(signature_.size());
        sink.write_bytes(signature_);
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
    data_chunk payload_;
    data_chunk signature_;
};

} // namespace kth::domain::message

#endif
