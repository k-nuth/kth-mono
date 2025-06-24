// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_CHAIN_TRANSACTION_HPP
#define KTH_DOMAIN_CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <kth/domain/chain/chain_state.hpp>
#include <kth/domain/chain/input.hpp>
#include <kth/domain/chain/output.hpp>
#include <kth/domain/chain/point.hpp>
#include <kth/domain/chain/transaction_basis.hpp>
#include <kth/domain/constants.hpp>
#include <kth/domain/define.hpp>
#include <kth/domain/multi_crypto_settings.hpp>

#include <kth/domain/machine/opcode.hpp>
#include <kth/domain/machine/rule_fork.hpp>

#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/math/elliptic_curve.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/thread.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::chain {

class KD_API transaction : public transaction_basis {
public:
    using ins = input::list;
    using outs = output::list;
    using list = std::vector<transaction>;
    using hash_ptr = std::shared_ptr<hash_digest>;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation {
        uint64_t originator = 0;
        code error = error::not_found;
        chain_state::ptr state = nullptr;

        // The transaction is an unspent duplicate.
        bool duplicate = false;

        // The unconfirmed tx exists in the store.
        bool pooled = false;

        // The unconfirmed tx is validated at the block's current fork state.
        bool current = false;

        // Similate organization and instead just validate the transaction.
        bool simulate = false;

        // The transaction was validated before its insertion in the mempool.
        bool validated = false;
    };

    // Constructors.
    //-----------------------------------------------------------------------------

    transaction();

    transaction(uint32_t version, uint32_t locktime, ins const& inputs, outs const& outputs
#ifdef KTH_CACHED_RPC_DATA
                , uint32_t cached_sigops = 0, uint64_t cached_fees = 0, bool cached_is_standard = false
#endif
               );
    transaction(uint32_t version, uint32_t locktime, ins&& inputs, outs&& outputs
#ifdef KTH_CACHED_RPC_DATA
               , uint32_t cached_sigops = 0, uint64_t cached_fees = 0, bool cached_is_standard = false
#endif
               );
    transaction(transaction const& x, hash_digest const& hash);
    transaction(transaction&& x, hash_digest const& hash);


    //Note(kth): cannot be defaulted because of the mutex data member.
    transaction(transaction const& x);
    transaction(transaction&& x) noexcept;
    transaction& operator=(transaction const& x);
    transaction& operator=(transaction&& x) noexcept;

    // Operators.
    //-----------------------------------------------------------------------------

    // bool operator==(transaction const& x) const;
    // bool operator!=(transaction const& x) const;

    // Deserialization.
    //-----------------------------------------------------------------------------

    // Witness is not used by outputs, just for template normalization.
    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, bool wire = true, bool witness = false
#ifdef KTH_CACHED_RPC_DATA
                    , bool unconfirmed = false
#endif
                ) {

        transaction_basis::from_data(source, wire, witness);

#ifdef KTH_CACHED_RPC_DATA
        if ( !  wire && unconfirmed) {
            auto const sigops = source.read_4_bytes_little_endian();
            cached_sigops_ = static_cast<uint32_t>(sigops);
            auto const fees = source.read_8_bytes_little_endian();
            cached_fees_ = static_cast<uint64_t>(fees);
            auto const is_standard = source.read_byte();
            cached_is_standard_ = static_cast<bool>(is_standard);
        }
#endif

        return source;
    }

    // bool is_valid() const;

    // Serialization.
    //-----------------------------------------------------------------------------

    data_chunk to_data(bool wire = true, bool witness = false
#ifdef KTH_CACHED_RPC_DATA
                        , bool unconfirmed = false
#endif
                    ) const;

    void to_data(data_sink& stream, bool wire = true, bool witness = false
#ifdef KTH_CACHED_RPC_DATA
                    , bool unconfirmed = false
#endif
                ) const;

    // Witness is not used by outputs, just for template normalization.
    template <typename W>
    void to_data(W& sink, bool wire = true, bool witness = false
#ifdef KTH_CACHED_RPC_DATA
                , bool unconfirmed = false
#endif
                ) const {

#if defined(KTH_SEGWIT_ENABLED)
        // Witness handling must be disabled for non-segregated txs.
        witness = witness && is_segregated();
#endif
        transaction_basis::to_data(sink, wire, witness);

#ifdef KTH_CACHED_RPC_DATA
        if ( ! wire && unconfirmed) {
            sink.write_4_bytes_little_endian(signature_operations());
            sink.write_8_bytes_little_endian(fees());
            sink.write_byte(is_standard());
        }
#endif
    }

    // Properties (size, accessors, cache).
    //-----------------------------------------------------------------------------

    size_t serialized_size(bool wire = true, bool witness = false
#ifdef KTH_CACHED_RPC_DATA
                            , bool unconfirmed = false
#endif
                        ) const;

    void set_version(uint32_t value);
    void set_locktime(uint32_t value);
    void set_inputs(const ins& value);
    void set_inputs(ins&& value);
    void set_outputs(const outs& value);
    void set_outputs(outs&& value);

#ifdef KTH_CACHED_RPC_DATA
    uint64_t cached_fees() const;
    uint32_t cached_sigops() const;
    bool cached_is_standard() const;
#endif

    hash_digest outputs_hash() const;
    hash_digest inpoints_hash() const;
    hash_digest sequences_hash() const;
    hash_digest hash(bool witness = false) const;

    // Utilities.
    //-------------------------------------------------------------------------

#if defined(KTH_SEGWIT_ENABLED)
    /// Clear witness from all inputs (does not change default hash).
    void strip_witness();
#endif

    void recompute_hash();

    // Validation.
    //-----------------------------------------------------------------------------

    using transaction_basis::signature_operations;

    uint64_t fees() const;
    // point::list previous_outputs() const;
    point::list missing_previous_outputs() const;
    // hash_list missing_previous_transactions() const;
    uint64_t total_input_value() const;
    uint64_t total_output_value() const;
    size_t signature_operations() const;

    bool is_overspent() const;

#if defined(KTH_SEGWIT_ENABLED)
    bool is_segregated() const;
#endif

    using transaction_basis::accept;

    code check(size_t max_block_size, bool transaction_pool, bool retarget = true) const;
    code accept(bool transaction_pool = true) const;
    code accept(chain_state const& state, bool transaction_pool = true) const;
    code connect() const;
    code connect(chain_state const& state) const;
    code connect_input(chain_state const& state, size_t input_index) const;


    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation validation{};

    bool is_standard() const;

// protected:
    void reset();

protected:
    void invalidate_cache() const;
    bool all_inputs_final() const;

private:

    // TODO(kth): (refactor to transaction_result)
    // this 3 variables should be stored in transaction_unconfired database when the store
    // function is called. This values will be in the transaction_result object before
    // creating the transaction object

    //Note(kth): Only accesible for unconfirmed txs
#ifdef KTH_CACHED_RPC_DATA
    uint64_t cached_fees_;
    uint32_t cached_sigops_;
    bool cached_is_standard_;
#endif

    // These share a mutex as they are not expected to contend.
    mutable hash_ptr hash_;
    mutable hash_ptr witness_hash_;
    mutable hash_ptr outputs_hash_;
    mutable hash_ptr inpoints_hash_;
    mutable hash_ptr sequences_hash_;

#if ! defined(__EMSCRIPTEN__)
    mutable upgrade_mutex hash_mutex_;
#else
    mutable shared_mutex hash_mutex_;
#endif

    // These share a mutex as they are not expected to contend.
    mutable std::optional<uint64_t> total_input_value_;
    mutable std::optional<uint64_t> total_output_value_;
    mutable std::optional<bool> segregated_;

#if ! defined(__EMSCRIPTEN__)
    mutable upgrade_mutex mutex_;
#else
    mutable shared_mutex mutex_;
#endif
};

#if ! defined(KTH_SEGWIT_ENABLED)
code verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, script const& prevout_script, uint64_t /*value*/);
#else
code verify(transaction const& tx, uint32_t input_index, uint32_t forks, script const& input_script, witness const& input_witness, script const& prevout_script, uint64_t value);
#endif

code verify(transaction const& tx, uint32_t input, uint32_t forks);

} // namespace kth::domain::chain

#endif
