// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_CHAIN_SCRIPT_HPP
#define KTH_DOMAIN_CHAIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <type_traits>

#include <kth/domain/chain/script_basis.hpp>
#include <kth/domain/constants.hpp>
#include <kth/domain/define.hpp>
#include <kth/domain/multi_crypto_settings.hpp>

#include <kth/domain/machine/operation.hpp>
#include <kth/domain/machine/rule_fork.hpp>

#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/machine/script_pattern.hpp>
#include <kth/infrastructure/machine/script_version.hpp>
#include <kth/infrastructure/math/elliptic_curve.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/thread.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::chain {

class transaction;
class witness;

class KD_API script : public script_basis {
public:
    using operation = machine::operation;
    using rule_fork = machine::rule_fork;
    using script_pattern = infrastructure::machine::script_pattern;
    using script_version = infrastructure::machine::script_version;

    // Constructors.
    //-------------------------------------------------------------------------

    script() = default;

    script(operation::list const& ops);
    script(operation::list&& ops);
    script(data_chunk const& encoded, bool prefix);
    script(data_chunk&& encoded, bool prefix);

    script(script const& x);
    script(script&& x) noexcept;
    script& operator=(script const& x);
    script& operator=(script&& x) noexcept;

    // Deserialization.
    //-------------------------------------------------------------------------

    /// Deserialization invalidates the iterator.
    void from_operations(operation::list&& ops);
    void from_operations(operation::list const& ops);
    bool from_string(std::string const& mnemonic);

    /// Script operations is valid if all push ops have the predicated size.
    bool is_valid_operations() const;

    // Serialization.
    //-------------------------------------------------------------------------

    std::string to_string(uint32_t active_forks) const;

    // Iteration.
    //-------------------------------------------------------------------------

    void clear();
    bool empty() const;
    size_t size() const;
    operation const& front() const;
    operation const& back() const;
    operation::iterator begin() const;
    operation::iterator end() const;
    operation const& operator[](size_t index) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    // size_t serialized_size(bool prefix) const;
    using script_basis::serialized_size;
    operation::list const& operations() const;

    // Signing.
    //-------------------------------------------------------------------------

    static
    hash_digest generate_signature_hash(transaction const& tx,
                                        uint32_t input_index,
                                        script const& script_code,
                                        uint8_t sighash_type,
                                        script_version version = script_version::unversioned,
                                        uint64_t value = max_uint64);

    static
    bool check_signature(ec_signature const& signature,
                            uint8_t sighash_type,
                            data_chunk const& public_key,
                            script const& script_code,
                            transaction const& tx,
                            uint32_t input_index,
                            script_version version = script_version::unversioned,
                            uint64_t value = max_uint64);

    static
    bool create_endorsement(endorsement& out, ec_secret const& secret, script const& prevout_script, transaction const& tx, uint32_t input_index, uint8_t sighash_type, script_version version = script_version::unversioned, uint64_t value = max_uint64);

    // Utilities (static).
    //-------------------------------------------------------------------------

    /// Transaction helpers.
    // static
    // hash_digest to_outputs(transaction const& tx);

    // static
    // hash_digest to_inpoints(transaction const& tx);

    // static
    // hash_digest to_sequences(transaction const& tx);

    /// Determine if the fork is enabled in the active forks set.
    static
    bool is_enabled(uint32_t active_forks, rule_fork fork) {
        return (fork & active_forks) != 0;
    }

    /// Consensus patterns.
    static
    bool is_push_only(operation::list const& ops);

    static
    bool is_relaxed_push(operation::list const& ops);

    static
    bool is_coinbase_pattern(operation::list const& ops, size_t height);

#if defined(KTH_SEGWIT_ENABLED)
    static
    bool is_commitment_pattern(operation::list const& ops);

    static
    bool is_witness_program_pattern(operation::list const& ops);
#endif

    /// Common output patterns (psh and pwsh are also consensus).
    static
    bool is_null_data_pattern(operation::list const& ops);

    static
    bool is_pay_multisig_pattern(operation::list const& ops);

    static
    bool is_pay_public_key_pattern(operation::list const& ops);

    static
    bool is_pay_key_hash_pattern(operation::list const& ops);

    static
    bool is_pay_script_hash_pattern(operation::list const& ops);

    static
    bool is_pay_witness_script_hash_pattern(operation::list const& ops);

    /// Common input patterns (skh is also consensus).
    static
    bool is_sign_multisig_pattern(operation::list const& ops);

    static
    bool is_sign_public_key_pattern(operation::list const& ops);

    static
    bool is_sign_key_hash_pattern(operation::list const& ops);

    static
    bool is_sign_script_hash_pattern(operation::list const& ops);

    /// Stack factories.
    static
    operation::list to_null_data_pattern(data_slice data);

    static
    operation::list to_pay_public_key_pattern(data_slice point);

    static
    operation::list to_pay_key_hash_pattern(short_hash const& hash);

    static
    operation::list to_pay_script_hash_pattern(short_hash const& hash);

    static
    operation::list to_pay_multisig_pattern(uint8_t signatures, point_list const& points);

    static
    operation::list to_pay_multisig_pattern(uint8_t signatures, data_stack const& points);

    // Utilities (non-static).
    //-------------------------------------------------------------------------

    /// Common pattern detection.
    data_chunk witness_program() const;
    script_version version() const;
    script_pattern pattern() const;
    script_pattern output_pattern() const;
    script_pattern input_pattern() const;

    /// Consensus computations.
    size_t sigops(bool accurate) const;
    void find_and_delete(data_stack const& endorsements);
    bool is_unspendable() const;

    void reset();

#if defined(KTH_SEGWIT_ENABLED)
    bool is_pay_to_witness(uint32_t forks) const;
#endif

    bool is_pay_to_script_hash(uint32_t forks) const;

private:
    static
    size_t serialized_size(operation::list const& ops);

    static
    data_chunk operations_to_data(operation::list const& ops);

    static
    hash_digest generate_unversioned_signature_hash(transaction const& tx, uint32_t input_index, script const& script_code, uint8_t sighash_type);

    static
    hash_digest generate_version_0_signature_hash(transaction const& tx,
                                                  uint32_t input_index,
                                                  script const& script_code,
                                                  uint64_t value,
                                                  uint8_t sighash_type);

    // These are protected by mutex.
    mutable bool cached_{false};
    mutable operation::list operations_;

#if ! defined(__EMSCRIPTEN__)
    mutable upgrade_mutex mutex_;
#else
    mutable shared_mutex mutex_;
#endif
};

} // namespace kth::domain::chain

#endif
