// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;

// Start Test Suite: elliptic curve tests

// Scenario 1
#define SECRET1 "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
#define COMPRESSED1 "0309ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc732731"
#define UNCOMPRESSED1 "0409ba8621aefd3b6ba4ca6d11a4746e8df8d35d9b51b383338f627ba7fc7327318c3a6ec6acd33c36328b8fb4349b31671bcd3a192316ea4f6236ee1ae4a7d8c9"

// Scenario 2
#define COMPRESSED2 "03bc88a1bd6ebac38e9a9ed58eda735352ad10650e235499b7318315cc26c9b55b"
#define SIGHASH2 "ed8f9b40c2d349c8a7e58cebe79faa25c21b6bb85b874901f72a1b3f1ad0a67f"
#define SIGNATURE2 "3045022100bc494fbd09a8e77d8266e2abdea9aef08b9e71b451c7d8de9f63cda33a62437802206b93edd6af7c659db42c579eb34a3a4cb60c28b5a6bc86fd5266d42f6b8bb67d"

// Scenario 3
#define SECRET3 "ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333"
#define SIGHASH3 "f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0"

#define EC_SIGNATURE3 "17b7b25c48e4ed2bd43369fa282f608b4329d96409860ce832fd5497b65fe663b901e34dff5291868c4401c8c1c6ed23b1887139cc4cd6884f38b9d936356131"
#define DER_SIGNATURE3 "3044022063e65fb69754fd32e80c860964d929438b602f28fa6933d42bede4485cb2b717022031613536d9b9384f88d64ccc397188b123edc6c1c801448c869152ff4de301b9"

// #if defined(KTH_CURRENCY_BCH)
// #define EC_SIGNATURE3 "17b7b25c48e4ed2bd43369fa282f608b4329d96409860ce832fd5497b65fe663b901e34dff5291868c4401c8c1c6ed23b1887139cc4cd6884f38b9d936356131"
// #define DER_SIGNATURE3 "3044022063e65fb69754fd32e80c860964d929438b602f28fa6933d42bede4485cb2b717022031613536d9b9384f88d64ccc397188b123edc6c1c801448c869152ff4de301b9"
// #else
// #define EC_SIGNATURE3 "4832febef8b31c7c922a15cb4063a43ab69b099bba765e24facef50dfbb4d057928ed5c6b6886562c2fe6972fd7c7f462e557129067542cce6b37d72e5ea5037"
// #define DER_SIGNATURE3 "3044022057d0b4fb0df5cefa245e76ba9b099bb63aa46340cb152a927c1cb3f8befe324802203750eae5727db3e6cc4275062971552e467f7cfd7269fec2626588b6c6d58e92"
// #endif //KTH_CURRENCY_BCH


TEST_CASE("elliptic curve  secret to public  positive  test", "[elliptic curve tests]") {
    ec_compressed point;
    REQUIRE(secret_to_public(point, base16_literal(SECRET1)));
    REQUIRE(encode_base16(point) == COMPRESSED1);
}

TEST_CASE("elliptic curve  decompress  positive  test", "[elliptic curve tests]") {
    ec_uncompressed uncompressed;
    REQUIRE(decompress(uncompressed, base16_literal(COMPRESSED1)));
    REQUIRE(encode_base16(uncompressed) == UNCOMPRESSED1);
}

TEST_CASE("elliptic curve  sign  positive  test", "[elliptic curve tests]") {
    ec_signature signature;
    ec_secret const secret = hash_literal(SECRET3);
    hash_digest const sighash = hash_literal(SIGHASH3);
    REQUIRE(sign(signature, secret, sighash));

    auto const result = encode_base16(signature);
    REQUIRE(result == EC_SIGNATURE3);
}

TEST_CASE("elliptic curve  encode signature  positive  test", "[elliptic curve tests]") {
    der_signature out;
    const ec_signature signature = base16_literal(EC_SIGNATURE3);
    REQUIRE(encode_signature(out, signature));

    auto const result = encode_base16(out);
    REQUIRE(result == DER_SIGNATURE3);
}

TEST_CASE("elliptic curve  sign  round trip positive  test", "[elliptic curve tests]") {
    ec_compressed point;
    ec_signature signature;
    data_chunk const data{ 'd', 'a', 't', 'a' };
    hash_digest const hash = bitcoin_hash(data);
    ec_secret const secret = hash_literal(SECRET1);
    REQUIRE(secret_to_public(point, secret));
    REQUIRE(sign(signature, secret, hash));
    REQUIRE(verify_signature(point, hash, signature));
}

TEST_CASE("elliptic curve  sign  round trip negative  test", "[elliptic curve tests]") {
    ec_compressed point;
    ec_signature signature;
    data_chunk const data{ 'd', 'a', 't', 'a' };
    hash_digest hash = bitcoin_hash(data);
    ec_secret const secret = base16_literal(SECRET1);
    REQUIRE(secret_to_public(point, secret));
    REQUIRE(sign(signature, secret, hash));

    // Invalidate the positive test.
    hash[0] = 0;
    REQUIRE( ! verify_signature(point, hash, signature));
}

TEST_CASE("elliptic curve  verify signature  positive  test", "[elliptic curve tests]") {
    ec_signature signature;
    static auto const strict = false;
    hash_digest const sighash = hash_literal(SIGHASH2);
    const ec_compressed point = base16_literal(COMPRESSED2);
    der_signature distinguished;
    REQUIRE(decode_base16(distinguished, SIGNATURE2));
    REQUIRE(parse_signature(signature, distinguished, strict));
    REQUIRE(verify_signature(point, sighash, signature));
}

TEST_CASE("elliptic curve  verify signature  negative  test", "[elliptic curve tests]") {
    ec_signature signature;
    static auto const strict = false;
    hash_digest const sighash = hash_literal(SIGHASH2);
    const ec_compressed point = base16_literal(COMPRESSED2);
    der_signature distinguished;
    REQUIRE(decode_base16(distinguished, SIGNATURE2));
    REQUIRE(parse_signature(signature, distinguished, strict));

    // Invalidate the positive test.
    signature[10] = 110;
    REQUIRE( ! verify_signature(point, sighash, signature));
}

TEST_CASE("elliptic curve  ec add  positive  test", "[elliptic curve tests]") {
    ec_secret secret1{ { 1, 2, 3 } };
    ec_secret const secret2{ { 3, 2, 1 } };
    ec_compressed public1;
    REQUIRE(secret_to_public(public1, secret1));
    REQUIRE(ec_add(secret1, secret2));
    REQUIRE(encode_base16(secret1) == "0404040000000000000000000000000000000000000000000000000000000000");

    ec_compressed public2;
    REQUIRE(secret_to_public(public2, secret1));
    REQUIRE(ec_add(public1, secret2));
    REQUIRE(std::equal(public1.begin(), public1.end(), public2.begin()));
}

TEST_CASE("elliptic curve  ec add  negative  test", "[elliptic curve tests]") {
    // = n - 1
    ec_secret secret1 = base16_literal("fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364140");
    ec_secret secret2{ { 0 } };
    secret2[31] = 1;
    ec_compressed public1;
    REQUIRE(secret_to_public(public1, secret1));
    REQUIRE( ! ec_add(secret1, secret2));
    REQUIRE( ! ec_add(public1, secret2));
}

TEST_CASE("elliptic curve  ec multiply test", "[elliptic curve tests]") {
    ec_secret secret1{{0}};
    ec_secret secret2{{0}};
    secret1[31] = 11;
    secret2[31] = 22;
    ec_compressed public1;
    REQUIRE(secret_to_public(public1, secret1));
    REQUIRE(ec_multiply(secret1, secret2));
    REQUIRE(secret1[31] == 242u);
    REQUIRE(ec_multiply(public1, secret2));

    ec_compressed public2;
    REQUIRE(secret_to_public(public2, secret1));
    REQUIRE(std::equal(public1.begin(), public1.end(), public2.begin()));
}

// End Test Suite
