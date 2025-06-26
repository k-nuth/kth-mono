// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DOMAIN_MACHINE_RULE_FORK_HPP_
#define KTH_DOMAIN_MACHINE_RULE_FORK_HPP_

#include <cstdint>

namespace kth::domain::machine {

enum rule_fork : uint32_t {
    no_rules = 0,

    /// Allow minimum difficulty blocks (hard fork, testnet).
    easy_blocks = 1U << 0,

    /// Pay-to-script-hash enabled (soft fork, feature).
    bip16_rule = 1U << 1,

    /// No duplicated unspent transaction ids (hard fork, security).
    bip30_rule = 1U << 2,

    /// Coinbase must include height (soft fork, security).
    bip34_rule = 1U << 3,

    /// Strict DER signatures required (soft fork, security).
    bip66_rule = 1U << 4,

    /// Operation nop2 becomes check locktime verify (soft fork, feature).
    bip65_rule = 1U << 5,

    /// Hard code bip34-based activation heights (hard fork, optimization).
    bip90_rule = 1U << 6,

    /// Assume hash collisions cannot happen (hard fork, optimization).
    allow_collisions = 1U << 7,

    /// Enforce relative locktime (soft fork, feature).
    bip68_rule = 1U << 8,

    /// Operation nop3 becomes check sequence verify (soft fork, feature).
    bip112_rule = 1U << 9,

    /// Use median time past for locktime (soft fork, feature).
    bip113_rule = 1U << 10,

#if defined(KTH_CURRENCY_BCH)
    bch_uahf = 1U << 11,                //2017-Aug Hardfork - Bitcoin Cash UAHF (1501590000)
    bch_daa_cw144  = 1U << 12,          //2017-Nov Hardfork - DAA/cw-144 - (1510600000)
    bch_pythagoras = 1U << 13,          //2018-May Hardfork - pythagoras - (1526400000)
    bch_euclid = 1U << 14,              //2018-Nov Hardfork - euclid - (1542300000)
    bch_pisano = 1U << 15,              //2019-May Hardfork - pisano - (1557921600)
    bch_mersenne = 1U << 16,            //2019-Nov Hardfork - mersenne - (1573819200)
    bch_fermat = 1U << 17,              //2020-May Hardfork - fermat - (1589544000)
    bch_euler = 1U << 18,               //2020-Nov Hardfork - euler - (1605441600)
                                        //2021-May NO-Hardfork - (1621080000)
    bch_gauss = 1U << 19,               //2022-May Hardfork - gauss - (1652616000)
    bch_descartes = 1U << 20,           //2023-May Hardfork - descartes - (1684152000)
    bch_lobachevski = 1U << 21,         //2024-May Hardfork - lobachevski - (1715774400)
    bch_galois = 1U << 22,              //2025-May Hardfork - galois - (1747310400)
    bch_leibniz = 1U << 23,             //2026-May Hardfork - leibniz - (1778846400)
    bch_cantor = 1U << 24,              //2027-May Hardfork - cantor - (xxxxxxxxx)
    // bch_unnamed = 1U << xx,          //20xx-May Hardfork - unnamed - (9999999999)
#else
    // Just for segwit coins
    /// Segregated witness consensus layer (soft fork, feature).
    bip141_rule = 1U << 11,

    /// Segregated witness v0 verification (soft fork, feature).
    bip143_rule = 1U << 12,

    /// Prevent dummy value malleability (soft fork, feature).
    bip147_rule = 1U << 13,
#endif //KTH_CURRENCY_BCH


    /// Perform difficulty retargeting (hard fork, regtest).
    retarget = 1U << 30,

    /// Sentinel bit to indicate tx has not been validated.
    unverified = 1U << 31,

    /// Rules that use bip34-based activation.
    bip34_activations =
        rule_fork::bip34_rule |
        rule_fork::bip65_rule |
        rule_fork::bip66_rule,


#if ! defined(KTH_CURRENCY_BCH)
    /// Rules that use BIP9 bit zero first time activation.
    bip9_bit0_group =
        rule_fork::bip68_rule |
        rule_fork::bip112_rule |
        rule_fork::bip113_rule,

    /// Rules that use BIP9 bit one first time activation.
    bip9_bit1_group =
        rule_fork::bip141_rule |
        rule_fork::bip143_rule |
        rule_fork::bip147_rule,
#endif

    /// Simple mask to set all bits.
    all_rules = 0xffffffff
};

} // namespace kth::domain::machine

#endif // KTH_DOMAIN_MACHINE_RULE_FORK_HPP_
