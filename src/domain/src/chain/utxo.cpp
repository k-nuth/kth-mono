// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/utxo.hpp>

namespace kth::domain::chain {

utxo::utxo(output_point const& point, uint64_t amount, std::optional<token_data_t> token_data)
    : point_(point), amount_(amount), token_data_(token_data)
{}

utxo::utxo(output_point&& point, uint64_t amount, std::optional<token_data_t> token_data)
    : point_(std::move(point)), amount_(amount), token_data_(token_data)
{}


// Getters
//-----------------------------------------------------------------------------

uint32_t utxo::height() const {
    return height_;
}

output_point& utxo::point() {
    return point_;
}

output_point const& utxo::point() const {
    return point_;
}

std::optional<token_data_t>& utxo::token_data() {
    return token_data_;
}

std::optional<token_data_t> const& utxo::token_data() const {
    return token_data_;
}

uint64_t utxo::amount() const {
    return amount_;
}

// Setters
//-----------------------------------------------------------------------------
void utxo::set_height(uint32_t height) {
    height_ = height;
}

void utxo::set_point(output_point const& point) {
    point_ = point;
}

void utxo::set_amount(uint64_t amount) {
    amount_ = amount;
}

void utxo::set_token_data(std::optional<token_data_t> token_data) {
    token_data_ = token_data;
}

} // namespace kth::domain::chain