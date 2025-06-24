// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/script.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <utility>

#include <boost/range/adaptor/reversed.hpp>

#include <kth/domain/chain/transaction.hpp>
#include <kth/domain/chain/witness.hpp>
#include <kth/domain/constants.hpp>
#include <kth/domain/machine/interpreter.hpp>
#include <kth/domain/machine/operation.hpp>
#include <kth/domain/machine/program.hpp>
// #include <kth/infrastructure/message/message_tools.hpp>
#include <kth/domain/machine/opcode.hpp>
#include <kth/domain/machine/rule_fork.hpp>
#include <kth/domain/multi_crypto_support.hpp>
#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/formats/base_16.hpp>
#include <kth/infrastructure/machine/script_pattern.hpp>
#include <kth/infrastructure/machine/script_version.hpp>
#include <kth/infrastructure/machine/sighash_algorithm.hpp>
#include <kth/infrastructure/math/elliptic_curve.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/message/message_tools.hpp>
#include <kth/infrastructure/utility/assert.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>
#include <kth/infrastructure/utility/string.hpp>

using namespace kth::domain::machine;
using namespace kth::infrastructure::machine;
using namespace boost::adaptors;

namespace kth::domain::chain {

// bit.ly/2cPazSa
static
auto const one_hash = hash_literal("0000000000000000000000000000000000000000000000000000000000000001"); //NOLINT

// Constructors.
//-----------------------------------------------------------------------------

script::script(operation::list const& ops) {
    from_operations(ops);
}

script::script(operation::list&& ops) {
    from_operations(std::move(ops));
}

script::script(data_chunk&& encoded, bool prefix)
    : script_basis(std::move(encoded), prefix)
{}

script::script(data_chunk const& encoded, bool prefix)
    : script_basis(encoded, prefix)
{}

script::script(script&& x) noexcept
    : script_basis(std::move(x))
{}

script::script(script const& x)
    : script_basis(x)
{}

// Concurrent read/write is not supported, so no critical section.
script& script::operator=(script&& x) noexcept {
    // TODO(legacy): implement safe private accessor for conditional cache transfer.
    reset();
    script_basis::operator=(std::move(x));
    return *this;
}

// Concurrent read/write is not supported, so no critical section.
script& script::operator=(script const& x) {
    // TODO(legacy): implement safe private accessor for conditional cache transfer.
    reset();
    script_basis::operator=(x);
    return *this;
}
// Operators.
//-----------------------------------------------------------------------------

// bool script::operator==(script const& x) const {
//     return bytes_ == x.bytes_;
// }

// bool script::operator!=(script const& x) const {
//     return !(*this == x);
// }

// Deserialization.
//-----------------------------------------------------------------------------

// Concurrent read/write is not supported, so no critical section.
bool script::from_string(std::string const& mnemonic) {
    reset();

    // There is strictly one operation per string token.
    auto const tokens = split(mnemonic);
    operation::list ops;
    ops.resize(tokens.empty() || tokens.front().empty() ? 0 : tokens.size());

    // Create an op list from the split tokens, one operation per token.
    for (size_t index = 0; index < ops.size(); ++index) {
        if ( ! ops[index].from_string(tokens[index])) {
            return false;
        }
    }

    from_operations(ops);
    return true;
}

// Concurrent read/write is not supported, so no critical section.
void script::from_operations(operation::list&& ops) {
    script_basis::from_operations(ops);
    operations_ = std::move(ops);
    cached_ = true;
}

// Concurrent read/write is not supported, so no critical section.
void script::from_operations(operation::list const& ops) {
    script_basis::from_operations(ops);
    operations_ = ops;
    cached_ = true;
}

// protected
// Concurrent read/write is not supported, so no critical section.
void script::reset() {
    script_basis::reset();
    cached_ = false;
    operations_.clear();
    operations_.shrink_to_fit();
}

bool script::is_valid_operations() const {
    // Script validity is independent of individual operation validity.
    // There is a trailing invalid/default op if a push op had a size mismatch.
    return operations().empty() || operations_.back().is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

std::string script::to_string(uint32_t active_forks) const {
    auto first = true;
    std::ostringstream text;

    for (auto const& op : operations()) {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

// Iteration.
//-----------------------------------------------------------------------------
// These are syntactic sugar that allow the caller to iterate ops directly.
// The first operations access must be method-based to guarantee the cache.

void script::clear() {
    reset();
}

bool script::empty() const {
    return operations().empty();
}

size_t script::size() const {
    return operations().size();
}

operation const& script::front() const {
    KTH_ASSERT( ! operations().empty());
    return operations().front();
}

operation const& script::back() const {
    KTH_ASSERT( ! operations().empty());
    return operations().back();
}

operation const& script::operator[](size_t index) const {
    KTH_ASSERT(index < operations().size());
    return operations()[index];
}

operation::iterator script::begin() const {
    return operations().begin();
}

operation::iterator script::end() const {
    return operations().end();
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

// // protected
// operation::list const& script::operations() const {
//     ///////////////////////////////////////////////////////////////////////////
//     // Critical Section
//     mutex_.lock_upgrade();

//     if (cached_) {
//         mutex_.unlock_upgrade();
//         //---------------------------------------------------------------------
//         return operations_;
//     }

//     // operation op;
//     data_source istream(bytes_);
//     istream_reader stream_r(istream);
//     auto const size = bytes_.size();

//     //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     mutex_.unlock_upgrade_and_lock();

//     // One operation per byte is the upper limit of operations.
//     operations_.reserve(size);

//     // ************************************************************************
//     // CONSENSUS: In the case of a coinbase script we must parse the entire
//     // script, beyond just the BIP34 requirements, so that sigops can be
//     // calculated from the script. These are counted despite being irrelevant.
//     // In this case an invalid script is parsed to the extent possible.
//     // ************************************************************************

//     // If an op fails it is pushed to operations and the loop terminates.
//     // To validate the ops the caller must test the last op.is_valid(), or may
//     // text script.is_valid_operations(), which is done in script validation.
//     while ( ! stream_r.is_exhausted()) {
//         // op.from_data(stream_r);
//         // operations_.push_back(std::move(op));
//         operations_.push_back(create<operation>(stream_r));
//     }

//     operations_.shrink_to_fit();
//     cached_ = true;

//     mutex_.unlock();
//     ///////////////////////////////////////////////////////////////////////////

//     return operations_;
// }


// protected
operation::list const& script::operations() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (cached_) {
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return operations_;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    mutex_.unlock_upgrade_and_lock();

    operations_ = chain::operations(*this);
    cached_ = true;

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////
#else
    {
        std::shared_lock lock(mutex_);
        if (cached_) {
            return operations_;
        }
    }
    std::unique_lock lock(mutex_);
    if ( ! cached_) {
        operations_ = chain::operations(*this);
        cached_ = true;
    }
#endif
    return operations_;
}

// Signing (unversioned).
//-----------------------------------------------------------------------------

inline
hash_digest signature_hash(transaction const& tx, uint32_t sighash_type) {
    // There is no rational interpretation of a signature hash for a coinbase.
    KTH_ASSERT( ! tx.is_coinbase());

    auto serialized = tx.to_data(true, false);
    extend_data(serialized, to_little_endian(sighash_type));
    return bitcoin_hash(serialized);
}

//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline
sighash_algorithm to_sighash_enum(uint8_t sighash_type) {
    switch (sighash_type & sighash_algorithm::mask) {
        case sighash_algorithm::single:
            return sighash_algorithm::single;
        case sighash_algorithm::none:
            return sighash_algorithm::none;
        default:
            return sighash_algorithm::all;
    }
}

inline
uint8_t is_sighash_enum(uint8_t sighash_type, sighash_algorithm value) {
    return static_cast<uint8_t>(
        to_sighash_enum(sighash_type) == value
    );
}

static
hash_digest sign_none(transaction const& tx, uint32_t input_index, script const& script_code, uint8_t sighash_type) {
    input::list ins;
    auto const& inputs = tx.inputs();
    auto const any = (sighash_type & sighash_algorithm::anyone_can_pay) != 0;
    ins.reserve(any ? 1 : inputs.size());

    KTH_ASSERT(input_index < inputs.size());
    auto const& self = inputs[input_index];

    if (any) {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    } else {
        // Erase all input scripts and sequences.
        for (auto const& input : inputs) {
            ins.emplace_back(input.previous_output(), script{}, 0);
        }

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ins[input_index].set_sequence(self.sequence());
    }

    // Move new inputs to new transaction and drop outputs.
    return signature_hash({tx.version(), tx.locktime(), std::move(ins), {}}, sighash_type);
}

static
hash_digest sign_single(transaction const& tx, uint32_t input_index, script const& script_code, uint8_t sighash_type) {
    input::list ins;
    auto const& inputs = tx.inputs();
    auto const any = (sighash_type & sighash_algorithm::anyone_can_pay) != 0;
    ins.reserve(any ? 1 : inputs.size());

    KTH_ASSERT(input_index < inputs.size());
    auto const& self = inputs[input_index];

    if (any) {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    } else {
        // Erase all input scripts and sequences.
        for (auto const& input : inputs) {
            ins.emplace_back(input.previous_output(), script{}, 0);
        }

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ins[input_index].set_sequence(self.sequence());
    }

    // Trim and clear outputs except that of specified input index.
    auto const& outputs = tx.outputs();
    output::list outs(input_index + 1);

    KTH_ASSERT(input_index < outputs.size());
    outs.back() = outputs[input_index];

    // Move new inputs and new outputs to new transaction.
    return signature_hash({tx.version(), tx.locktime(), std::move(ins),
                           std::move(outs)},
                          sighash_type);
}

static
hash_digest sign_all(transaction const& tx, uint32_t input_index, script const& script_code, uint8_t sighash_type) {
    input::list ins;
    auto const& inputs = tx.inputs();
    auto const any = (sighash_type & sighash_algorithm::anyone_can_pay) != 0;
    ins.reserve(any ? 1 : inputs.size());

    KTH_ASSERT(input_index < inputs.size());
    auto const& self = inputs[input_index];

    if (any) {
        // Retain only self.
        ins.emplace_back(self.previous_output(), script_code, self.sequence());
    } else {
        // Erase all input scripts.
        for (auto const& input : inputs) {
            ins.emplace_back(input.previous_output(), script{},
                             input.sequence());
        }

        // Replace self that is lost in the loop.
        ins[input_index].set_script(script_code);
        ////ins[input_index].set_sequence(self.sequence());
    }

    // Move new inputs and copy outputs to new transaction.
    transaction out(tx.version(), tx.locktime(), input::list{}, tx.outputs());
    out.set_inputs(std::move(ins));
    return signature_hash(out, sighash_type);
}

static
script strip_code_seperators(script const& script_code) {
    operation::list ops;

     for (auto const& op : script_code) {
        if (op.code() != opcode::codeseparator) {
            ops.push_back(op);
        }
    }

    return script(std::move(ops));
}

// private/static
hash_digest script::generate_unversioned_signature_hash(transaction const& tx,
                                                        uint32_t input_index,
                                                        script const& script_code,
                                                        uint8_t sighash_type) {
    auto const sighash = to_sighash_enum(sighash_type);
    if (input_index >= tx.inputs().size() ||
        (input_index >= tx.outputs().size() && sighash == sighash_algorithm::single)) {
        //*********************************************************************
        // CONSENSUS: wacky satoshi behavior.
        //*********************************************************************
        return one_hash;
    }

    //*************************************************************************
    // CONSENSUS: more wacky satoshi behavior.
    //*************************************************************************
    auto const stripped = strip_code_seperators(script_code);

    // The sighash serializations are isolated for clarity and optimization.
    switch (sighash) {
        case sighash_algorithm::none:
            return sign_none(tx, input_index, stripped, sighash_type);
        case sighash_algorithm::single:
            return sign_single(tx, input_index, stripped, sighash_type);
        default:
        case sighash_algorithm::all:
            return sign_all(tx, input_index, stripped, sighash_type);
    }
}

// Signing (version 0).
//-----------------------------------------------------------------------------

// hash_digest script::to_outputs(transaction const& tx) {
//     auto const sum = [&](size_t total, output const& output) {
//         return total + output.serialized_size();
//     };

//     auto const& outs = tx.outputs();
//     auto size = std::accumulate(outs.begin(), outs.end(), size_t(0), sum);
//     data_chunk data;
//     data.reserve(size);
//     data_sink ostream(data);
//     ostream_writer sink_w(ostream);

//     auto const write = [&](output const& output) {
//         output.to_data(sink_w, true);
//     };

//     std::for_each(outs.begin(), outs.end(), write);
//     ostream.flush();
//     KTH_ASSERT(data.size() == size);
//     return bitcoin_hash(data);
// }

// hash_digest script::to_inpoints(transaction const& tx) {
//     auto const sum = [&](size_t total, input const& input) {
//         return total + input.previous_output().serialized_size();
//     };

//     auto const& ins = tx.inputs();
//     auto size = std::accumulate(ins.begin(), ins.end(), size_t(0), sum);
//     data_chunk data;
//     data.reserve(size);
//     data_sink ostream(data);
//     ostream_writer sink_w(ostream);

//     auto const write = [&](input const& input) {
//         input.previous_output().to_data(sink_w);
//     };

//     std::for_each(ins.begin(), ins.end(), write);
//     ostream.flush();
//     KTH_ASSERT(data.size() == size);
//     return bitcoin_hash(data);
// }

// hash_digest script::to_sequences(transaction const& tx) {
//     auto const sum = [&](size_t total, input const& input) {
//         return total + sizeof(uint32_t);
//     };

//     auto const& ins = tx.inputs();
//     auto size = std::accumulate(ins.begin(), ins.end(), size_t(0), sum);
//     data_chunk data;
//     data.reserve(size);
//     data_sink ostream(data);
//     ostream_writer sink_w(ostream);

//     auto const write = [&](input const& input) {
//         sink_w.write_4_bytes_little_endian(input.sequence());
//     };

//     std::for_each(ins.begin(), ins.end(), write);
//     ostream.flush();
//     KTH_ASSERT(data.size() == size);
//     return bitcoin_hash(data);
// }

static
size_t preimage_size(size_t script_size) {
    return sizeof(uint32_t) + hash_size + hash_size + point::satoshi_fixed_size() + script_size + sizeof(uint64_t) + sizeof(uint32_t) + hash_size + sizeof(uint32_t) + sizeof(uint32_t);
}

// private/static
hash_digest script::generate_version_0_signature_hash(transaction const& tx,
                                                      uint32_t input_index,
                                                      script const& script_code,
                                                      uint64_t value,
                                                      uint8_t sighash_type
                                                      ) {
    // Unlike unversioned algorithm this does not allow an invalid input index.
    KTH_ASSERT(input_index < tx.inputs().size());
    auto const& input = tx.inputs()[input_index];
    auto const size = preimage_size(script_code.serialized_size(true));

    data_chunk data;
    data.reserve(size);
    data_sink ostream(data);
    ostream_writer sink_w(ostream);

    // Flags derived from the signature hash byte.
    auto const sighash = to_sighash_enum(sighash_type);
    auto const any = (sighash_type & sighash_algorithm::anyone_can_pay) != 0;

#if defined(KTH_CURRENCY_BCH)
    auto const single = (sighash == sighash_algorithm::single || sighash == sighash_algorithm::cash_forkid_all);

    //Note(kth: Not used for the moment:
    // auto const none = (sighash == sighash_algorithm::none || sighash == sighash_algorithm::cash_forkid_all);

    auto const all = (sighash == sighash_algorithm::all || sighash == sighash_algorithm::cash_forkid_all);
#else
    auto const single = (sighash == sighash_algorithm::single);

    //Note(kth: Not used for the moment:
    // auto const none = (sighash == sighash_algorithm::none);

    auto const all = (sighash == sighash_algorithm::all);
#endif

    // 1. transaction version (4-byte little endian).
    sink_w.write_little_endian(tx.version());

    // 2. inpoints hash (32-byte hash).
    sink_w.write_hash( ! any ? tx.inpoints_hash() : null_hash);

    // 3. sequences hash (32-byte hash).
    sink_w.write_hash( ! any && all ? tx.sequences_hash() : null_hash);

    // 4. outpoint (32-byte hash + 4-byte little endian).
    input.previous_output().to_data(sink_w);

    // 5. script of the input (with prefix).
    script_code.to_data(sink_w, true);

    // 6. value of the output spent by this input (8-byte little endian).
    sink_w.write_little_endian(value);

    // 7. sequence of the input (4-byte little endian).
    sink_w.write_little_endian(input.sequence());

    // 8. outputs hash (32-byte hash).
    sink_w.write_hash(all ? tx.outputs_hash() : (single && input_index < tx.outputs().size() ? bitcoin_hash(tx.outputs()[input_index].to_data()) : null_hash));

    // 9. transaction locktime (4-byte little endian).
    sink_w.write_little_endian(tx.locktime());

    // 10. sighash type of the signature (4-byte [not 1] little endian).
    sink_w.write_4_bytes_little_endian(sighash_type);

    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return bitcoin_hash(data);
}

// Signing (common).
//-----------------------------------------------------------------------------

// static
hash_digest script::generate_signature_hash(transaction const& tx,
                                            uint32_t input_index,
                                            script const& script_code,
                                            uint8_t sighash_type,
                                            script_version version,
                                            uint64_t value) {
    // The way of serialization is changed (bip143).
    switch (version) {
        case script_version::unversioned:
            return generate_unversioned_signature_hash(tx, input_index, script_code, sighash_type);
        case script_version::zero:
            return generate_version_0_signature_hash(tx, input_index, script_code, value, sighash_type);
        case script_version::reserved:
        default:
            KTH_ASSERT_MSG(false, "invalid script version");
            return {};
    }
}

// static
bool script::check_signature(ec_signature const& signature,
                             uint8_t sighash_type,
                             data_chunk const& public_key,
                             script const& script_code,
                             transaction const& tx,
                             uint32_t input_index,
                             script_version version,
                             uint64_t value) {
    if (public_key.empty()) {
        return false;
    }

    // This always produces a valid signature hash, including one_hash.
    auto const sighash = chain::script::generate_signature_hash(tx,
                                                                input_index, script_code, sighash_type, version, value);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

// static
bool script::create_endorsement(endorsement& out, ec_secret const& secret, script const& prevout_script, transaction const& tx, uint32_t input_index, uint8_t sighash_type, script_version version, uint64_t value) {
    out.reserve(max_endorsement_size);

    // This always produces a valid signature hash, including one_hash.
    auto const sighash = chain::script::generate_signature_hash(tx, input_index, prevout_script, sighash_type, version, value);

    // Create the EC signature and encode as DER.
    ec_signature signature;
    if ( ! sign(signature, secret, sighash) || !encode_signature(out, signature)) {
        return false;
    }

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(sighash_type);
    out.shrink_to_fit();
    return true;
}

// Utilities (static).
//-----------------------------------------------------------------------------

bool script::is_push_only(operation::list const& ops) {
    auto const push = [](operation const& op) {
        return op.is_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_relaxed_push(operation::list const& ops) {
    auto const push = [&](operation const& op) {
        return op.is_relaxed_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: BIP34 requires coinbase input script to begin with one byte that
// indicates the height size. This is inconsistent with an extreme future where
// the size byte overflows. However satoshi actually requires nominal encoding.
//*****************************************************************************
bool script::is_coinbase_pattern(operation::list const& ops, size_t height) {
    if (ops.empty()) return false;

    //Note(kth): Bitcoin core and derivatives do not follow the BIP34 specification.
    //  https://github.com/bitcoin/bitcoin/pull/14633
    if (height <= 16) {
        static constexpr auto op_1 = static_cast<uint8_t>(opcode::push_positive_1);
        auto const op_0 = static_cast<uint8_t>(ops[0].code());
        if (op_0 < op_1) return false;
        return height == op_0 - op_1 + 1;
    }

    return ops[0].is_nominal_push() && ops[0].data() == number(height).data();
}

//*****************************************************************************
// CONSENSUS: this pattern is used to commit to bip141 witness data.
//*****************************************************************************
#if defined(KTH_SEGWIT_ENABLED)
bool script::is_commitment_pattern(operation::list const& ops) {
    static auto const header = to_big_endian(witness_head);

    // Bytes after commitment are optional with no consensus meaning (bip141).
    // Commitment is not executable so invalid trailing operations are allowed.
    return ops.size() > 1 && ops[0].code() == opcode::return_ && ops[1].code() == opcode::push_size_36 && std::equal(header.begin(), header.end(), ops[1].data().begin());
}

//*****************************************************************************
// CONSENSUS: this pattern is used in bip141 validation rules.
//*****************************************************************************
bool script::is_witness_program_pattern(operation::list const& ops) {
    return ops.size() == 2 && ops[0].is_version() && ops[1].data().size() >= min_witness_program && ops[1].data().size() <= max_witness_program;
}
#endif

// The satoshi client tests for 83 bytes total. This allows for the waste of
// one byte to represent up to 75 bytes using the push_one_size opcode.
// It also allows any number of push ops and limits it to 0 value and 1 per tx.
////bool script::is_null_data_pattern(operation::list const& ops)
////{
////    static constexpr auto op_76 = static_cast<uint8_t>(opcode::push_one_size);
////
////    return ops.size() >= 2
////        && ops[0].code() == opcode::return_
////        && static_cast<uint8_t>(ops[1].code()) <= op_76
////        && ops[1].data().size() <= max_null_data_size;
////}

// The satoshi client enables configurable data size for policy.
bool script::is_null_data_pattern(operation::list const& ops) {
    return ops.size() == 2 && ops[0].code() == opcode::return_ && ops[1].is_minimal_push() && ops[1].data().size() <= max_null_data_size;
}

// TODO(legacy): expand this to the 20 signature op_check_multisig limit.
// The current 16 (or 20) limit does not affect server indexing because bare
// multisig is not indexable and p2sh multisig is byte-limited to 15 sigs.
// The satoshi client policy limit is 3 signatures for bare multisig.
bool script::is_pay_multisig_pattern(operation::list const& ops) {
    static constexpr auto op_1 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_16 = static_cast<uint8_t>(opcode::push_positive_16);

    auto const op_count = ops.size();

    if (op_count < 4 || ops[op_count - 1].code() != opcode::checkmultisig) {
        return false;
    }

    auto const op_m = static_cast<uint8_t>(ops[0].code());
    auto const op_n = static_cast<uint8_t>(ops[op_count - 2].code());

    if (op_m < op_1 || op_m > op_n || op_n < op_1 || op_n > op_16) {
        return false;
    }

    auto const number = op_n - op_1 + 1u;
    auto const points = op_count - 3u;

    if (number != points) {
        return false;
    }

    for (auto op = ops.begin() + 1; op != ops.end() - 2; ++op) {
        if ( ! is_public_key(op->data())) {
            return false;
        }
    }

    return true;
}

// The satoshi client considers this non-standard for policy.
bool script::is_pay_public_key_pattern(operation::list const& ops) {
    return ops.size() == 2 && is_public_key(ops[0].data()) && ops[1].code() == opcode::checksig;
}

bool script::is_pay_key_hash_pattern(operation::list const& ops) {
    return ops.size() == 5 && ops[0].code() == opcode::dup && ops[1].code() == opcode::hash160 && ops[2].data().size() == short_hash_size && ops[3].code() == opcode::equalverify && ops[4].code() == opcode::checksig;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_pay_script_hash_pattern(operation::list const& ops) {
    return ops.size() == 3 && ops[0].code() == opcode::hash160 && ops[1].code() == opcode::push_size_20 && ops[2].code() == opcode::equal;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_pay_witness_script_hash_pattern(operation::list const& ops) {
    return ops.size() == 2 && ops[0].code() == opcode::push_size_0 && ops[1].code() == opcode::push_size_32;
}

// The first push is based on wacky satoshi op_check_multisig behavior that
// we must perpetuate, though it's appearance here is policy not consensus.
// Limiting to push_size_0 eliminates pattern ambiguity with little downside.
bool script::is_sign_multisig_pattern(operation::list const& ops) {
    return ops.size() >= 2 && ops[0].code() == opcode::push_size_0 && std::all_of(ops.begin() + 1, ops.end(), [](operation const& op) { return is_endorsement(op.data()); });
}

bool script::is_sign_public_key_pattern(operation::list const& ops) {
    return ops.size() == 1 && is_endorsement(ops[0].data());
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_sign_key_hash_pattern(operation::list const& ops) {
    return ops.size() == 2 && is_endorsement(ops[0].data()) && is_public_key(ops[1].data());
}

// Ambiguous with is_sign_key_hash when second/last op is a public key.
// Ambiguous with is_sign_public_key_pattern when only op is an endorsement.
bool script::is_sign_script_hash_pattern(operation::list const& ops) {
    return !ops.empty() && is_push_only(ops) && !ops.back().data().empty();
}

operation::list script::to_null_data_pattern(data_slice data) {
    if (data.size() > max_null_data_size) {
        return {};
    }

    return operation::list{{opcode::return_},
                           {to_chunk(data)}};
}

operation::list script::to_pay_public_key_pattern(data_slice point) {
    if ( ! is_public_key(point)) {
        return {};
    }

    return operation::list{{to_chunk(point)},
                           {opcode::checksig}};
}

operation::list script::to_pay_key_hash_pattern(short_hash const& hash) {
    return operation::list{
        {opcode::dup},
        {opcode::hash160},
        {to_chunk(hash)},
        {opcode::equalverify},
        {opcode::checksig}};
}

operation::list script::to_pay_script_hash_pattern(short_hash const& hash) {
    return operation::list{
        {opcode::hash160},
        {to_chunk(hash)},
        {opcode::equal}};
}

operation::list script::to_pay_multisig_pattern(uint8_t signatures,
                                                point_list const& points) {
    data_stack chunks;
    chunks.reserve(points.size());
    auto const conversion = [&chunks](ec_compressed const& point) {
        chunks.push_back(to_chunk(point));
    };

    // Operation ordering matters, don't use std::transform here.
    std::for_each(points.begin(), points.end(), conversion);
    return to_pay_multisig_pattern(signatures, chunks);
}

// TODO(legacy): expand this to a 20 signature limit.
// This supports up to 16 signatures, however check_multisig is limited to 20.
// The embedded script is limited to 520 bytes, an effective limit of 15 for
// p2sh multisig, which can be as low as 7 when using all uncompressed keys.
operation::list script::to_pay_multisig_pattern(uint8_t signatures, data_stack const& points) {
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    static constexpr auto zero = op_81 - 1;
    static constexpr auto max = op_96 - zero;

    auto const m = signatures;
    auto const n = points.size();

    if (m < 1 || m > n || n < 1 || n > max) {
        return operation::list();
    }

    auto const op_m = static_cast<opcode>(m + zero);
    auto const op_n = static_cast<opcode>(points.size() + zero);

    operation::list ops;
    ops.reserve(points.size() + 3);
    ops.emplace_back(op_m);

    for (auto const& point : points) {
        if ( ! is_public_key(point)) {
            return {};
        }

        ops.emplace_back(point);
    }

    ops.emplace_back(op_n);
    ops.emplace_back(opcode::checkmultisig);
    return ops;
}

// Utilities (non-static).
//-----------------------------------------------------------------------------

#if defined(KTH_SEGWIT_ENABLED)
data_chunk script::witness_program() const {
    // The first operations access must be method-based to guarantee the cache.
    auto const& ops = operations();
    return is_witness_program_pattern(ops) ? ops[1].data() : data_chunk{};
}
#endif

script_version script::version() const {
    // The first operations access must be method-based to guarantee the cache.
    auto const& ops = operations();

#if defined(KTH_SEGWIT_ENABLED)
    if ( ! is_witness_program_pattern(ops)) {
        return script_version::unversioned;
    }
#endif

    // Version 0 is specified, others are reserved (bip141).
    return (ops[0].code() == opcode::push_size_0) ? script_version::zero : script_version::reserved;
}

// Caller should test for is_sign_script_hash_pattern when sign_key_hash result
// as it is possible for an input script to match both patterns.
script_pattern script::pattern() const {
    auto const input = output_pattern();
    return input == script_pattern::non_standard ? input_pattern() : input;
}

// Output patterns are mutually and input unambiguous.
// The bip141 coinbase pattern is not tested here, must test independently.
script_pattern script::output_pattern() const {
    // The first operations access must be method-based to guarantee the cache.
    if (is_pay_key_hash_pattern(operations())) {
        return script_pattern::pay_key_hash;
    }

    if (is_pay_script_hash_pattern(operations_)) {
        return script_pattern::pay_script_hash;
    }

    if (is_null_data_pattern(operations_)) {
        return script_pattern::null_data;
    }

    if (is_pay_public_key_pattern(operations_)) {
        return script_pattern::pay_public_key;
    }

    if (is_pay_multisig_pattern(operations_)) {
        return script_pattern::pay_multisig;
    }

    return script_pattern::non_standard;
}

// A sign_key_hash result always implies sign_script_hash as well.
// The bip34 coinbase pattern is not tested here, must test independently.
script_pattern script::input_pattern() const {
    // The first operations access must be method-based to guarantee the cache.
    if (is_sign_key_hash_pattern(operations())) {
        return script_pattern::sign_key_hash;
    }

    // This must follow is_sign_key_hash_pattern for ambiguity comment to hold.
    if (is_sign_script_hash_pattern(operations_)) {
        return script_pattern::sign_script_hash;
    }

    if (is_sign_public_key_pattern(operations_)) {
        return script_pattern::sign_public_key;
    }

    if (is_sign_multisig_pattern(operations_)) {
        return script_pattern::sign_multisig;
    }

    return script_pattern::non_standard;
}

#if defined(KTH_SEGWIT_ENABLED)
bool script::is_pay_to_witness(uint32_t forks) const {
#if ! defined(KTH_SEGWIT_ENABLED)
    (void)forks;    //Note(kth): to mute the Linter
    return false;
#else
    // This is used internally as an optimization over using script::pattern.
    // The first operations access must be method-based to guarantee the cache.
    return is_enabled(forks, rule_fork::bip141_rule) &&
           is_witness_program_pattern(operations());
#endif
}
#endif

bool script::is_pay_to_script_hash(uint32_t forks) const {
    // This is used internally as an optimization over using script::pattern.
    // The first operations access must be method-based to guarantee the cache.
    return is_enabled(forks, rule_fork::bip16_rule) &&
           is_pay_script_hash_pattern(operations());
}

// Count 1..16 multisig accurately for embedded (bip16) and witness (bip141).
inline size_t multisig_sigops(bool accurate, opcode code) {
    return accurate && operation::is_positive(code) ? operation::opcode_to_positive(code) : multisig_default_sigops;
}

size_t script::sigops(bool accurate) const {
    size_t total = 0;
    auto preceding = opcode::reserved_255;

    // The first operations access must be method-based to guarantee the cache.
    for (auto const& op : operations()) {
        auto const code = op.code();

        if (code == opcode::checksig || code == opcode::checksigverify) {
            ++total;
        } else if (code == opcode::checkmultisig || code == opcode::checkmultisigverify) {
            total += multisig_sigops(accurate, preceding);
        }

        preceding = code;
    }

    return total;
}

// Concurrent read/write is not supported, so no critical section.
void script::find_and_delete(data_stack const& endorsements) {
    script_basis::find_and_delete(endorsements);

    // Invalidate the cache so that the operations may be regenerated.
    operations_.clear();
    cached_ = false;
}

////// This is slightly more efficient because the script does not get parsed,
////// but the static template implementation is more self-explanatory.
////bool script::is_coinbase_pattern(size_t height) const
////{
////    auto const actual = to_data(false);
////
////    // Create the expected script as a non-minimal byte vector.
////    script compare(operation::list{ { number(height).data(), false } });
////    auto const expected = compare.to_data(false);
////
////    // Require the actual script start with the expected coinbase script.
////    return std::equal(expected.begin(), expected.end(), actual.begin());
////}

// An unspendable script is any that can provably not be spent under any
// circumstance. This allows for exclusion of the output as unspendable.
// The criteria below are not be comprehensive but are fast to evaluate.
bool script::is_unspendable() const {
    // The first operations access must be method-based to guarantee the cache.
    return ( ! operations().empty() && operations_[0].code() == opcode::return_) || serialized_size(false) > max_script_size;
}

// Validation.
//-----------------------------------------------------------------------------

// #if ! defined(KTH_SEGWIT_ENABLED)
// code script::verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, script const& prevout_script, uint64_t /*value*/) {
// #else
// code script::verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, witness const& input_witness, script const& prevout_script, uint64_t value) {
// #endif
//     code ec;

//     // Evaluate input script.
//     program input(input_script, tx, input_index, forks);
//     if ((ec = input.evaluate())) {
//         return ec;
//     }

//     // Evaluate output script using stack result from input script.
//     program prevout(prevout_script, input);
//     if ((ec = prevout.evaluate())) {
//         return ec;
//     }

//     // This precludes bare witness programs of -0 (undocumented).
//     if ( ! prevout.stack_result(false)) {
//         return error::stack_false;
//     }

// #if defined(KTH_SEGWIT_ENABLED)
//     bool witnessed;
//     // Triggered by output script push of version and witness program (bip141).
//     if ((witnessed = prevout_script.is_pay_to_witness(forks))) {
//         // The input script must be empty (bip141).
//         if ( ! input_script.empty()) {
//             return error::dirty_witness;
//         }

//         // This is a valid witness script so validate it.
//         if ((ec = input_witness.verify(tx, input_index, forks, prevout_script, value))) {
//             return ec;
//         }
//     } else
// #endif
//     // p2sh and p2w are mutually exclusive.
//     /*else*/
//     if (prevout_script.is_pay_to_script_hash(forks)) {
//         if ( ! is_relaxed_push(input_script.operations())) {
//             return error::invalid_script_embed;
//         }

//         // Embedded script must be at the top of the stack (bip16).
//         script embedded_script(input.pop(), false);

//         program embedded(embedded_script, std::move(input), true);
//         if ((ec = embedded.evaluate())) {
//             return ec;
//         }

//         // This precludes embedded witness programs of -0 (undocumented).
//         if ( ! embedded.stack_result(false)) {
//             return error::stack_false;
//         }

// #if defined(KTH_SEGWIT_ENABLED)
//         // Triggered by embedded push of version and witness program (bip141).
//         if ((witnessed = embedded_script.is_pay_to_witness(forks))) {
//             // The input script must be a push of the embedded_script (bip141).
//             if (input_script.size() != 1) {
//                 return error::dirty_witness;
//             }

//             // This is a valid embedded witness script so validate it.
//             if ((ec = input_witness.verify(tx, input_index, forks, embedded_script, value))) {
//                 return ec;
//             }
//         }
// #endif
//     }

// #if defined(KTH_SEGWIT_ENABLED)
//     // Witness must be empty if no bip141 or valid witness program (bip141).
//     if ( ! witnessed && !input_witness.empty()) {
//         return error::unexpected_witness;
//     }
// #endif

//     return error::success;
// }

// code script::verify(transaction const& tx, uint32_t input, uint32_t forks) {
//     if (input >= tx.inputs().size()) {
//         return error::operation_failed;
//     }

//     auto const& in = tx.inputs()[input];
//     auto const& prevout = in.previous_output().validation.cache;

// #if ! defined(KTH_SEGWIT_ENABLED)
//     return verify(tx, input, forks, in.script(), prevout.script(), prevout.value());
// #else
//     return verify(tx, input, forks, in.script(), in.witness(), prevout.script(), prevout.value());
// #endif
// }

} // namespace kth::domain::chain
