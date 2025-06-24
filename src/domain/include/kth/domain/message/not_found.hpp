// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MESSAGE_NOT_FOUND_HPP
#define KTH_DOMAIN_MESSAGE_NOT_FOUND_HPP

#include <initializer_list>
#include <istream>
#include <memory>
#include <string>

#include <kth/domain/constants.hpp>
#include <kth/domain/define.hpp>
#include <kth/domain/message/inventory.hpp>
#include <kth/domain/message/inventory_vector.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::message {

class KD_API not_found : public inventory {
public:
    using ptr = std::shared_ptr<not_found>;
    using const_ptr = std::shared_ptr<const not_found>;

    not_found() = default;
    not_found(inventory_vector::list const& values);
    not_found(inventory_vector::list&& values);
    not_found(hash_list const& hashes, type_id type);
    not_found(std::initializer_list<inventory_vector> const& values);

    // not_found(not_found const& x) = default;
    // not_found(not_found&& x) = default;
    // // This class is move assignable but not copy assignable.
    // not_found& operator=(not_found&& x) = default;
    // not_found& operator=(not_found const&) = default;

    bool operator==(not_found const& x) const;
    bool operator!=(not_found const& x) const;

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, uint32_t version) {
        if ( ! inventory::from_data(source, version)) {
            return false;
        }

        if (version < not_found::version_minimum) {
            source.invalidate();
        }

        if ( ! source) {
            reset();
        }

        return source;
    }


    static
    std::string const command;

    static
    uint32_t const version_minimum;

    static
    uint32_t const version_maximum;

};

} // namespace kth::domain::message

#endif
