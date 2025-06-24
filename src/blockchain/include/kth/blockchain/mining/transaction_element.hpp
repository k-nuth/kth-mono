// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_MINING_TRANSACTION_ELEMENT_HPP_
#define KTH_BLOCKCHAIN_MINING_TRANSACTION_ELEMENT_HPP_

#include <unordered_map>
#include <vector>

#include <kth/domain.hpp>

namespace kth {
namespace mining {

class transaction_element {
public:

    transaction_element(hash_digest const& txid,
#if ! defined(KTH_CURRENCY_BCH)
                        hash_digest const& hash,
#endif
                        data_chunk const& raw,
                        uint64_t fee,
                        size_t sigops,
                        uint32_t output_count
                        // std::vector<domain::chain::point> const& previous_outputs
                        )
        : txid_(txid)
#if ! defined(KTH_CURRENCY_BCH)
        , hash_(hash)
#endif
        , raw_(raw)
        , fee_(fee)
        , sigops_(sigops)
        , output_count_(output_count)
        // , previous_outputs_(previous_outputs) {}

    transaction_element(hash_digest const& txid,
#if ! defined(KTH_CURRENCY_BCH)
                        hash_digest const& hash,
#endif
                        data_chunk&& raw,
                        uint64_t fee,
                        size_t sigops,
                        uint32_t output_count
                        // std::vector<domain::chain::point>&& previous_outputs
                        )
        : txid_(txid)
#if ! defined(KTH_CURRENCY_BCH)
        , hash_(hash)
#endif
        , raw_(std::move(raw))
        , fee_(fee)
        , sigops_(sigops)
        , output_count_(output_count)
        // , previous_outputs_(std::move(previous_outputs)) {}

    hash_digest const& txid() const {
        return txid_;
    }

#if ! defined(KTH_CURRENCY_BCH)
    hash_digest const& hash() const {
        return hash_;
    }
#endif

    data_chunk const& raw() const {
        return raw_;
    }

    uint64_t fee() const {
        return fee_;
    }

    size_t sigops() const {
        return sigops_;
    }

    size_t size() const {
        return raw_.size();
    }

    //TODO(fernando): move to node class
    uint32_t output_count() const {
        return output_count_;
    }

    // std::vector<domain::chain::point> const& previous_outputs() const {
    //     return previous_outputs_;
    // }
private:
    hash_digest txid_;

#if ! defined(KTH_CURRENCY_BCH)
    hash_digest hash_;
#endif

    data_chunk raw_;
    uint64_t fee_;
    size_t sigops_;
    uint32_t output_count_;
    // std::vector<domain::chain::point> previous_outputs_;
};

}  // namespace mining
}  // namespace kth

#endif  //KTH_BLOCKCHAIN_MINING_TRANSACTION_ELEMENT_HPP_
