// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_WALLET_SELECT_OUTPUTS_HPP
#define KTH_WALLET_SELECT_OUTPUTS_HPP

#include <cstdint>

#include <kth/domain/chain/points_value.hpp>
#include <kth/domain/define.hpp>

namespace kth::domain::wallet {

struct KD_API select_outputs {
    enum class algorithm {
        /// The smallest single sufficient unspent output, if one exists, or a
        /// sufficient set of unspent outputs, if such a set exists. The set is
        /// minimal by number of outputs but not necessarily by total value.
        greedy,

        /// A set of individually sufficient unspent outputs. Each individual
        /// member of the set is sufficient. Return ascending order by value.
        individual
    };

    /// Select outpoints for a spend from a list of unspent outputs.
    static
    void select(chain::points_value& out,
                       const chain::points_value& unspent,
                       uint64_t minimum_value,
                       algorithm option = algorithm::greedy);

private:
    static
    void greedy(chain::points_value& out,
                       const chain::points_value& unspent,
                       uint64_t minimum_value);

    static
    void individual(chain::points_value& out,
                           const chain::points_value& unspent,
                           uint64_t minimum_value);
};

} // namespace kth::domain::wallet

#endif
