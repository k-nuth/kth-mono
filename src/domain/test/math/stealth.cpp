// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;

// TODO(legacy): split out individual functions and standardize test names.
// Start Test Suite: stealth tests

#define SCAN_PRIVATE "fa63521e333e4b9f6a98a142680d3aef4d8e7f79723ce0043691db55c36bd905"
#define SCAN_PUBLIC "034ea70b28d607bf3a2493102001cab35689cf2152530bf8bf8a5b594af6ae31d0"

#define SPEND_PRIVATE "dcc1250b51c0f03ae4e978e0256ede51dc1144e345c926262b9717b1bcc9bd1b"
#define SPEND_PUBLIC "03d5b3853bbee336b551ff999b0b1d656e65a7649037ae0dcb02b3c4ff5f29e5be"

#define EPHEMERAL_PRIVATE "5f70a77b32260a7a32c62242381fba2cf40c0e209e665a7959418eae4f2da22b"
#define EPHEMERAL_PUBLIC "0387ff9128d18ddcec0a8119589a62b88bc035cb9cd6db08ce5ff702a78ef8f922"

#define STEALTH_PRIVATE "280a9931c0a7b8f9bed96bad35f69a1431817fb77043fdff641ad48ce1e4411e"
#define STEALTH_PUBLIC "0305f6b99a44a2bdec8b484ffcee561cf9a0c3b7ea92ea8e6334e6fbc4f1c17899"

// $ bx ec-add 03d5b3853bbee336b551ff999b0b1d656e65a7649037ae0dcb02b3c4ff5f29e5be 4b4974266ee6c8bed9eff2cd1087bbc1101f17bad9c37814f8561b67f550c544 | bx ec-to-address
#define P2PKH_ADDRESS "1Gvq8pSTRocNLDyf858o4PL3yhZm5qQDgB"

// $ bx ec-add 03d5b3853bbee336b551ff999b0b1d656e65a7649037ae0dcb02b3c4ff5f29e5be 4b4974266ee6c8bed9eff2cd1087bbc1101f17bad9c37814f8561b67f550c544 | bx ec-to-address - v 111
// #define P2PKH_ADDRESS_TESTNET "mwSnRsXSEq3d7LTGqe7AtJYNqhATwHdhMb"

TEST_CASE("stealth round trip", "[stealth]") {
    ec_secret expected_stealth_private;
    REQUIRE(decode_base16(expected_stealth_private, STEALTH_PRIVATE));

    // Receiver generates a new scan private.
    ec_secret scan_private;
    ec_compressed scan_public;
    REQUIRE(decode_base16(scan_private, SCAN_PRIVATE));
    REQUIRE(secret_to_public(scan_public, scan_private));
    REQUIRE(encode_base16(scan_public) == SCAN_PUBLIC);

    // Receiver generates a new spend private.
    ec_secret spend_private;
    ec_compressed spend_public;
    REQUIRE(decode_base16(spend_private, SPEND_PRIVATE));
    REQUIRE(secret_to_public(spend_public, spend_private));
    REQUIRE(encode_base16(spend_public) == SPEND_PUBLIC);

    // Sender generates a new ephemeral key.
    ec_secret ephemeral_private;
    ec_compressed ephemeral_public;
    REQUIRE(decode_base16(ephemeral_private, EPHEMERAL_PRIVATE));
    REQUIRE(secret_to_public(ephemeral_public, ephemeral_private));
    REQUIRE(encode_base16(ephemeral_public) == EPHEMERAL_PUBLIC);

    // Sender derives stealth public, requiring ephemeral private.
    ec_compressed sender_public;
    REQUIRE(uncover_stealth(sender_public, scan_public, ephemeral_private, spend_public));
    REQUIRE(encode_base16(sender_public) == STEALTH_PUBLIC);

    // Receiver derives stealth public, requiring scan private.
    ec_compressed receiver_public;
    REQUIRE(uncover_stealth(receiver_public, ephemeral_public, scan_private, spend_public));
    REQUIRE(encode_base16(receiver_public) == STEALTH_PUBLIC);

    // Only reciever can derive stealth private, as it requires both scan and spend private.
    ec_secret stealth_private;
    REQUIRE(uncover_stealth(stealth_private, ephemeral_public, scan_private, spend_private));

    // This shows that both parties have actually generated stealth public.
    ec_compressed stealth_public;
    REQUIRE(secret_to_public(stealth_public, stealth_private));
    REQUIRE(encode_base16(stealth_public) == STEALTH_PUBLIC);

    // Both parties therefore have the ability to generate the p2pkh address.
    // versioning: stealth_address::main corresponds to payment_address::main_p2pkh
    wallet::payment_address address(wallet::ec_public{stealth_public}, wallet::payment_address::mainnet_p2kh);
    REQUIRE(address.encoded_legacy() == P2PKH_ADDRESS);
}

TEST_CASE("verify string constructor", "[stealth]") {
    std::string const value = "01100110000";
    binary prefix(value);
    REQUIRE(value.size() == prefix.size());
    for (size_t i = 0; i < value.size(); ++i) {
        auto const comparison = value[i] == '1';
       REQUIRE(prefix[i] == comparison);
    }
}

// Binary as a value on the left, padded with zeros to the y.
TEST_CASE("compare constructor results", "[stealth]") {
    std::string value = "01100111000";
    binary prefix(value);
    data_chunk blocks{{0x67, 0x00}};
    binary prefix2(value.size(), blocks);
    REQUIRE(prefix == prefix2);
}

TEST_CASE("bitfield test1", "[stealth]") {
    binary prefix("01100111001");
    data_chunk raw_bitfield{{0x67, 0x20, 0x00, 0x0}};
    REQUIRE(raw_bitfield.size() * 8 >= prefix.size());
    binary compare(prefix.size(), raw_bitfield);
    REQUIRE(prefix == compare);
}

TEST_CASE("bitfield test2", "[stealth]") {
    data_chunk const blocks{{0x8b, 0xf4, 0x1c, 0x69}};
    const binary prefix(27, blocks);
    data_chunk const raw_bitfield{{0x8b, 0xf4, 0x1c, 0x79}};
    REQUIRE(raw_bitfield.size() * 8 >= prefix.size());
    const binary compare(prefix.size(), raw_bitfield);
    REQUIRE(prefix == compare);
}

TEST_CASE("bitfield test3", "[stealth]") {
    data_chunk const blocks{{0x69, 0x1c, 0xf4, 0x8b}};
    const binary prefix(32, blocks);
    data_chunk const raw_bitfield{{0x69, 0x1c, 0xf4, 0x8b}};
    const binary compare(prefix.size(), raw_bitfield);
    REQUIRE(prefix == compare);
}

TEST_CASE("bitfield test4", "[stealth]") {
    data_chunk const blocks{{0x69, 0x1c, 0xf4, 0x8b}};
    const binary prefix(29, blocks);
    data_chunk const raw_bitfield{{0x69, 0x1c, 0xf4, 0x8b}};
    const binary compare(prefix.size(), raw_bitfield);
    REQUIRE(prefix == compare);
}

// End Test Suite
