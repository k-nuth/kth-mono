// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTUCTURE_MACHINE_SCRIPT_PATTERN_HPP
#define KTH_INFRASTUCTURE_MACHINE_SCRIPT_PATTERN_HPP

namespace kth::infrastructure::machine {

/// Script patterms.
/// Comments from: bitcoin.org/en/developer-guide#signature-hash-types
enum class script_pattern
{
    /// Null Data
    /// Pubkey Script: OP_RETURN <0 to 80 bytes of data> (formerly 40 bytes)
    /// Null data scripts cannot be spent, so there's no signature script.
    null_data,

    /// Pay to Multisig [BIP11]
    /// Pubkey script: <m> <A pubkey>[B pubkey][C pubkey...] <n> OP_CHECKMULTISIG
    /// Signature script: OP_0 <A sig>[B sig][C sig...]
    pay_multisig,

    /// Pay to Public Key (obsolete)
    pay_public_key,

    /// Pay to Public Key Hash [P2PKH]
    /// Pubkey script: OP_DUP OP_HASH160 <PubKeyHash> OP_EQUALVERIFY OP_CHECKSIG
    /// Signature script: <sig> <pubkey>
    pay_key_hash,

    /// Pay to Script Hash [P2SH/BIP16]
    /// The redeem script may be any pay type, but only multisig makes sense.
    /// Pubkey script: OP_HASH160 <Hash160(redeemScript)> OP_EQUAL
    /// Signature script: <sig>[sig][sig...] <redeemScript>
    pay_script_hash,

    /// Sign Multisig script [BIP11]
    sign_multisig,

    /// Sign Public Key (obsolete)
    sign_public_key,

    /// Sign Public Key Hash [P2PKH]
    sign_key_hash,

    /// Sign Script Hash [P2SH/BIP16]
    sign_script_hash,

    /// Witness coinbase reserved value [BIP141].
    witness_reservation,

    /// The script may be valid but does not conform to the common templates.
    /// Such scripts are always accepted if they are mined into blocks, but
    /// transactions with uncommon scripts may not be forwarded by peers.
    non_standard
};

} // namespace kth::infrastructure::machine

#endif
