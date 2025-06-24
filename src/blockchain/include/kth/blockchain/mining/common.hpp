// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_MINING_COMMON_HPP_
#define KTH_BLOCKCHAIN_MINING_COMMON_HPP_

#include <unordered_set>
#include <vector>

#include <kth/domain/constants.hpp>

// #ifndef KTH_MINING_DONT_INCLUDE_CTOR
// #define KTH_MINING_CTOR_ENABLED
// #endif

#if defined(KTH_CURRENCY_BCH)
#define KTH_WITNESS_DEFAULT false
#else
#define KTH_WITNESS_DEFAULT true
#endif

namespace kth::mining {

#if defined(KTH_CURRENCY_BCH)
constexpr size_t min_transaction_size_for_capacity = min_transaction_size;
#else
 //TODO(fernando): check if it is possible to be a TX size less than this...
constexpr size_t min_transaction_size_for_capacity = 61;
#endif  //KTH_CURRENCY_BCH

//TODO(fernando): put in a common file
using index_t = size_t;
using measurements_t = double;
using removal_list_t = std::unordered_set<index_t>;
using indexes_t = std::vector<index_t>;

constexpr index_t null_index = max_size_t;

}  // namespace kth::mining

#endif  //KTH_BLOCKCHAIN_MINING_COMMON_HPP_
