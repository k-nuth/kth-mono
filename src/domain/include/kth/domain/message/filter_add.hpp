// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_FILTER_ADD_HPP
#define KTH_DOMAIN_MESSAGE_FILTER_ADD_HPP

#include <istream>
#include <memory>
#include <string>

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

class KD_API filter_add {
public:
    using ptr = std::shared_ptr<filter_add>;
    using const_ptr = std::shared_ptr<const filter_add>;

    filter_add() = default;
    filter_add(data_chunk const& data);
    filter_add(data_chunk&& data);

    // filter_add(filter_add const& x) = default;
    // filter_add(filter_add&& x) = default;
    // // This class is move assignable but not copy assignable.
    // filter_add& operator=(filter_add&& x) = default;
    // filter_add& operator=(filter_add const&) = default;

    bool operator==(filter_add const& x) const;
    bool operator!=(filter_add const& x) const;

    data_chunk& data();

    [[nodiscard]]
    data_chunk const& data() const;

    void set_data(data_chunk const& value);
    void set_data(data_chunk&& value);

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) {
        reset();

        auto const size = source.read_size_little_endian();

        if (size > max_filter_add) {
            source.invalidate();
        } else {
            data_ = source.read_bytes(size);
        }

        if (version < filter_add::version_minimum) {
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
        sink.write_variable_little_endian(data_.size());
        sink.write_bytes(data_);
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
    data_chunk data_;
};

} // namespace kth::domain::message

#endif
