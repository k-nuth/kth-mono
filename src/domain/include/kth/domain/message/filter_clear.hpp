// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_FILTER_CLEAR_HPP
#define KTH_DOMAIN_MESSAGE_FILTER_CLEAR_HPP

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

class KD_API filter_clear {
public:
    using ptr = std::shared_ptr<filter_clear>;
    using const_ptr = std::shared_ptr<const filter_clear>;

    static
    size_t satoshi_fixed_size(uint32_t version);

    // This is a default instance so is invalid.
    // The only way to make this valid is to deserialize it :/.
    filter_clear() = default;
    filter_clear(filter_clear const& x) = default;
    filter_clear(filter_clear&& x) = default;

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) {
        reset();

        // Initialize as valid from deserialization.
        insufficient_version_ = false;

        if (version < filter_clear::version_minimum) {
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
    void to_data(uint32_t /*version*/, W&  /*sink*/) const {
    }

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

protected:
    filter_clear(bool insufficient_version);

private:
    bool insufficient_version_{true};
};

} // namespace kth::domain::message

#endif
