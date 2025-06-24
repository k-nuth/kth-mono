// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTUCTURE_ELLIPTIC_CURVE_HPP
#define KTH_INFRASTUCTURE_ELLIPTIC_CURVE_HPP

#include <cstddef>

#include <kth/infrastructure/compat.hpp>
#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/data.hpp>

namespace kth {

/// The sign byte value for an even (y-valued) key.
static constexpr uint8_t ec_even_sign = 2;

/// Private key:
static constexpr size_t ec_secret_size = 32;
using ec_secret = byte_array<ec_secret_size>;

/// Compressed public key:
static constexpr size_t ec_compressed_size = 33;
using ec_compressed = byte_array<ec_compressed_size>;

using point_list = std::vector<ec_compressed>;

/// Uncompressed public key:
static constexpr size_t ec_uncompressed_size = 65;
using ec_uncompressed = byte_array<ec_uncompressed_size>;

// Parsed ECDSA signature:
static constexpr size_t ec_signature_size = 64;
using ec_signature = byte_array<ec_signature_size>;

// DER encoded signature:
static constexpr size_t max_der_signature_size = 72;
using der_signature = data_chunk;

/// DER encoded signature with sighash byte for contract endorsement:
static constexpr size_t min_endorsement_size = 9;
static constexpr size_t max_endorsement_size = 73;
using endorsement = data_chunk;

/// Recoverable ecdsa signature for message signing:
struct KI_API recoverable_signature
{
    ec_signature signature;
    uint8_t recovery_id;
};

static constexpr ec_compressed null_compressed_point =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static constexpr ec_uncompressed null_uncompressed_point =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Add and multiply EC values
// ----------------------------------------------------------------------------

/// Compute the sum a += G*b, where G is the curve's generator point.
/// return false on failure (such as infinity or zero).
KI_API bool ec_add(ec_compressed& point, ec_secret const& secret);

/// Compute the sum a += G*b, where G is the curve's generator point.
/// return false on failure (such as infinity or zero).
KI_API bool ec_add(ec_uncompressed& point, ec_secret const& secret);

/// Compute the sum a = (a + b) % n, where n is the curve order.
/// return false on failure (such as a zero result).
KI_API bool ec_add(ec_secret& left, ec_secret const& right);

/// Compute the product point *= secret.
/// return false on failure (such as infinity or zero).
KI_API bool ec_multiply(ec_compressed& point, ec_secret const& secret);

/// Compute the product point *= secret.
/// return false on failure (such as infinity or zero).
KI_API bool ec_multiply(ec_uncompressed& point, ec_secret const& secret);

/// Compute the product a = (a * b) % n, where n is the curve order.
/// return false on failure (such as a zero result).
KI_API bool ec_multiply(ec_secret& left, ec_secret const& right);

// Convert keys
// ----------------------------------------------------------------------------

/// Convert an uncompressed public point to compressed.
KI_API bool compress(ec_compressed& out, const ec_uncompressed& point);

/// Convert a compressed public point to decompressed.
KI_API bool decompress(ec_uncompressed& out, const ec_compressed& point);

/// Convert a secret to a compressed public point.
KI_API bool secret_to_public(ec_compressed& out, ec_secret const& secret);

/// Convert a secret parameter to an uncompressed public point.
KI_API bool secret_to_public(ec_uncompressed& out, ec_secret const& secret);

// Verify keys
// ----------------------------------------------------------------------------

/// Verify a secret.
KI_API bool verify(ec_secret const& secret);

/// Verify a point.
KI_API bool verify(const ec_compressed& point);

/// Verify a point.
KI_API bool verify(const ec_uncompressed& point);

// Detect public keys
// ----------------------------------------------------------------------------

/// Determine if the compressed public key is even (y-valued).
bool is_even_key(const ec_compressed& point);

/// Fast detection of compressed public key structure.
bool is_compressed_key(data_slice point);

/// Fast detection of uncompressed public key structure.
bool is_uncompressed_key(data_slice point);

/// Fast detection of compressed or uncompressed public key structure.
bool is_public_key(data_slice point);

/// Fast detection of endorsement structure (DER with signature hash type).
bool is_endorsement(const endorsement& endorsement);

// DER parse/encode
// ----------------------------------------------------------------------------

/// Parse an endorsement into signature hash type and DER signature.
KI_API bool parse_endorsement(uint8_t& sighash_type,
    der_signature& der_signature, endorsement&& endorsement);

/// Parse a DER encoded signature with optional strict DER enforcement.
/// Treat an empty DER signature as invalid, in accordance with BIP66.
KI_API bool parse_signature(ec_signature& out,
    const der_signature& der_signature, bool strict);

/// Encode an EC signature as DER (strict).
KI_API bool encode_signature(der_signature& out, const ec_signature& signature);

// EC sign/verify
// ----------------------------------------------------------------------------

/// Create a deterministic ECDSA signature using a private key.
KI_API bool sign(ec_signature& out, ec_secret const& secret,
    hash_digest const& hash);

/// Verify an EC signature using a compressed point.
KI_API bool verify_signature(const ec_compressed& point,
    hash_digest const& hash, const ec_signature& signature);

/// Verify an EC signature using an uncompressed point.
KI_API bool verify_signature(const ec_uncompressed& point,
    hash_digest const& hash, const ec_signature& signature);

/// Verify an EC signature using a potential point.
KI_API bool verify_signature(data_slice point, hash_digest const& hash,
    const ec_signature& signature);

// Recoverable sign/recover
// ----------------------------------------------------------------------------

/// Create a recoverable signature for use in message signing.
KI_API bool sign_recoverable(recoverable_signature& out,
    ec_secret const& secret, hash_digest const& hash);

/// Recover the compressed point from a recoverable message signature.
KI_API bool recover_public(ec_compressed& out,
    const recoverable_signature& recoverable, hash_digest const& hash);

/// Recover the uncompressed point from a recoverable message signature.
KI_API bool recover_public(ec_uncompressed& out,
    const recoverable_signature& recoverable, hash_digest const& hash);

} // namespace kth

#endif
