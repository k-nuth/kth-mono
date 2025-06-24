// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_GET_DATA_HPP
#define KTH_DOMAIN_MESSAGE_GET_DATA_HPP

#include <initializer_list>
#include <istream>
#include <memory>
#include <string>

#include <kth/domain/define.hpp>
#include <kth/domain/message/inventory.hpp>
#include <kth/domain/message/inventory_vector.hpp>
#include <kth/domain/multi_crypto_settings.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API get_data : public inventory {
public:
    using ptr = std::shared_ptr<get_data>;
    using const_ptr = std::shared_ptr<const get_data>;

    get_data() = default;
    get_data(inventory_vector::list const& values);
    get_data(inventory_vector::list&& values);
    get_data(hash_list const& hashes, type_id type);
    get_data(std::initializer_list<inventory_vector> const& values);

    bool operator==(get_data const& x) const;
    bool operator!=(get_data const& x) const;

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) { /*override*/  //TODO(fernando): check if this function is used in a run-time-polymorphic way
        if ( ! inventory::from_data(source, version)) {
            return false;
        }

        if (version < get_data::version_minimum) {
            source.invalidate();
        }

        if ( ! source) {
            reset();
        }

        return source;
    }

#if defined(KTH_SEGWIT_ENABLED)
    /// Convert message types to witness types.
    void to_witness();
#endif

    static
    std::string const command;

    static
    uint32_t const version_minimum;

    static
    uint32_t const version_maximum;
};

} // namespace kth::domain::message

#endif
