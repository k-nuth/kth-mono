// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTUCTURE_MACHINE_SIGHASH_ALGORITHM_HPP
#define KTH_INFRASTUCTURE_MACHINE_SIGHASH_ALGORITHM_HPP

#include <cstdint>

namespace kth::infrastructure::machine {

/// Signature hash types.
/// Comments from: bitcoin.org/en/developer-guide#standard-transactions
enum sighash_algorithm : uint32_t
{
    /// The default, signs all the inputs and outputs, protecting everything
    /// except the signature scripts against modification.
    all = 0x01,

    /// Signs all of the inputs but none of the outputs, allowing anyone to
    /// change where the satoshis are going unless other signatures using
    /// other signature hash flags protect the outputs.
    none = 0x02,

    /// The only output signed is the one corresponding to this input (the
    /// output with the same output index number as this input), ensuring
    /// nobody can change your part of the transaction but allowing other
    /// signers to change their part of the transaction. The corresponding
    /// output must exist or the value '1' will be signed, breaking the
    /// security scheme. This input, as well as other inputs, are included
    /// in the signature. The sequence numbers of other inputs are not
    /// included in the signature, and can be updated.
    single = 0x03,

    //TODO(fernando): check if we have to cancel the following values for coins not equal to BCH
// #if defined(KTH_CURRENCY_BCH)
    cash_forkid_all = all | 0x40,           //Old name: all_forkid
    cash_forkid_none = none | 0x40,         //Old name: none_forkid
    cash_forkid_single = single | 0x40,     //Old name: single_forkid
// #endif

    /// The above types can be modified with this flag, creating three new
    /// combined types.
    anyone_can_pay = 0x80,

    /// Signs all of the outputs but only this one input, and it also allows
    /// anyone to add or remove other inputs, so anyone can contribute
    /// additional satoshis but they cannot change how many satoshis are
    /// sent nor where they go.
    all_anyone_can_pay = all | anyone_can_pay,

    /// Signs only this one input and allows anyone to add or remove other
    /// inputs or outputs, so anyone who gets a copy of this input can spend
    /// it however they'd like.
    none_anyone_can_pay = none | anyone_can_pay,

    /// Signs this one input and its corresponding output. Allows anyone to
    /// add or remove other inputs.
    single_anyone_can_pay = single | anyone_can_pay,

    /// Used to mask unused bits in the signature hash byte.
    mask = 0x1f
};

} // namespace kth::infrastructure::machine

#endif
