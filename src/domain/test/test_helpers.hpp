// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// https://github.com/catchorg/Catch2/blob/master/docs/tostring.md

#ifndef KTH_DOMAIN_TEST_HELPERS_HPP
#define KTH_DOMAIN_TEST_HELPERS_HPP

#include <ostream>

#include <kth/infrastructure/hash_define.hpp>
#include <kth/infrastructure/formats/base_16.hpp>

namespace kth {

inline
std::ostream& operator<<(std::ostream& os, hash_digest const& x) {
    os << encode_hash(x);
    return os;
}

} // namespace kth

// #include <kth/domain.hpp>
#include <kth/domain/message/version.hpp>
#include <kth/infrastructure/message/network_address.hpp>

namespace kth::infrastructure::message {

inline
std::ostream& operator<<(std::ostream& os, network_address const& x) {
    os << encode_base16(x.to_data(kth::domain::message::version::level::minimum, false));
    return os;
}

} // namespace kth::infrastructure::message {


#include <kth/domain/chain/input.hpp>
#include <kth/domain/chain/output.hpp>
#include <kth/domain/chain/transaction.hpp>

namespace kth::domain::chain {

inline
std::ostream& operator<<(std::ostream& os, input const& x) {
    os << encode_base16(x.to_data());
    return os;
}

inline
std::ostream& operator<<(std::ostream& os, output const& x) {
    os << encode_base16(x.to_data());
    return os;
}

inline
std::ostream& operator<<(std::ostream& os, transaction const& x) {
    os << encode_base16(x.to_data());
    return os;
}

} // namespace kth::domain::chain


#include <kth/domain/message/prefilled_transaction.hpp>

namespace kth::domain::message {

// inline
// std::ostream& operator<<(std::ostream& os, inventory_vector const& x) {
//     os << encode_base16(x.to_data(version::level::minimum));
//     return os;
// }

inline
std::ostream& operator<<(std::ostream& os, prefilled_transaction const& x) {
    os << encode_base16(x.to_data(version::level::minimum));
    return os;
}

} // namespace kth::domain::message


#include <catch2/catch_test_macros.hpp>

#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK(cond); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)


// #include <kth/infrastructure.hpp>
#include <kth/domain.hpp>

#endif // KTH_DOMAIN_TEST_HELPERS_HPP
