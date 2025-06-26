// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_COMMON_HPP_
#define KTH_DOMAIN_COMMON_HPP_

#include <utility>

#include <kth/domain/concepts.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>

namespace kth::domain {

//TODO: this is obselete, remove it later
template <typename T, typename... U>
[[deprecated]]
inline
T create_old(data_chunk const& data, U&&... u) {
    byte_reader reader(data);
    auto res = T::from_data(reader, std::forward<U>(u)...);
    if ( ! res) {
        return {};
    }
    return *res;
}

} // namespace kth::domain

#endif // KTH_DOMAIN_COMMON_HPP_
