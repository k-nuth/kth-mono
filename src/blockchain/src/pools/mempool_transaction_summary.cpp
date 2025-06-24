/**
 * Copyright (c) 2011-2018 libitcoin developers (see AUTHORS)
 *
 * This file is part of the Knuth Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <kth/blockchain/pools/mempool_transaction_summary.hpp>

namespace kth::blockchain {

mempool_transaction_summary::mempool_transaction_summary(std::string const& address, std::string const& hash, std::string const& previous_output_hash,
                        std::string const& previous_output_index, std::string const& satoshis, uint64_t index,
                        uint64_t timestamp)
    : address_(address),
      hash_(hash),
      previous_output_hash_(previous_output_hash),
      previous_output_index_(previous_output_index),
      satoshis_(satoshis),
      index_(index),
      timestamp_(timestamp)
{
}

std::string mempool_transaction_summary::address() const {
    return address_;
}

std::string mempool_transaction_summary::hash() const {
    return hash_;
}

std::string mempool_transaction_summary::previous_output_hash() const {
    return previous_output_hash_;
}

std::string mempool_transaction_summary::previous_output_index() const {
    return previous_output_index_;
}

std::string mempool_transaction_summary::satoshis() const {
    return satoshis_;
}

uint64_t mempool_transaction_summary::index() const {
    return index_;
}

uint64_t mempool_transaction_summary::timestamp() const {
    return timestamp_;
}

} // namespace kth::blockchain
