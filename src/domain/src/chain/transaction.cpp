// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/transaction.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

#include <kth/domain/chain/chain_state.hpp>
#include <kth/domain/chain/input.hpp>
#include <kth/domain/chain/output.hpp>
#include <kth/domain/chain/script.hpp>
#include <kth/domain/constants.hpp>
#include <kth/domain/machine/opcode.hpp>
#include <kth/domain/machine/operation.hpp>
#include <kth/domain/machine/program.hpp>
#include <kth/domain/machine/rule_fork.hpp>
#include <kth/domain/multi_crypto_support.hpp>
#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/message/message_tools.hpp>
#include <kth/infrastructure/utility/collection.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/endian.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/limits.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

// using namespace kth::domain::machine;
using namespace kth::infrastructure::machine;

namespace kth::domain::chain {

// Constructors.
//-----------------------------------------------------------------------------

transaction::transaction()
    : validation{}
{}

transaction::transaction(uint32_t version, uint32_t locktime, input::list const& inputs, output::list const& outputs
#ifdef KTH_CACHED_RPC_DATA
                        , uint32_t cached_sigops, uint64_t cached_fees, bool cached_is_standard
#endif
                        )
    : transaction_basis(version, locktime, inputs, outputs)
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(cached_fees)
    , cached_sigops_(cached_sigops)
    , cached_is_standard_(cached_is_standard)
#endif
    , validation{}
{}

transaction::transaction(uint32_t version, uint32_t locktime, input::list&& inputs, output::list&& outputs
#ifdef KTH_CACHED_RPC_DATA
                       , uint32_t cached_sigops, uint64_t cached_fees, bool cached_is_standard
#endif
                        )
    : transaction_basis(version, locktime, std::move(inputs), std::move(outputs))
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(cached_fees)
    , cached_sigops_(cached_sigops)
    , cached_is_standard_(cached_is_standard)
#endif
    , validation{}
{}

transaction::transaction(transaction const& x, hash_digest const& hash)
    : transaction_basis(x)
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(x.cached_fees_)
    , cached_sigops_(x.cached_sigops_)
    , cached_is_standard_(x.cached_is_standard_)
#endif
    , validation(x.validation)
{
    hash_ = std::make_shared<hash_digest>(hash);
    // validation = x.validation;
}

transaction::transaction(transaction&& x, hash_digest const& hash)
    : transaction_basis(std::move(x))
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(x.cached_fees_)
    , cached_sigops_(x.cached_sigops_)
    , cached_is_standard_(x.cached_is_standard_)
#endif
    , validation(std::move(x.validation))
{
    hash_ = std::make_shared<hash_digest>(hash);
    // validation = std::move(x.validation);
}

transaction::transaction(transaction const& x)
    : transaction_basis(x)
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(0)
    , cached_sigops_(0)
    , cached_is_standard_(false)
#endif
    , validation(x.validation)
{}

transaction::transaction(transaction&& x) noexcept
    : transaction_basis(std::move(x))
#ifdef KTH_CACHED_RPC_DATA
    , cached_fees_(0)
    , cached_sigops_(0)
    , cached_is_standard_(false)
#endif
    , validation(std::move(x.validation))
{}

transaction& transaction::operator=(transaction const& x) {
    transaction_basis::operator=(x);
    validation = x.validation;

#ifdef KTH_CACHED_RPC_DATA
    cached_fees_ = x.cached_fees_;
    cached_sigops_ = x.cached_sigops_;
    cached_is_standard_ = x.cached_is_standard_;
#endif

    return *this;
}

transaction& transaction::operator=(transaction&& x) noexcept {
    transaction_basis::operator=(std::move(static_cast<transaction_basis&&>(x)));

    validation = std::move(x.validation);
#ifdef KTH_CACHED_RPC_DATA
    cached_fees_ = x.cached_fees_;
    cached_sigops_ = x.cached_sigops_;
    cached_is_standard_ = x.cached_is_standard_;
#endif
    return *this;
}

// protected
void transaction::reset() {
    transaction_basis::reset();
    invalidate_cache();
    outputs_hash_.reset();
    inpoints_hash_.reset();
    sequences_hash_.reset();
    segregated_ = std::nullopt;
    total_input_value_ = std::nullopt;
    total_output_value_ = std::nullopt;
}

// Serialization.
//-----------------------------------------------------------------------------

// Transactions with empty witnesses always use old serialization (bip144).
// If no inputs are witness programs then witness hash is tx hash (bip141).
data_chunk transaction::to_data(bool wire, bool witness
#ifdef KTH_CACHED_RPC_DATA
    , bool unconfirmed
#endif
    ) const {
#if defined(KTH_SEGWIT_ENABLED)
    // Witness handling must be disabled for non-segregated txs.
    witness = witness && is_segregated();
#endif

    data_chunk data;
    auto const size = serialized_size(wire, witness_val(witness)
#ifdef KTH_CACHED_RPC_DATA
                                     , unconfirmed
#endif
                                     );

    // Reserve an extra byte to prevent full reallocation in the case of
    // generate_signature_hash extension by addition of the sighash_type.
    data.reserve(size + sizeof(uint8_t));

    data_sink ostream(data);
    to_data(ostream, wire, witness_val(witness)
#ifdef KTH_CACHED_RPC_DATA
           , unconfirmed
#endif
           );

    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void transaction::to_data(data_sink& stream, bool wire, bool witness
#ifdef KTH_CACHED_RPC_DATA
    , bool unconfirmed
#endif
    ) const {
#if defined(KTH_SEGWIT_ENABLED)
    // Witness handling must be disabled for non-segregated txs.
    witness = witness && is_segregated();
#endif

    ostream_writer sink_w(stream);
    to_data(sink_w, wire, witness_val(witness)
#ifdef KTH_CACHED_RPC_DATA
           , unconfirmed
#endif
           );
}

// Size.
//-----------------------------------------------------------------------------

size_t transaction::serialized_size(bool wire, bool witness
#ifdef KTH_CACHED_RPC_DATA
    , bool unconfirmed
#endif
    ) const {

#if defined(KTH_SEGWIT_ENABLED)
    // Witness hashing must be disabled for non-segregated txs.
    witness = witness && is_segregated();
#endif


    // Must be both witness and wire encoding for bip144 serialization.
    return transaction_basis::serialized_size(wire, witness)
#ifdef KTH_CACHED_RPC_DATA
         + (( ! wire && unconfirmed) ? sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint8_t) : 0)
#endif
         ;
}

// Accessors.
//-----------------------------------------------------------------------------

void transaction::set_version(uint32_t value) {
    transaction_basis::set_version(value);
    invalidate_cache();
}

void transaction::set_locktime(uint32_t value) {
    transaction_basis::set_locktime(value);
    invalidate_cache();
}

void transaction::set_inputs(input::list const& value) {
    transaction_basis::set_inputs(value);
    invalidate_cache();
    inpoints_hash_.reset();
    sequences_hash_.reset();
    segregated_ = std::nullopt;
    total_input_value_ = std::nullopt;
}

void transaction::set_inputs(input::list&& value) {
    transaction_basis::set_inputs(std::move(value));
    invalidate_cache();
    segregated_ = std::nullopt;
    total_input_value_ = std::nullopt;
}

void transaction::set_outputs(output::list const& value) {
    transaction_basis::set_outputs(value);
    invalidate_cache();
    outputs_hash_.reset();
    total_output_value_ = std::nullopt;
}

void transaction::set_outputs(output::list&& value) {
    transaction_basis::set_outputs(std::move(value));
    invalidate_cache();
    total_output_value_ = std::nullopt;
}

#ifdef KTH_CACHED_RPC_DATA
uint64_t transaction::cached_fees() const {
    return cached_fees_;
}

uint32_t transaction::cached_sigops() const {
    return cached_sigops_;
}

bool transaction::cached_is_standard() const {
    return cached_is_standard_;
}
#endif // KTH_CACHED_RPC_DATA

// Cache.
//-----------------------------------------------------------------------------

// protected
void transaction::invalidate_cache() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    hash_mutex_.lock_upgrade();

    if (hash_ || witness_hash_) {
        hash_mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_.reset();
        witness_hash_.reset();
        //---------------------------------------------------------------------
        hash_mutex_.unlock_and_lock_upgrade();
    }

    hash_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
#else
    {
        std::shared_lock lock(hash_mutex_);
        if ( ! hash_ && ! witness_hash_) {
            return;
        }
    }

    std::unique_lock lock(hash_mutex_);
    hash_.reset();
    witness_hash_.reset();
#endif
}

hash_digest transaction::hash(bool witness) const {
#if defined(KTH_SEGWIT_ENABLED)
    // Witness hashing must be disabled for non-segregated txs.
    witness = witness && is_segregated();
#endif
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    hash_mutex_.lock_upgrade(); //TODO(fernando): use RAII

    if (witness_val(witness)) {
#if defined(KTH_SEGWIT_ENABLED)
        if ( ! witness_hash_) {
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            hash_mutex_.unlock_upgrade_and_lock(); //TODO(fernando): use RAII
            witness_hash_ = std::make_shared<hash_digest>(hash_witness(*this));
            hash_mutex_.unlock_and_lock_upgrade();
            //-----------------------------------------------------------------
        }
#endif
    } else {
        if ( ! hash_) {
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            hash_mutex_.unlock_upgrade_and_lock(); //TODO(fernando): use RAII
            hash_ = std::make_shared<hash_digest>(hash_non_witness(*this));
            hash_mutex_.unlock_and_lock_upgrade();
            //-----------------------------------------------------------------
        }
    }

    auto const hash = witness_val(witness) ? *witness_hash_ : *hash_;
    hash_mutex_.unlock_upgrade();
///////////////////////////////////////////////////////////////////////////
    return hash;
#else
    {
        std::shared_lock lock(hash_mutex_);

        if (witness_val(witness)) {
#if defined(KTH_SEGWIT_ENABLED)
            if (witness_hash_) {
                return *witness_hash_;
            }
#endif
        } else {
            if (hash_) {
                return *hash_;
            }
        }
    }

    std::unique_lock lock(hash_mutex_);

    if (witness_val(witness)) {
#if defined(KTH_SEGWIT_ENABLED)
        if ( ! witness_hash_) {
            witness_hash_ = std::make_shared<hash_digest>(hash_witness(*this));
        }
        return *witness_hash_;
#endif
    }
    if ( ! hash_) {
        hash_ = std::make_shared<hash_digest>(hash_non_witness(*this));
    }
    return *hash_;
#endif
}

hash_digest transaction::outputs_hash() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    hash_mutex_.lock_upgrade();

    if ( ! outputs_hash_) {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_mutex_.unlock_upgrade_and_lock();
        outputs_hash_ = std::make_shared<hash_digest>(to_outputs(*this));
        hash_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    auto const hash = *outputs_hash_;
    hash_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
#else
    {
        std::shared_lock lock(hash_mutex_);
        if (outputs_hash_) {
            return *outputs_hash_;
        }
    }
    std::unique_lock lock(hash_mutex_);
    if ( ! outputs_hash_) {
        outputs_hash_ = std::make_shared<hash_digest>(to_outputs(*this));
    }
    return *outputs_hash_;

#endif
}

hash_digest transaction::inpoints_hash() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    hash_mutex_.lock_upgrade();

    if ( ! inpoints_hash_) {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_mutex_.unlock_upgrade_and_lock();
        inpoints_hash_ = std::make_shared<hash_digest>(to_inpoints(*this));
        hash_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    auto const hash = *inpoints_hash_;
    hash_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
#else
    {
        std::shared_lock lock(hash_mutex_);
        if (inpoints_hash_) {
            return *inpoints_hash_;
        }
    }
    std::unique_lock lock(hash_mutex_);
    if ( ! inpoints_hash_) {
        inpoints_hash_ = std::make_shared<hash_digest>(to_inpoints(*this));
    }
    return *inpoints_hash_;
#endif
}

hash_digest transaction::sequences_hash() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    hash_mutex_.lock_upgrade();

    if ( ! sequences_hash_) {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        hash_mutex_.unlock_upgrade_and_lock();
        sequences_hash_ = std::make_shared<hash_digest>(to_sequences(*this));
        hash_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    auto const hash = *sequences_hash_;
    hash_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return hash;
#else
    {
        std::shared_lock lock(hash_mutex_);
        if (sequences_hash_) {
            return *sequences_hash_;
        }
    }
    std::unique_lock lock(hash_mutex_);
    if ( ! sequences_hash_) {
        sequences_hash_ = std::make_shared<hash_digest>(to_sequences(*this));
    }
    return *sequences_hash_;
#endif
}

// Utilities.
//-----------------------------------------------------------------------------

#if defined(KTH_SEGWIT_ENABLED)
// Clear witness from all inputs (does not change default transaction hash).
void transaction::strip_witness() {
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    segregated_ = false;
    transaction_basis::strip_witness();
    ///////////////////////////////////////////////////////////////////////////
}
#endif

void transaction::recompute_hash() {
    hash_ = nullptr;
    hash();
}

// Validation helpers.
//-----------------------------------------------------------------------------

// Returns max_uint64 in case of overflow.
uint64_t transaction::total_input_value() const {

#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (total_input_value_ != std::nullopt) {
        auto const value = total_input_value_.value();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    auto const value = chain::total_input_value(*this);
    total_input_value_ = value;
    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
#else
    {
        std::shared_lock lock(mutex_);
        if (total_input_value_ != std::nullopt) {
            return total_input_value_.value();
        }
    }
    std::unique_lock lock(mutex_);
    if (total_input_value_ == std::nullopt) {
        total_input_value_ = chain::total_input_value(*this);
    }
    return total_input_value_.value();
#endif
}

// Returns max_uint64 in case of overflow.
uint64_t transaction::total_output_value() const {

#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (total_output_value_ != std::nullopt) {
        auto const value = total_output_value_.value();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    auto const value = chain::total_output_value(*this);
    total_output_value_ = value;
    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
#else
    {
        std::shared_lock lock(mutex_);
        if (total_output_value_ != std::nullopt) {
            return total_output_value_.value();
        }
    }

    std::unique_lock lock(mutex_);
    if (total_output_value_ == std::nullopt) {
        total_output_value_ = chain::total_output_value(*this);
    }
    return total_output_value_.value();
#endif
}

uint64_t transaction::fees() const {
    return floor_subtract(total_input_value(), total_output_value());
}

bool transaction::is_overspent() const {
    return !is_coinbase() && total_output_value() > total_input_value();
}

// Returns max_size_t in case of overflow.
size_t transaction::signature_operations() const {
    auto const state = validation.state;
    auto const bip16 = state ? state->is_enabled(kth::domain::machine::rule_fork::bip16_rule) : true;
#if ! defined(KTH_SEGWIT_ENABLED)
    auto const bip141 = false;
#else
    auto const bip141 = state ? state->is_enabled(kth::domain::machine::rule_fork::bip141_rule) : false;
#endif
    return transaction_basis::signature_operations(bip16, bip141);
}

// size_t transaction::weight() const {
//     // Block weight is 3 * Base size * + 1 * Total size (bip141).
//     return base_size_contribution * serialized_size(true, false) +
//            total_size_contribution * serialized_size(true, true);
// }

#if defined(KTH_SEGWIT_ENABLED)

bool transaction::is_segregated() const {
#if ! defined(KTH_SEGWIT_ENABLED)
    return false;
#endif
    bool value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (segregated_ != std::nullopt) {
        value = segregated_.value();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // If no block tx is has witness data the commitment is optional (bip141).
    value = chain::is_segregated(*this);
    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

#endif

// Coinbase transactions return success, to simplify iteration.
code transaction::connect_input(chain_state const& state, size_t input_index) const {
    if (input_index >= inputs().size()) {
        return error::operation_failed;
    }

    if (is_coinbase()) {
        return error::success;
    }

    auto const& prevout = inputs()[input_index].previous_output().validation;

    // Verify that the previous output cache has been populated.
    if ( ! prevout.cache.is_valid()) {
        return error::missing_previous_output;
    }

    auto const forks = state.enabled_forks();
    auto const index32 = static_cast<uint32_t>(input_index);

    // Verify the transaction input script against the previous output.
    // return script::verify(*this, index32, forks);
    return verify(*this, index32, forks);
}

// Validation.
//-----------------------------------------------------------------------------

// These checks are self-contained; blockchain (and so version) independent.
code transaction::check(size_t max_block_size, bool transaction_pool, bool retarget /* = true */) const {
    return transaction_basis::check(total_output_value(), max_block_size, transaction_pool, retarget);
}

code transaction::accept(bool transaction_pool) const {
    auto const state = validation.state;
    return state ? accept(*state, transaction_pool) : error::operation_failed;
}

// These checks assume that prevout caching is completed on all tx.inputs.
code transaction::accept(chain_state const& state, bool transaction_pool) const {
#if defined(KTH_SEGWIT_ENABLED)
    return transaction_basis::accept(state, is_segregated(), is_overspent(), validation.duplicate, transaction_pool);
#else
    return transaction_basis::accept(state, false, is_overspent(), validation.duplicate, transaction_pool);
#endif
}

code transaction::connect() const {
    auto const state = validation.state;
    return state ? connect(*state) : error::operation_failed;
}

code transaction::connect(chain_state const& state) const {
    code ec;

    for (size_t input = 0; input < inputs().size(); ++input) {
        if ((ec = connect_input(state, input))) {
            return ec;
        }
    }

    return error::success;
}

#if ! defined(KTH_SEGWIT_ENABLED)
code verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, script const& prevout_script, uint64_t /*value*/) {
#else
code verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, witness const& input_witness, script const& prevout_script, uint64_t value) {
#endif
    using machine::program;
    code ec;

    // Evaluate input script.
    program input(input_script, tx, input_index, forks);
    if ((ec = input.evaluate())) {
        return ec;
    }

    // Evaluate output script using stack result from input script.
    program prevout(prevout_script, input);
    if ((ec = prevout.evaluate())) {
        return ec;
    }

    // This precludes bare witness programs of -0 (undocumented).
    if ( ! prevout.stack_result(false)) {
        return error::stack_false;
    }

#if defined(KTH_SEGWIT_ENABLED)
    bool witnessed;
    // Triggered by output script push of version and witness program (bip141).
    if ((witnessed = prevout_script.is_pay_to_witness(forks))) {
        // The input script must be empty (bip141).
        if ( ! input_script.empty()) {
            return error::dirty_witness;
        }

        // This is a valid witness script so validate it.
        if ((ec = input_witness.verify(tx, input_index, forks, prevout_script, value))) {
            return ec;
        }
    } else
#endif
    // p2sh and p2w are mutually exclusive.
    /*else*/
    if (prevout_script.is_pay_to_script_hash(forks)) {
        if ( ! script::is_relaxed_push(input_script.operations())) {
            return error::invalid_script_embed;
        }

        // Embedded script must be at the top of the stack (bip16).
        script embedded_script(input.pop(), false);

        program embedded(embedded_script, std::move(input), true);
        if ((ec = embedded.evaluate())) {
            return ec;
        }

        // This precludes embedded witness programs of -0 (undocumented).
        if ( ! embedded.stack_result(false)) {
            return error::stack_false;
        }

#if defined(KTH_SEGWIT_ENABLED)
        // Triggered by embedded push of version and witness program (bip141).
        if ((witnessed = embedded_script.is_pay_to_witness(forks))) {
            // The input script must be a push of the embedded_script (bip141).
            if (input_script.size() != 1) {
                return error::dirty_witness;
            }

            // This is a valid embedded witness script so validate it.
            if ((ec = input_witness.verify(tx, input_index, forks, embedded_script, value))) {
                return ec;
            }
        }
#endif
    }

#if defined(KTH_SEGWIT_ENABLED)
    // Witness must be empty if no bip141 or valid witness program (bip141).
    if ( ! witnessed && !input_witness.empty()) {
        return error::unexpected_witness;
    }
#endif

    return error::success;
}

code verify(transaction const& tx, uint32_t input, uint32_t forks) {
    if (input >= tx.inputs().size()) {
        return error::operation_failed;
    }

    auto const& in = tx.inputs()[input];
    auto const& prevout = in.previous_output().validation.cache;

#if ! defined(KTH_SEGWIT_ENABLED)
    return verify(tx, input, forks, in.script(), prevout.script(), prevout.value());
#else
    return verify(tx, input, forks, in.script(), in.witness(), prevout.script(), prevout.value());
#endif
}

} // namespace kth::domain::chain
