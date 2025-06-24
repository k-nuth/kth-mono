// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_COMMON_HPP_
#define KTH_DOMAIN_COMMON_HPP_

#include <utility>

#include <kth/domain/concepts.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>

namespace kth::domain {

// enum class wire_t : bool {};
// enum class witness_t : bool {};

template <typename T, typename R, KTH_IS_READER(R), typename... U>
inline
bool entity_from_data(T& x, R& source, U&&... u) {
    return x.from_data(source, std::forward<U>(u)...);
}

template <typename T, typename... U>
inline
bool entity_from_data(T& x, std::istream& stream, U&&... u) {
    istream_reader stream_r(stream);
    return entity_from_data(x, stream_r, std::forward<U>(u)...);
}

template <typename T, typename... U>
inline
bool entity_from_data(T& x, data_chunk const& data, U&&... u) {
    data_source istream(data);
    return entity_from_data(x, istream, std::forward<U>(u)...);
}

template <typename T, typename R, KTH_IS_READER(R), typename... U>
inline
bool entity_from_data(T& x, uint32_t version, R& source, U&&... u) {
    return x.from_data(source, version, std::forward<U>(u)...);
}

template <typename T, typename... U>
inline
bool entity_from_data(T& x, uint32_t version, std::istream& stream, U&&... u) {
    istream_reader stream_r(stream);
    return entity_from_data(x, version, stream_r, std::forward<U>(u)...);
}

template <typename T, typename... U>
inline
bool entity_from_data(T& x, uint32_t version, data_chunk const& data, U&&... u) {
    data_source istream(data);
    return entity_from_data(x, version, istream, std::forward<U>(u)...);
}

template <typename T, typename... U>
inline
T create(U&&... u) {
    T x;
    entity_from_data(x, std::forward<U>(u)...);
    return x;
}

} // namespace kth::domain

#endif // KTH_DOMAIN_COMMON_HPP_




// template <typename T, typename R, KTH_IS_READER(R), typename... U>
// T create(R& source, U&&... u) {
//     T x;
//     x.from_data(source, std::forward<U>(u)...);
//     return x;
// }

// template <typename T, typename R, KTH_IS_READER(R)>
// inline
// T create(R& source, bool witness = false) {
//     T x;
//     x.from_data(source, witness_val(witness));
//     return x;
// }

// template <typename T, typename R, KTH_IS_READER(R)>
// inline
// T create(R& source, bool wire = true) {
//     T x;
//     x.from_data(source, wire);
//     return x;
// }


// template <typename T>
// inline
// T create(data_chunk const& data, bool witness) {
//     T x;
//     x.from_data(data, witness_val(witness));
//     return x;
// }

// template <typename T>
// inline
// T create(std::istream& stream, bool witness) {
//     T x;
//     x.from_data(stream, witness_val(witness));
//     return x;
// }
