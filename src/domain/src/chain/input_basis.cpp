// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/input_basis.hpp>

#include <algorithm>
#include <sstream>

#include <kth/domain/chain/script.hpp>

#if defined(KTH_SEGWIT_ENABLED)
#include <kth/domain/chain/witness.hpp>
#endif

#include <kth/domain/common.hpp>
#include <kth/domain/constants.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::chain {

using namespace kth::domain::machine;

// Constructors.
//-----------------------------------------------------------------------------

input_basis::input_basis(output_point&& previous_output, chain::script&& script, uint32_t sequence)
    : previous_output_(std::move(previous_output)),
      script_(std::move(script)),
      sequence_(sequence) {}

input_basis::input_basis(output_point const& previous_output, chain::script const& script, uint32_t sequence)
    : previous_output_(previous_output),
      script_(script),
      sequence_(sequence) {}

#if defined(KTH_SEGWIT_ENABLED)
input_basis::input_basis(output_point const& previous_output, chain::script const& script, chain::witness const& witness, uint32_t sequence)
    : previous_output_(previous_output)
    , script_(script)
    , witness_(witness)
    , sequence_(sequence)
{}

input_basis::input_basis(output_point&& previous_output, chain::script&& script, chain::witness&& witness, uint32_t sequence)
    : previous_output_(std::move(previous_output))
    , script_(std::move(script))
    , witness_(std::move(witness))
    , sequence_(sequence)
{}
#endif


// Operators.
//-----------------------------------------------------------------------------

bool input_basis::operator==(input_basis const& x) const {
    return (sequence_ == x.sequence_)
        && (previous_output_ == x.previous_output_)
        && (script_ == x.script_)
#if defined(KTH_SEGWIT_ENABLED)
        && (witness_ == x.witness_)
#endif
        ;
}

bool input_basis::operator!=(input_basis const& x) const {
    return !(*this == x);
}

void input_basis::reset() {
    previous_output_.reset();
    script_.reset();
#if defined(KTH_SEGWIT_ENABLED)
    witness_.reset();
#endif
    sequence_ = 0;
}

// Since empty scripts and zero sequence are valid this relies on the prevout.
bool input_basis::is_valid() const {
    return sequence_ != 0
        || previous_output_.is_valid()
        || script_.is_valid()
#if defined(KTH_SEGWIT_ENABLED)
        || witness_.is_valid()
#endif
        ;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk input_basis::to_data(bool wire, bool witness) const {
    data_chunk data;
    auto const size = serialized_size(wire, witness_val(witness));
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, wire, witness_val(witness));
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void input_basis::to_data(data_sink& stream, bool wire, bool witness) const {
    ostream_writer sink_w(stream);
    to_data(sink_w, wire, witness_val(witness));
}

// Size.
//-----------------------------------------------------------------------------
size_t input_basis::serialized_size_non_witness(bool wire) const {
    return previous_output_.serialized_size(wire)
         + script_.serialized_size(true)
         + sizeof(sequence_);
}


#if ! defined(KTH_SEGWIT_ENABLED)
size_t input_basis::serialized_size(bool wire, bool /*witness*/) const {
    return serialized_size_non_witness(wire);
}
#else
size_t input_basis::serialized_size(bool wire, bool witness) const {
    // Always write witness to store so that we know how to read it.
    witness |= !wire;

    // Witness size added in both contexts despite that tx writes wire witness.
    // Prefix is written for both wire and store/other contexts.
    return serialized_size_non_witness(wire)
         + (witness ? witness_.serialized_size(true) : 0);
}
#endif


// Accessors.
//-----------------------------------------------------------------------------

output_point& input_basis::previous_output() {
    return previous_output_;
}

output_point const& input_basis::previous_output() const {
    return previous_output_;
}

void input_basis::set_previous_output(output_point const& value) {
    previous_output_ = value;
}

void input_basis::set_previous_output(output_point&& value) {
    previous_output_ = std::move(value);
}

chain::script& input_basis::script() {
    return script_;
}

chain::script const& input_basis::script() const {
    return script_;
}

void input_basis::set_script(chain::script const& value) {
    script_ = value;
}

void input_basis::set_script(chain::script&& value) {
    script_ = std::move(value);
}

#if defined(KTH_SEGWIT_ENABLED)
chain::witness const& input_basis::witness() const {
    return witness_;
}

chain::witness& input_basis::witness() {
    return witness_;
}

void input_basis::set_witness(chain::witness const& value) {
    witness_ = value;
}

void input_basis::set_witness(chain::witness&& value) {
    witness_ = std::move(value);
}
#endif // KTH_CURRENCY_BCH


uint32_t input_basis::sequence() const {
    return sequence_;
}

void input_basis::set_sequence(uint32_t value) {
    sequence_ = value;
}


// Utilities.
//-----------------------------------------------------------------------------

#if defined(KTH_SEGWIT_ENABLED)
void input_basis::strip_witness() {
    witness_.clear();
}
#endif

// Validation helpers.
//-----------------------------------------------------------------------------

bool input_basis::is_final() const {
    return sequence_ == max_input_sequence;
}

#if defined(KTH_SEGWIT_ENABLED)
bool input_basis::is_segregated() const {
    // If no block tx is has witness data the commitment is optional (bip141).
    return !witness_.empty();
}
#endif

bool input_basis::is_locked(size_t block_height, uint32_t median_time_past) const {
    if ((sequence_ & relative_locktime_disabled) != 0) {
        return false;
    }

    // bip68: a minimum block-height constraint over the input's age.
    auto const minimum = (sequence_ & relative_locktime_mask);
    auto const& prevout = previous_output_.validation;

    if ((sequence_ & relative_locktime_time_locked) != 0) {
        // Median time past must be monotonically-increasing by block.
        KTH_ASSERT(median_time_past >= prevout.median_time_past);
        auto const age_seconds = median_time_past - prevout.median_time_past;
        return age_seconds < (minimum << relative_locktime_seconds_shift);
    }

    KTH_ASSERT(block_height >= prevout.height);
    auto const age_blocks = block_height - prevout.height;
    return age_blocks < minimum;
}

// This requires that previous outputs have been populated.
// This cannot overflow because each total is limited by max ops.
size_t input_basis::signature_operations(bool bip16, bool bip141) const {
    auto const& prevout = previous_output_.validation.cache.script();

#if defined(KTH_SEGWIT_ENABLED)
    // Penalize quadratic signature operations (bip141).
    size_t const sigops_factor = bip141 ? fast_sigops_factor : 1U;
#else
    size_t const sigops_factor = 1U;
#endif

    // Count heavy sigops in the input script.
    auto sigops = script_.sigops(false) * sigops_factor;

#if defined(KTH_SEGWIT_ENABLED)
    chain::script witness;
    if (bip141 && witness_.extract_sigop_script(witness, prevout)) {
        // Add sigops in the witness (bip141).
        return sigops + witness.sigops(true);
    }
#endif

    chain::script embedded;
    if (bip16 && extract_embedded_script(embedded)) {
#if defined(KTH_SEGWIT_ENABLED)
        if (bip141 && witness_.extract_sigop_script(witness, embedded)) {
            // Add sigops in the embedded witness (bip141).
            return sigops + witness.sigops(true);
        }
#endif
        // Add heavy sigops in the embedded script (bip16).
        return sigops + embedded.sigops(true) * sigops_factor;
    }

    return sigops;
}

// This requires that previous outputs have been populated.
bool input_basis::extract_embedded_script(chain::script& out) const {
    ////KTH_ASSERT(previous_output_.is_valid());
    auto const& ops = script_.operations();
    auto const& prevout_script = previous_output_.validation.cache.script();

    // There are no embedded sigops when the prevout script is not p2sh.
    if ( ! prevout_script.is_pay_to_script_hash(rule_fork::bip16_rule)) {
        return false;
    }

    // There are no embedded sigops when the input script is not push only.
    // The first operations access must be method-based to guarantee the cache.
    if (ops.empty() || !script::is_relaxed_push(ops)) {
        return false;
    }

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    return entity_from_data(out, ops.back().data(), false);
}

#if defined(KTH_SEGWIT_ENABLED)
bool input_basis::extract_reserved_hash(hash_digest& out) const {
    auto const& stack = witness_.stack();

    if ( ! witness::is_reserved_pattern(stack)) {
        return false;
    }

    std::copy_n(stack.front().begin(), hash_size, out.begin());
    return true;
}
#endif // KTH_CURRENCY_BCH

} // namespace kth::domain::chain
