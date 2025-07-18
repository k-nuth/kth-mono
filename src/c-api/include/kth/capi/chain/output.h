// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_CAPI_CHAIN_OUTPUT_H_
#define KTH_CAPI_CHAIN_OUTPUT_H_

#include <stdint.h>

#include <kth/capi/primitives.h>
#include <kth/capi/visibility.h>

#include <kth/capi/chain/token_capability.h>

#ifdef __cplusplus
extern "C" {
#endif

KTH_EXPORT
kth_output_t kth_chain_output_construct_default(void);

KTH_EXPORT
kth_output_t kth_chain_output_construct(uint64_t value, kth_script_t script);

KTH_EXPORT
kth_output_t kth_chain_output_construct_with_token_fungible(uint64_t value, kth_script_t script, kth_hash_t const* token_category, int64_t token_amount);

KTH_EXPORT
kth_output_t kth_chain_output_construct_with_token_non_fungible(uint64_t value, kth_script_t script, kth_hash_t const* token_category, kth_token_capability_t capability, uint8_t* commitment_data, kth_size_t commitment_n);

KTH_EXPORT
kth_output_t kth_chain_output_construct_with_token_both(uint64_t value, kth_script_t script, kth_hash_t const* token_category, int64_t token_amount, kth_token_capability_t capability, uint8_t* commitment_data, kth_size_t commitment_n);

KTH_EXPORT
kth_output_t kth_chain_output_construct_with_token_data(uint64_t value, kth_script_t script, kth_token_data_t token_data);

KTH_EXPORT
void kth_chain_output_destruct(kth_output_t output);

KTH_EXPORT
kth_output_t kth_chain_output_factory_from_data(uint8_t* data, kth_size_t n);

KTH_EXPORT
kth_bool_t kth_chain_output_is_valid(kth_output_t output);

KTH_EXPORT
kth_size_t kth_chain_output_serialized_size(kth_output_t output, kth_bool_t wire); //wire = true

KTH_EXPORT
uint64_t kth_chain_output_value(kth_output_t output);

KTH_EXPORT
kth_size_t kth_chain_output_signature_operations(kth_output_t output);

KTH_EXPORT
kth_script_t kth_chain_output_script(kth_output_t output);

KTH_EXPORT
kth_payment_address_t kth_chain_output_payment_address(kth_output_t output, kth_bool_t use_testnet_rules);

KTH_EXPORT
uint8_t const* kth_chain_output_to_data(kth_output_t output, kth_bool_t wire, kth_size_t* out_size);

//uint32_t kth_chain_output_get_index(kth_output_t output);

// Cash Tokens ---------------------------------------------------------------
KTH_EXPORT
kth_bool_t kth_chain_output_has_token_data(kth_output_t output);

KTH_EXPORT
kth_token_data_t kth_chain_output_token_data(kth_output_t output);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* KTH_CAPI_CHAIN_OUTPUT_H_ */
