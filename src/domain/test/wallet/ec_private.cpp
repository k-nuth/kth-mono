// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;
using namespace kth::domain::wallet;

// Start Test Suite: ec private tests

// TODO(legacy): add version tests

#define SECRET "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define WIF_COMPRESSED "L1WepftUBemj6H4XQovkiW1ARVjxMqaw4oj2kmkYqdG1xTnBcHfC"
#define WIF_UNCOMPRESSED "5JngqQmHagNTknnCshzVUysLMWAjT23FWs1TgNU5wyFH5SB3hrP"

// TODO(legacy): implement testnet version tests
//#define WIF_COMPRESSED_TESTNET "cRseHatKciTzFiXnoDjt5pWE3j3N2Hgd8qsVsCD4Ljv2DCwuD1V6"
//#define WIF_UNCOMPRESSED_TESTNET "92ZKR9aqAuSbirHVW3tQMaRJ1AXScBaSrosQkzpbHhzKrVBsZBL"

TEST_CASE("ec private  compressed wif  compressed  test", "[ec private]") {
    REQUIRE(ec_private(WIF_COMPRESSED).compressed());
}

TEST_CASE("ec private  uncompressed wif  not compressed  test", "[ec private]") {
    REQUIRE( ! ec_private(WIF_UNCOMPRESSED).compressed());
}

TEST_CASE("ec private  encode wif  compressed  test", "[ec private]") {
    REQUIRE(ec_private(base16_literal(SECRET)).encoded() == WIF_COMPRESSED);
}

TEST_CASE("ec private  encode wif  uncompressed  test", "[ec private]") {
    REQUIRE(ec_private(base16_literal(SECRET), 0x8000, false).encoded() == WIF_UNCOMPRESSED);
}

TEST_CASE("ec private  decode wif  compressed  test", "[ec private]") {
    ec_private secret(WIF_COMPRESSED);
    REQUIRE(encode_base16(secret.secret()) == SECRET);
    REQUIRE(secret.version() == 0x8000);
    REQUIRE(secret.compressed());
}

TEST_CASE("ec private  decode wif  uncompressed  test", "[ec private]") {
    ec_private secret(WIF_UNCOMPRESSED);
    REQUIRE(encode_base16(secret.secret()) == SECRET);
    REQUIRE(secret.version() == 0x8000);
    REQUIRE( ! secret.compressed());
}

// End Test Suite
