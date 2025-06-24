// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/output_basis.hpp>

#include <algorithm>
#include <cstdint>
#include <sstream>

#include <kth/domain/constants.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::chain {

// Constructors.
//-----------------------------------------------------------------------------

output_basis::output_basis(uint64_t value, chain::script const& script, token_data_opt const& token_data)
    : value_(value)
    , script_(script)
    , token_data_(token_data)
{}

output_basis::output_basis(uint64_t value, chain::script&& script, token_data_opt&& token_data)
    : value_(value)
    , script_(std::move(script))
    , token_data_(std::move(token_data))
{}

//-----------------------------------------------------------------------------

// protected
void output_basis::reset() {
    value_ = output_basis::not_found;
    script_.reset();
    token_data_.reset();
}

// Empty scripts are valid, validation relies on not_found only.
bool output_basis::is_valid() const {
    return value_ != output_basis::not_found &&
           chain::is_valid(token_data_);
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk output_basis::to_data(bool wire) const {
    data_chunk data;
    auto const size = serialized_size(wire);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, wire);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void output_basis::to_data(data_sink& stream, bool wire) const {
    ostream_writer sink_w(stream);
    to_data(sink_w, wire);
}

// Size.
//-----------------------------------------------------------------------------

size_t output_basis::serialized_size(bool /*wire*/) const {
    return sizeof(value_) +
           script_.serialized_size(true) +
           chain::encoding::serialized_size(token_data_) +
           size_t(token_data_.has_value());
}

// Accessors.
//-----------------------------------------------------------------------------

uint64_t output_basis::value() const {
    return value_;
}

void output_basis::set_value(uint64_t x) {
    value_ = x;
}

chain::script& output_basis::script() {
    return script_;
}

chain::script const& output_basis::script() const {
    return script_;
}

void output_basis::set_script(chain::script const& x) {
    script_ = x;
}

void output_basis::set_script(chain::script&& x) {
    script_ = std::move(x);
}

chain::token_data_opt const& output_basis::token_data() const {
    return token_data_;
}

void output_basis::set_token_data(chain::token_data_opt const& x) {
    token_data_ = x;
}

void output_basis::set_token_data(chain::token_data_opt&& x) {
    token_data_ = std::move(x);
}

// Validation helpers.
//-----------------------------------------------------------------------------

size_t output_basis::signature_operations(bool bip141) const {
#if defined(KTH_SEGWIT_ENABLED)
    // Penalize quadratic signature operations (bip141).
    auto const sigops_factor = bip141 ? fast_sigops_factor : 1U;
   // Count heavy sigops in the output script.
    return script_.sigops(false) * sigops_factor;
#else
    return script_.sigops(false);
#endif
}

bool output_basis::is_dust(uint64_t minimum_output_value) const {
    // If provably unspendable it does not expand the unspent output set.
    return value_ < minimum_output_value && !script_.is_unspendable();
}

#if defined(KTH_SEGWIT_ENABLED)
bool output_basis::extract_committed_hash(hash_digest& out) const {
    auto const& ops = script_.operations();

    if ( ! script::is_commitment_pattern(ops)) {
        return false;
    }

    // The four byte offset for the witness commitment hash (bip141).
    auto const start = ops[1].data().begin() + sizeof(witness_head);
    std::copy_n(start, hash_size, out.begin());
    return true;
}
#endif

} // namespace kth::domain::chain
