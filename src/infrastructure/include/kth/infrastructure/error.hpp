// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ERROR_HPP
#define KTH_INFRASTRUCTURE_ERROR_HPP

#include <string>
#include <system_error>
#include <unordered_map>


#if defined(ASIO_STANDALONE)
#include <asio/error_code.hpp>
#else
#include <boost/system/error_code.hpp>
#endif

#include <kth/infrastructure/define.hpp>

namespace kth {

/// Console result codes, positive values are domain-specific.
enum console_result : int {
    failure = -1,
    okay = 0,
    invalid = 1
};

/// Alias for error code declarations.
using code = std::error_code;

/// Alias for boost error code declarations.

#if defined(ASIO_STANDALONE)
using boost_code = ::asio::error_code;
#else
using boost_code = boost::system::error_code;
#endif

namespace error {

// The numeric values of these codes may change without notice.
enum error_code_t {
    // general codes
    success = 0,
    deprecated = 6,
    unknown = 43,
    not_found = 3,
    file_system = 42,
    non_standard = 17,
    not_implemented = 4,
    oversubscribed = 71,

    // network
    service_stopped = 1,
    operation_failed = 2,
    resolve_failed = 7,
    network_unreachable = 8,
    address_in_use = 9,
    listen_failed = 10,
    accept_failed = 11,
    bad_stream = 12,
    channel_timeout = 13,
    address_blocked = 44,
    channel_stopped = 45,
    peer_throttling = 73,

    // database
    store_block_duplicate = 66,
    store_block_invalid_height = 67,
    store_block_missing_parent = 68,

    // blockchain
    duplicate_block = 51,
    orphan_block = 5,
    invalid_previous_block = 24,
    insufficient_work = 48,
    orphan_transaction = 14,
    insufficient_fee = 70,
    dusty_transaction = 76,
    stale_chain = 75,

    // check header
    invalid_proof_of_work = 26,
    futuristic_timestamp = 27,

    // accept header
    checkpoints_failed = 35,
    old_version_block = 36,
    incorrect_proof_of_work = 32,
    timestamp_too_early = 33,

    // check block
    block_size_limit = 50,
    empty_block = 47,
    first_not_coinbase = 28,
    extra_coinbases = 29,
    internal_duplicate = 49,
    block_internal_double_spend = 15,
    forward_reference = 79,
    merkle_mismatch = 31,
    block_legacy_sigop_limit = 30,

#if defined(KTH_CURRENCY_BCH)
    non_canonical_ordered = 84,
    block_sigchecks_limit = 85,
#endif

    // accept block
    block_non_final = 34,
    coinbase_height_mismatch = 37,
    coinbase_value_limit = 41,
    block_embedded_sigop_limit = 52,
    invalid_witness_commitment = 25,
    block_weight_limit = 82,

    // check transaction
    empty_transaction = 20,
    previous_output_null = 23,
    spend_overflow = 21,
    invalid_coinbase_script_size = 22,
    coinbase_transaction = 16,
    transaction_internal_double_spend = 72,
    transaction_size_limit = 53,
    transaction_legacy_sigop_limit = 54,

#if defined(KTH_CURRENCY_BCH)
    transaction_sigchecks_limit = 86,
#endif

    // accept transaction
    transaction_non_final = 74,
    premature_validation = 69,
    unspent_duplicate = 38,
    missing_previous_output = 19,
    double_spend = 18,
    coinbase_maturity = 46,
    spend_exceeds_value = 40,
    transaction_embedded_sigop_limit = 55,
    sequence_locked = 78,
    transaction_weight_limit = 83,
    transaction_version_out_of_range = 87,

    // connect input
    invalid_script = 39,
    invalid_script_size = 56,
    invalid_push_data_size = 57,
    invalid_operation_count = 58,
    invalid_stack_size = 59,
    invalid_stack_scope = 60,
    invalid_script_embed = 61,
    invalid_signature_encoding = 62,
    invalid_signature_lax_encoding = 63,
    incorrect_signature = 64,
    unexpected_witness = 77,
    invalid_witness = 80,
    dirty_witness = 81,
    stack_false = 65,

    // op eval
    op_disabled = 100,
    op_reserved,
    op_push_size,
    op_push_data,
    op_if,
    op_notif,
    op_else,
    op_endif,
    op_verify1,
    op_verify2,
    op_return,
    op_to_alt_stack,
    op_from_alt_stack,
    op_drop2,
    op_dup2,
    op_dup3,
    op_over2,
    op_rot2,
    op_swap2,
    op_if_dup,
    op_drop,
    op_dup,
    op_nip,
    op_over,
    op_pick,
    op_roll,
    op_rot,
    op_swap,
    op_tuck,

    op_cat,
    op_split,
    op_reverse_bytes,
    op_num2bin,
    op_num2bin_invalid_size,
    op_num2bin_size_exceeded,
    op_num2bin_impossible_encoding,
    op_bin2num,
    op_bin2num_invalid_number_range,

    op_size,

    op_and,
    op_or,
    op_xor,

    op_equal,
    op_equal_verify1,
    op_equal_verify2,
    op_add1,
    op_sub1,
    op_negate,
    op_abs,
    op_not,
    op_nonzero,

    op_add,
    op_add_overflow,
    op_sub,
    op_sub_underflow,
    op_mul,
    op_mul_overflow,
    op_div,
    op_div_by_zero,
    op_mod,
    op_mod_by_zero,

    op_bool_and,
    op_bool_or,
    op_num_equal,
    op_num_equal_verify1,
    op_num_equal_verify2,
    op_num_not_equal,
    op_less_than,
    op_greater_than,
    op_less_than_or_equal,
    op_greater_than_or_equal,
    op_min,
    op_max,
    op_within,
    op_ripemd160,
    op_sha1,
    op_sha256,
    op_hash160,
    op_hash256,
    op_code_seperator,

    op_check_sig,
    op_check_sig_verify1,

    op_check_data_sig,
    op_check_data_sig_verify,

    op_check_multisig_verify1,
    op_check_multisig_verify2,
    op_check_multisig_verify3,
    op_check_multisig_verify4,
    op_check_multisig_verify5,
    op_check_multisig_verify6,
    op_check_multisig_verify7,
    op_check_multisig,
    op_check_locktime_verify1,
    op_check_locktime_verify2,
    op_check_locktime_verify3,
    op_check_locktime_verify4,
    op_check_locktime_verify5,
    op_check_locktime_verify6,
    op_check_sequence_verify1,
    op_check_sequence_verify2,
    op_check_sequence_verify3,
    op_check_sequence_verify4,
    op_check_sequence_verify5,
    op_check_sequence_verify6,
    op_check_sequence_verify7,

    // Native Introspection Opcodes
    op_input_index,
    op_active_bytecode,
    op_tx_version,
    op_tx_input_count,
    op_tx_output_count,
    op_tx_locktime,
    op_utxo_value,
    op_utxo_bytecode,
    op_outpoint_tx_hash,
    op_outpoint_index,
    op_input_bytecode,
    op_input_sequence_number,
    op_output_value,
    op_output_bytecode,
    op_utxo_token_category,
    op_utxo_token_commitment,
    op_utxo_token_amount,
    op_output_token_category,
    op_output_token_commitment,
    op_output_token_amount,

    operation_failed_0,
    operation_failed_1,
    operation_failed_2,
    operation_failed_3,
    operation_failed_4,
    operation_failed_5,
    operation_failed_6,
    operation_failed_7,
    operation_failed_8,
    operation_failed_9,

    operation_failed_10,
    operation_failed_11,
    operation_failed_12,
    operation_failed_13,
    operation_failed_14,
    operation_failed_15,
    operation_failed_16,
    operation_failed_17,
    operation_failed_18,
    operation_failed_19,

    operation_failed_20,
    operation_failed_21,
    operation_failed_22,
    operation_failed_23,
    operation_failed_24,
    operation_failed_25,
    operation_failed_26,
    operation_failed_27,
    operation_failed_28,
    operation_failed_29,
    operation_failed_30,
    // Added out of order (bip147).
    op_check_multisig_verify8,

    // TX creation
    invalid_output,
    lock_time_conflict,
    input_index_out_of_range,
    input_sign_failed,

    // Mining
    low_benefit_transaction,
    duplicate_transaction,
    double_spend_mempool,
    double_spend_blockchain,

    // Numeric operations
    overflow,
    underflow,
    out_of_range,

    // Chip VM limits
    too_many_hash_iters,
    conditional_stack_depth,

    // Create transaction template
    insufficient_amount,
    empty_utxo_list,
    invalid_change,

    //TODO: check C-API error codes, add the new ones there
    //TODO: see the error to string functions

    // Cash Tokens
    invalid_bitfield,

    // Domain object serialization/deserialization
    read_past_end_of_buffer,
    skip_past_end_of_buffer,
    invalid_size,
    invalid_script_type,
    script_not_push_only,
    script_invalid_size,
    invalid_address_count,
    bad_inventory_count,
    version_too_low,
    version_too_new,
    invalid_compact_block,
    unsupported_version,
    invalid_filter_add,
    invalid_filter_load,
    bad_merkle_block_count,
    illegal_value,

    // Database cache
    height_not_found,
    hash_not_found,
    empty_cache,
    utxo_not_found,

    // Last error code.
    last_error_code
};

enum error_condition_t {};

KI_API code make_error_code(error_code_t e);
KI_API std::error_condition make_error_condition(error_condition_t e);
KI_API error_code_t boost_to_error_code(boost_code const& ec);
KI_API error_code_t posix_to_error_code(int ec);

} // namespace error
} // namespace kth

namespace std {

template <>
struct is_error_code_enum<kth::error::error_code_t>
  : public true_type
{};

template <>
struct is_error_condition_enum<kth::error::error_condition_t>
  : public true_type
{};

} // namespace std

#endif
