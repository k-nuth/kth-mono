// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_CHAIN_INPUT_BASIS_HPP
#define KTH_DOMAIN_CHAIN_INPUT_BASIS_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

#include <kth/domain/chain/output_point.hpp>
#include <kth/domain/chain/script.hpp>
#include <kth/domain/chain/witness.hpp>
#include <kth/domain/define.hpp>
#include <kth/domain/multi_crypto_settings.hpp>

#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/reader.hpp>
#include <kth/infrastructure/utility/thread.hpp>
#include <kth/infrastructure/utility/writer.hpp>

#include <kth/domain/utils.hpp>
#include <kth/domain/concepts.hpp>

namespace kth::domain::chain {

class KD_API input_basis {
public:
    using list = std::vector<input_basis>;

    // Constructors.
    //-------------------------------------------------------------------------

    input_basis() = default;
    input_basis(output_point const& previous_output, chain::script const& script, uint32_t sequence);
    input_basis(output_point&& previous_output, chain::script&& script, uint32_t sequence);

#if defined(KTH_SEGWIT_ENABLED)
    input_basis(output_point const& previous_output, chain::script const& script, chain::witness const& witness, uint32_t sequence);
    input_basis(output_point&& previous_output, chain::script&& script, chain::witness&& witness, uint32_t sequence);
#endif

    input_basis(input_basis const& x) = default;
    input_basis(input_basis&& x)  = default;
    input_basis& operator=(input_basis const& x) = default;
    input_basis& operator=(input_basis&& x) = default;

    // Operators.
    //-------------------------------------------------------------------------

    bool operator==(input_basis const& x) const;
    bool operator!=(input_basis const& x) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    template <typename R, KTH_IS_READER(R)>
    bool from_data(R& source, bool wire = true, KTH_DECL_WITN_ARG) {
#if defined(KTH_SEGWIT_ENABLED)
        // Always write witness to store so that we know how to read it.
        witness |= !wire;
#endif

        reset();

        if ( ! previous_output_.from_data(source, wire)) {
            return false;
        }

        script_.from_data(source, true);

#if defined(KTH_SEGWIT_ENABLED)
        // Transaction from_data handles the discontiguous wire witness decoding.
        if (witness_val(witness) && !wire) {
            witness_.from_data(source, true);
        }
#endif
        sequence_ = source.read_4_bytes_little_endian();

        if ( ! source) {
            reset();
        }

        return source;
    }

    [[nodiscard]]
    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    [[nodiscard]]
    data_chunk to_data(bool wire = true, bool witness = false) const;

    void to_data(data_sink& stream, bool wire = true, bool witness = false) const;

    template <typename W>
    void to_data(W& sink, bool wire = true, KTH_DECL_WITN_ARG) const {
#if defined(KTH_SEGWIT_ENABLED)
        // Always write witness to store so that we know how to read it.
        witness |= !wire;
#endif

        previous_output_.to_data(sink, wire);
        script_.to_data(sink, true);

#if defined(KTH_SEGWIT_ENABLED)
        // Transaction to_data handles the discontiguous wire witness encoding.
        if (witness_val(witness) && !wire) {
            witness_.to_data(sink, true);
        }
#endif
        sink.write_4_bytes_little_endian(sequence_);
    }

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    [[nodiscard]]
    size_t serialized_size_non_witness(bool wire) const;

    /// This accounts for wire witness, but does not read or write it.
    [[nodiscard]]
    size_t serialized_size(bool wire = true, bool witness = false) const;


    output_point& previous_output();

    [[nodiscard]]
    output_point const& previous_output() const;

    void set_previous_output(output_point const& value);
    void set_previous_output(output_point&& value);

    // [[deprecated]] // unsafe
    chain::script& script();

    [[nodiscard]]
    chain::script const& script() const;

    void set_script(chain::script const& value);
    void set_script(chain::script&& value);


#if defined(KTH_SEGWIT_ENABLED)
    // [[deprecated]] // unsafe
    chain::witness& witness();
    chain::witness const& witness() const;
    void set_witness(chain::witness const& value);
    void set_witness(chain::witness&& value);
#endif // KTH_CURRENCY_BCH

    [[nodiscard]]
    uint32_t sequence() const;

    void set_sequence(uint32_t value);

    // Utilities.
    //-------------------------------------------------------------------------

#if defined(KTH_SEGWIT_ENABLED)
    void strip_witness();
#endif

    // Validation.
    //-------------------------------------------------------------------------

    [[nodiscard]]
    bool is_final() const;

#if defined(KTH_SEGWIT_ENABLED)
    [[nodiscard]]
    bool is_segregated() const;
#endif

    [[nodiscard]]
    bool is_locked(size_t block_height, uint32_t median_time_past) const;

    [[nodiscard]]
    size_t signature_operations(bool bip16, bool bip141) const;

    bool extract_reserved_hash(hash_digest& out) const;
    bool extract_embedded_script(chain::script& out) const;

#if defined(KTH_SEGWIT_ENABLED)
    bool extract_witness_script(chain::script& out, chain::script const& prevout) const;
#endif

// protected:
    void reset();

private:
    output_point previous_output_;
    chain::script script_;
#if defined(KTH_SEGWIT_ENABLED)
    chain::witness witness_;
#endif
    uint32_t sequence_{0};
};

} // namespace kth::domain::chain

#endif // KTH_DOMAIN_CHAIN_INPUT_BASIS_HPP