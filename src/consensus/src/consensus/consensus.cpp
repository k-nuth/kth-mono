// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "consensus/consensus.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string.h>
#include <kth/consensus/define.hpp>
#include <kth/consensus/export.hpp>
#include <kth/consensus/version.hpp>
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/interpreter.h"
#include "script/script_error.h"
#include "streams.h"
#include "version.h"

#if defined(KTH_CURRENCY_BCH)
#include "script/script_metrics.h"
#endif

namespace kth::consensus {

// Initialize libsecp256k1 context.
static auto secp256k1_context = ECCVerifyHandle();

// Helper class, not published. This is tested internal to verify_script.
class transaction_istream {
public:
    template<typename Type>
    transaction_istream& operator>>(Type& instance) {
        ::Unserialize(*this, instance);
        return *this;
    }

    transaction_istream(uint8_t const* transaction, size_t size)
        : source_(transaction), remaining_(size) {}

    void read(char* destination, size_t size) {
        if (size > remaining_) {
            throw std::ios_base::failure("end of data");
        }

        memcpy(destination, source_, size);
        remaining_ -= size;
        source_ += size;
    }

    int GetType() const {
        return SER_NETWORK;
    }

    int GetVersion() const {
        return PROTOCOL_VERSION;
    }

private:
    size_t remaining_;
    uint8_t const* source_;
};

// This mapping decouples the consensus API from the satoshi implementation
// files. We prefer to keep our copies of consensus files isomorphic.
// This function is not published (but non-static for testability).

// enum class ScriptError {
//     UNKNOWN,
//     OP_RETURN,

//     /* Max sizes */
//     SCRIPT_SIZE,
//     PUSH_SIZE,
//     OP_COUNT,
//     STACK_SIZE,
//     SIG_COUNT,
//     PUBKEY_COUNT,
//     INPUT_SIGCHECKS,

//     /* Operands checks */
//     INVALID_OPERAND_SIZE,
//     INVALID_NUMBER_RANGE,
//     IMPOSSIBLE_ENCODING,
//     INVALID_SPLIT_RANGE,
//     INVALID_BIT_COUNT,

//     /* Failed verify operations */
//     VERIFY,
//     EQUALVERIFY,
//     CHECKMULTISIGVERIFY,
//     CHECKSIGVERIFY,
//     CHECKDATASIGVERIFY,
//     NUMEQUALVERIFY,

//     /* Logical/Format/Canonical errors */
//     BAD_OPCODE,
//     DISABLED_OPCODE,
//     INVALID_STACK_OPERATION,
//     INVALID_ALTSTACK_OPERATION,
//     UNBALANCED_CONDITIONAL,

//     /* Divisor errors */
//     DIV_BY_ZERO,
//     MOD_BY_ZERO,

//     /* Bitfield errors */
//     INVALID_BITFIELD_SIZE,
//     INVALID_BIT_RANGE,

//     /* CHECKLOCKTIMEVERIFY and CHECKSEQUENCEVERIFY */
//     NEGATIVE_LOCKTIME,
//     UNSATISFIED_LOCKTIME,

//     /* Malleability */
//     SIG_HASHTYPE,
//     SIG_DER,
//     MINIMALDATA,
//     SIG_PUSHONLY,
//     SIG_HIGH_S,
//     PUBKEYTYPE,
//     CLEANSTACK,
//     MINIMALIF,
//     SIG_NULLFAIL,

//     /* Schnorr */
//     SIG_BADLENGTH,
//     SIG_NONSCHNORR,

//     /* softfork safeness */
//     DISCOURAGE_UPGRADABLE_NOPS,

//     /* anti replay */
//     ILLEGAL_FORKID,
//     MUST_USE_FORKID,

//     /* Auxiliary errors (unused by interpreter) */
//     SIGCHECKS_LIMIT_EXCEEDED,

//     ERROR_COUNT,
// };

#if defined(KTH_CURRENCY_BCH)
verify_result_type script_error_to_verify_result(ScriptError code) {
    switch (code) {
        // Logical result
        case ScriptError::OK:
            return verify_result_eval_true;
        case ScriptError::EVAL_FALSE:
            return verify_result_eval_false;

        // Max size errors
        case ScriptError::SCRIPT_SIZE:
            return verify_result_script_size;
        case ScriptError::PUSH_SIZE:
            return verify_result_push_size;
        case ScriptError::OP_COUNT:
            return verify_result_op_count;
        case ScriptError::STACK_SIZE:
            return verify_result_stack_size;
        case ScriptError::SIG_COUNT:
            return verify_result_sig_count;
        case ScriptError::PUBKEY_COUNT:
            return verify_result_pubkey_count;

        // Failed verify operations
        case ScriptError::VERIFY:
            return verify_result_verify;
        case ScriptError::EQUALVERIFY:
            return verify_result_equalverify;
        case ScriptError::CHECKMULTISIGVERIFY:
            return verify_result_checkmultisigverify;
        case ScriptError::CHECKSIGVERIFY:
            return verify_result_checksigverify;
        case ScriptError::NUMEQUALVERIFY:
            return verify_result_numequalverify;

        // Logical/Format/Canonical errors
        case ScriptError::BAD_OPCODE:
            return verify_result_bad_opcode;
        case ScriptError::DISABLED_OPCODE:
            return verify_result_disabled_opcode;
        case ScriptError::INVALID_STACK_OPERATION:
            return verify_result_invalid_stack_operation;
        case ScriptError::INVALID_ALTSTACK_OPERATION:
            return verify_result_invalid_altstack_operation;
        case ScriptError::UNBALANCED_CONDITIONAL:
            return verify_result_unbalanced_conditional;

        // BIP65/BIP112 (shared codes)
        case ScriptError::NEGATIVE_LOCKTIME:
            return verify_result_negative_locktime;
        case ScriptError::UNSATISFIED_LOCKTIME:
            return verify_result_unsatisfied_locktime;

        // BIP62
        case ScriptError::SIG_HASHTYPE:
            return verify_result_sig_hashtype;
        case ScriptError::SIG_DER:
            return verify_result_sig_der;
        case ScriptError::MINIMALDATA:
            return verify_result_minimaldata;
        case ScriptError::SIG_PUSHONLY:
            return verify_result_sig_pushonly;
        case ScriptError::SIG_HIGH_S:
            return verify_result_sig_high_s;
        case ScriptError::PUBKEYTYPE:
            return verify_result_pubkeytype;
        case ScriptError::CLEANSTACK:
            return verify_result_cleanstack;
        case ScriptError::MINIMALIF:
            return verify_result_minimalif;
        case ScriptError::SIG_NULLFAIL:
            return verify_result_sig_nullfail;
        case ScriptError::MINIMALNUM:
            return verify_result_minimalnum;

        // Softfork safeness
        case ScriptError::DISCOURAGE_UPGRADABLE_NOPS:
            return verify_result_discourage_upgradable_nops;

        case ScriptError::INPUT_SIGCHECKS:
            return verify_result_input_sigchecks;

        case ScriptError::INVALID_OPERAND_SIZE:
            return verify_result_invalid_operand_size;
        case ScriptError::INVALID_NUMBER_RANGE:
            return verify_result_invalid_number_range;
        case ScriptError::IMPOSSIBLE_ENCODING:
            return verify_result_impossible_encoding;
        case ScriptError::INVALID_SPLIT_RANGE:
            return verify_result_invalid_split_range;
        case ScriptError::INVALID_BIT_COUNT:
            return verify_result_invalid_bit_count;

        case ScriptError::CHECKDATASIGVERIFY:
            return verify_result_checkdatasigverify;

        case ScriptError::DIV_BY_ZERO:
            return verify_result_div_by_zero;
        case ScriptError::MOD_BY_ZERO:
            return verify_result_mod_by_zero;

        case ScriptError::INVALID_BITFIELD_SIZE:
            return verify_result_invalid_bitfield_size;
        case ScriptError::INVALID_BIT_RANGE:
            return verify_result_invalid_bit_range;

        case ScriptError::SIG_BADLENGTH:
            return verify_result_sig_badlength;
        case ScriptError::SIG_NONSCHNORR:
            return verify_result_sig_nonschnorr;

        case ScriptError::ILLEGAL_FORKID:
            return verify_result_illegal_forkid;
        case ScriptError::MUST_USE_FORKID:
            return verify_result_must_use_forkid;

        case ScriptError::SIGCHECKS_LIMIT_EXCEEDED:
            return verify_result_sigchecks_limit_exceeded;

        // Other
        case ScriptError::OP_RETURN:
            return verify_result_op_return;
        case ScriptError::UNKNOWN:
        case ScriptError::ERROR_COUNT:
        default:
            return verify_result_unknown_error;
    }
}

#else

verify_result_type script_error_to_verify_result(ScriptError_t code) {
    switch (code) {
        // Logical result
        case SCRIPT_ERR_OK:
            return verify_result_eval_true;
        case SCRIPT_ERR_EVAL_FALSE:
            return verify_result_eval_false;

        // Max size errors
        case SCRIPT_ERR_SCRIPT_SIZE:
            return verify_result_script_size;
        case SCRIPT_ERR_PUSH_SIZE:
            return verify_result_push_size;
        case SCRIPT_ERR_OP_COUNT:
            return verify_result_op_count;
        case SCRIPT_ERR_STACK_SIZE:
            return verify_result_stack_size;
        case SCRIPT_ERR_SIG_COUNT:
            return verify_result_sig_count;
        case SCRIPT_ERR_PUBKEY_COUNT:
            return verify_result_pubkey_count;

        // Failed verify operations
        case SCRIPT_ERR_VERIFY:
            return verify_result_verify;
        case SCRIPT_ERR_EQUALVERIFY:
            return verify_result_equalverify;
        case SCRIPT_ERR_CHECKMULTISIGVERIFY:
            return verify_result_checkmultisigverify;
        case SCRIPT_ERR_CHECKSIGVERIFY:
            return verify_result_checksigverify;
        case SCRIPT_ERR_NUMEQUALVERIFY:
            return verify_result_numequalverify;

        // Logical/Format/Canonical errors
        case SCRIPT_ERR_BAD_OPCODE:
            return verify_result_bad_opcode;
        case SCRIPT_ERR_DISABLED_OPCODE:
            return verify_result_disabled_opcode;
        case SCRIPT_ERR_INVALID_STACK_OPERATION:
            return verify_result_invalid_stack_operation;
        case SCRIPT_ERR_INVALID_ALTSTACK_OPERATION:
            return verify_result_invalid_altstack_operation;
        case SCRIPT_ERR_UNBALANCED_CONDITIONAL:
            return verify_result_unbalanced_conditional;

        // BIP65/BIP112 (shared codes)
        case SCRIPT_ERR_NEGATIVE_LOCKTIME:
            return verify_result_negative_locktime;
        case SCRIPT_ERR_UNSATISFIED_LOCKTIME:
            return verify_result_unsatisfied_locktime;

        // BIP62
        case SCRIPT_ERR_SIG_HASHTYPE:
            return verify_result_sig_hashtype;
        case SCRIPT_ERR_SIG_DER:
            return verify_result_sig_der;
        case SCRIPT_ERR_MINIMALDATA:
            return verify_result_minimaldata;
        case SCRIPT_ERR_SIG_PUSHONLY:
            return verify_result_sig_pushonly;
        case SCRIPT_ERR_SIG_HIGH_S:
            return verify_result_sig_high_s;
        case SCRIPT_ERR_SIG_NULLDUMMY:
            return verify_result_sig_nulldummy;
        case SCRIPT_ERR_PUBKEYTYPE:
            return verify_result_pubkeytype;
        case SCRIPT_ERR_CLEANSTACK:
            return verify_result_cleanstack;
        case SCRIPT_ERR_MINIMALIF:
            return verify_result_minimalif;
        case SCRIPT_ERR_SIG_NULLFAIL:
            return verify_result_sig_nullfail;

        // Softfork safeness
        case SCRIPT_ERR_DISCOURAGE_UPGRADABLE_NOPS:
            return verify_result_discourage_upgradable_nops;

        // Softfork safeness
        case SCRIPT_ERR_DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM:
            return verify_result_discourage_upgradable_witness_program;

        // Segregated witness
        case SCRIPT_ERR_WITNESS_PROGRAM_WRONG_LENGTH:
            return verify_result_witness_program_wrong_length;
        case SCRIPT_ERR_WITNESS_PROGRAM_WITNESS_EMPTY:
            return verify_result_witness_program_empty_witness;
        case SCRIPT_ERR_WITNESS_PROGRAM_MISMATCH:
            return verify_result_witness_program_mismatch;
        case SCRIPT_ERR_WITNESS_MALLEATED:
            return verify_result_witness_malleated;
        case SCRIPT_ERR_WITNESS_MALLEATED_P2SH:
            return verify_result_witness_malleated_p2sh;
        case SCRIPT_ERR_WITNESS_UNEXPECTED:
            return verify_result_witness_unexpected;
        case SCRIPT_ERR_WITNESS_PUBKEYTYPE:
            return verify_result_witness_pubkeytype;

        case SCRIPT_ERR_OP_CODESEPARATOR:
            return verify_result_op_codeseparator;
        case SCRIPT_ERR_SIG_FINDANDDELETE:
            return verify_result_sig_findanddelete;

        // Other
        case SCRIPT_ERR_OP_RETURN:
            return verify_result_op_return;
        case SCRIPT_ERR_UNKNOWN_ERROR:
        case SCRIPT_ERR_ERROR_COUNT:
        default:
            return verify_result_unknown_error;
    }
}
#endif

// This mapping decouples the consensus API from the satoshi implementation
// files. We prefer to keep our copies of consensus files isomorphic.
// This function is not published (but non-static for testability).
unsigned int verify_flags_to_script_flags(unsigned int flags) {
    unsigned int script_flags = SCRIPT_VERIFY_NONE;

    if ((flags & verify_flags_p2sh) != 0) {
        script_flags |= SCRIPT_VERIFY_P2SH;
    }

    if ((flags & verify_flags_strictenc) != 0) {
        script_flags |= SCRIPT_VERIFY_STRICTENC;
    }

    if ((flags & verify_flags_dersig) != 0) {
        script_flags |= SCRIPT_VERIFY_DERSIG;
    }

    if ((flags & verify_flags_low_s) != 0) {
        script_flags |= SCRIPT_VERIFY_LOW_S;
    }

#if ! defined(KTH_CURRENCY_BCH)
    if ((flags & verify_flags_nulldummy) != 0) {
        script_flags |= SCRIPT_VERIFY_NULLDUMMY;
    }
#endif

    if ((flags & verify_flags_sigpushonly) != 0) {
        script_flags |= SCRIPT_VERIFY_SIGPUSHONLY;
    }

    if ((flags & verify_flags_minimaldata) != 0) {
        script_flags |= SCRIPT_VERIFY_MINIMALDATA;
    }

    if ((flags & verify_flags_discourage_upgradable_nops) != 0) {
        script_flags |= SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_NOPS;
    }

    if ((flags & verify_flags_cleanstack) != 0) {
        script_flags |= SCRIPT_VERIFY_CLEANSTACK;
    }

    if ((flags & verify_flags_checklocktimeverify) != 0) {
        script_flags |= SCRIPT_VERIFY_CHECKLOCKTIMEVERIFY;
    }

    if ((flags & verify_flags_checksequenceverify) != 0) {
        script_flags |= SCRIPT_VERIFY_CHECKSEQUENCEVERIFY;
    }

#if ! defined(KTH_CURRENCY_BCH)
    if ((flags & verify_flags_witness) != 0) {
        script_flags |= SCRIPT_VERIFY_WITNESS;
    }

    if ((flags & verify_flags_discourage_upgradable_witness_program) != 0) {
        script_flags |= SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM;
    }

    if ((flags & verify_flags_minimal_if) != 0) {
        script_flags |= SCRIPT_VERIFY_MINIMALIF;
    }

    if ((flags & verify_flags_null_fail) != 0) {
        script_flags |= SCRIPT_VERIFY_NULLFAIL;
    }

    if ((flags & verify_flags_witness_public_key_compressed) != 0) {
        script_flags |= SCRIPT_VERIFY_WITNESS_PUBKEYTYPE;
    }

    if ((flags & verify_flags_const_scriptcode) != 0) {
        script_flags |= SCRIPT_VERIFY_CONST_SCRIPTCODE;
    }
#endif //! defined(KTH_CURRENCY_BCH)

#if defined(KTH_CURRENCY_BCH)
    if ((flags & verify_flags_null_fail) != 0) {
        script_flags |= SCRIPT_VERIFY_NULLFAIL;
    }

    // Removed
    // if ((flags & verify_flags_compressed_pubkeytype) != 0)
    //     script_flags |= SCRIPT_VERIFY_COMPRESSED_PUBKEYTYPE;

    if ((flags & verify_flags_enable_sighash_forkid) != 0)
        script_flags |= SCRIPT_ENABLE_SIGHASH_FORKID;

    if ((flags & verify_flags_disallow_segwit_recovery) != 0)
        script_flags |= SCRIPT_DISALLOW_SEGWIT_RECOVERY;

    if ((flags & verify_flags_enable_schnorr_multisig) != 0)
        script_flags |= SCRIPT_ENABLE_SCHNORR_MULTISIG;

    if ((flags & verify_flags_input_sigchecks) != 0)
        script_flags |= SCRIPT_VERIFY_INPUT_SIGCHECKS;

    if ((flags & verify_flags_enforce_sigchecks) != 0)
        script_flags |= SCRIPT_ENFORCE_SIGCHECKS;

    if ((flags & verify_flags_64_bit_integers) != 0)
        script_flags |= SCRIPT_64_BIT_INTEGERS;

    if ((flags & verify_flags_native_introspection) != 0)
        script_flags |= SCRIPT_NATIVE_INTROSPECTION;

    if ((flags & verify_flags_enable_p2sh_32) != 0)
        script_flags |= SCRIPT_ENABLE_P2SH_32;

    if ((flags & verify_flags_enable_tokens) != 0)
        script_flags |= SCRIPT_ENABLE_TOKENS;
#endif

    return script_flags;
}

// This function is published. The implementation exposes no satoshi internals.

#if defined(KTH_CURRENCY_BCH)
verify_result_type verify_script(unsigned char const* transaction,
    size_t transaction_size, unsigned char const* prevout_script,
    size_t prevout_script_size, unsigned int tx_input_index,
    unsigned int flags, size_t& sig_checks, int64_t amount, std::vector<std::vector<uint8_t>> coins) {


    if (amount > INT64_MAX) {
        throw std::invalid_argument("value");
    }

    if (transaction_size > 0 && transaction == NULL) {
        throw std::invalid_argument("transaction");
    }

    if (prevout_script_size > 0 && prevout_script == NULL) {
        throw std::invalid_argument("prevout_script");
    }

    std::optional<CTransaction> txopt;

    try {
        transaction_istream stream(transaction, transaction_size);
        txopt.emplace(deserialize, stream);
    }
    catch (const std::exception&) {
        return verify_result_tx_invalid;
    }

    if ( ! txopt) {
        return verify_result_tx_invalid;
    }

    auto const& tx = *txopt;

    if (tx_input_index >= tx.vin.size()) {
        return verify_result_tx_input_invalid;
    }

    if (GetSerializeSize(tx, PROTOCOL_VERSION) != transaction_size) {
        return verify_result_tx_size_invalid;
    }

    ScriptError error;
    Amount am(amount);
    const unsigned int script_flags = verify_flags_to_script_flags(flags);

    CScript const output_script(prevout_script, prevout_script + prevout_script_size);
    auto const& input_script = tx.vin[tx_input_index].scriptSig;

    ScriptExecutionMetrics metrics = {};

    if (coins.size() != 0) {
        auto const output_getter = [&coins](size_t i) {
            auto const& data = coins.at(i);
            CDataStream stream(data, SER_NETWORK, PROTOCOL_VERSION);
            CTxOut ret;
            ::Unserialize(stream, ret);
            return ret;
        };

        auto const contexts = ScriptExecutionContext::createForAllInputs(tx, output_getter);

        if (tx_input_index >= contexts.size()) {
            return verify_result_tx_input_invalid;
        }
        auto const context = contexts[tx_input_index];
        PrecomputedTransactionData txdata(context);
        TransactionSignatureChecker checker(context, txdata);
        VerifyScript(input_script, output_script, script_flags, checker, metrics, &error);
    } else {
        ScriptExecutionContextOpt context = std::nullopt;
        // ContextOptSignatureChecker(const ScriptExecutionContextOpt &contextIn) : contextOpt(contextIn) {}
        ContextOptSignatureChecker checker(context);
        VerifyScript(input_script, output_script, script_flags, checker, metrics, &error);
    }

    // VerifyScript(input_script, output_script, script_flags, checker, metrics, context, &error);
    sig_checks = metrics.nSigChecks;
    return script_error_to_verify_result(error);
}
#else //KTH_CURRENCY_BCH

verify_result_type verify_script(const unsigned char* transaction,
    size_t transaction_size, const unsigned char* prevout_script,
    size_t prevout_script_size, unsigned long long prevout_value,
    unsigned int tx_input_index, unsigned int flags) {

    if (prevout_value > INT64_MAX) {
        throw std::invalid_argument("value");
    }

    if (transaction_size > 0 && transaction == NULL) {
        throw std::invalid_argument("transaction");
    }

    if (prevout_script_size > 0 && prevout_script == NULL) {
        throw std::invalid_argument("prevout_script");
    }

    std::shared_ptr<CTransaction> tx;

    try {
        transaction_istream stream(transaction, transaction_size);
        tx = std::make_shared<CTransaction>(deserialize, stream);
    }
    catch (const std::exception&) {
        return verify_result_tx_invalid;
    }

    if (tx_input_index >= tx->vin.size()) {
        return verify_result_tx_input_invalid;
    }

    if (GetSerializeSize(*tx, PROTOCOL_VERSION) != transaction_size) {
        return verify_result_tx_size_invalid;
    }

    ScriptError_t error;
    auto const& tx_ref = *tx;
    const CAmount amount(static_cast<int64_t>(prevout_value));
    TransactionSignatureChecker checker(&tx_ref, tx_input_index, amount);
    const unsigned int script_flags = verify_flags_to_script_flags(flags);
    CScript output_script(prevout_script, prevout_script + prevout_script_size);
    auto const& input_script = tx->vin[tx_input_index].scriptSig;
    auto const witness_stack = &tx->vin[tx_input_index].scriptWitness;

    VerifyScript(input_script, output_script, witness_stack, script_flags, checker, &error);

    return script_error_to_verify_result(error);
}
#endif //KTH_CURRENCY_BCH

char const* version() {
    return KTH_CONSENSUS_VERSION;
}

} // namespace kth::consensus
