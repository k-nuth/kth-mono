// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <optional>

#include <test_helpers.hpp>

using namespace kth;
using namespace kd;
using namespace kth::domain::wallet;

// Start Test Suite: bitcoin uri tests

// Constructors
// ----------------------------------------------------------------------------

TEST_CASE("bitcoin uri  construct  uninitialized  false", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri());
}

TEST_CASE("bitcoin uri  construct  initialized  true", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:"));
}

TEST_CASE("bitcoin uri  construct  scheme mixed case  normalized", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcOin:").encoded() == "bitcoin:");
}

TEST_CASE("bitcoin uri  construct  invalid scheme  false", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri("fedcoin:"));
}

TEST_CASE("bitcoin uri  construct  payment address only  false", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));
}

TEST_CASE("bitcoin uri  construct  stealth address only  false", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri("hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i"));
}

TEST_CASE("bitcoin uri  construct  fragment  false", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri("bitcoin:#satoshi"));
}

TEST_CASE("bitcoin uri  construct  strict  test", "[bitcoin uri]") {
    REQUIRE( ! bitcoin_uri("bitcoin:?label=Some テスト"));
}

TEST_CASE("bitcoin uri  construct  not strict  test", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?label=Some テスト", false));
}

// Setters
// ----------------------------------------------------------------------------

TEST_CASE("bitcoin uri  set path  payment address  expected encoding", "[bitcoin uri]") {
    auto const expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    REQUIRE(uri.set_path(expected_payment));
    REQUIRE(uri.encoded() == expected_uri);
}

TEST_CASE("bitcoin uri  set path  stealth address  expected encoding", "[bitcoin uri]") {
    auto const expected_payment = "hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    REQUIRE(uri.set_path(expected_payment));
    REQUIRE(uri.encoded() == expected_uri);
}

TEST_CASE("bitcoin uri  set path  reset stealth after payment  expected encoding", "[bitcoin uri]") {
    auto const expected_stealth = "hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i";
    auto const expected_uri = std::string("bitcoin:") + expected_stealth;

    bitcoin_uri uri;
    auto const payment = payment_address("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    REQUIRE(payment);
    uri.set_address(payment);
    uri.set_address(stealth_address(expected_stealth));
    REQUIRE(uri.encoded() == expected_uri);
}

TEST_CASE("bitcoin uri  set path  reset payment after stealth  expected encoding", "[bitcoin uri]") {
    auto const expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    auto const stealth = stealth_address("hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i");
    REQUIRE(stealth);
    uri.set_address(stealth);
    uri.set_address(payment_address(expected_payment));
    REQUIRE(uri.encoded() == expected_uri);
}

TEST_CASE("bitcoin uri  set path  reset path  false", "[bitcoin uri]") {
    auto const expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    uri.set_address(stealth_address("hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i"));

    // The set_path will not reset a path. This is necessary to catch failures in non-strict parsing.
    REQUIRE( ! uri.set_path(expected_payment));
}

TEST_CASE("bitcoin uri  set amount  reset amount  latter amount", "[bitcoin uri]") {
    bitcoin_uri uri;
    uri.set_amount(10000000000);
    uri.set_amount(120000);
    REQUIRE(uri.encoded() == "bitcoin:?amount=0.0012");
}

TEST_CASE("bitcoin uri  all setters  complex uri  expected encoding", "[bitcoin uri]") {
    bitcoin_uri uri;
    uri.set_path("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    uri.set_amount(120000);
    uri.set_label("&=\n");
    uri.set_message("hello bitcoin");
    uri.set_r("http://example.com?purchase=shoes&user=bob");

    REQUIRE(uri.encoded() ==
                "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
                "amount=0.0012&"
                "label=%26%3D%0A&"
                "message=hello%20bitcoin&"
                "r=http://example.com?purchase%3Dshoes%26user%3Dbob");
}

TEST_CASE("bitcoin uri  set parameter  amount denormalized  normalized", "[bitcoin uri]") {
    bitcoin_uri uri;
    REQUIRE(uri.set_parameter("amount", ".0012"));
    REQUIRE(uri.encoded() == "bitcoin:?amount=0.0012");
}

// Getters
// ----------------------------------------------------------------------------

TEST_CASE("bitcoin uri  amount  set  expected", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?amount=0.0012").amount() == 120000u);
}

TEST_CASE("bitcoin uri  label  escaped  expected", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?label=%26%3D%0A").label() == "&=\n");
}

TEST_CASE("bitcoin uri  message  escaped  expected", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?message=hello%20bitcoin").message() == "hello bitcoin");
}

TEST_CASE("bitcoin uri  r  escaped  expected", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?r=http://example.com?purchase%3Dshoes%26user%3Dbob").r() == "http://example.com?purchase=shoes&user=bob");
}

TEST_CASE("bitcoin uri  payment  valid  expected", "[bitcoin uri]") {
    auto const expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;
    REQUIRE(bitcoin_uri(expected_uri).payment().encoded_legacy() == expected_payment);
}

TEST_CASE("bitcoin uri  stealth  valid  expected", "[bitcoin uri]") {
    auto const expected_stealth = "hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i";
    auto const expected_uri = std::string("bitcoin:") + expected_stealth;
    REQUIRE(bitcoin_uri(expected_uri).stealth().encoded() == expected_stealth);
}

TEST_CASE("bitcoin uri  address  payment  expected", "[bitcoin uri]") {
    auto const expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    auto const expected_uri = std::string("bitcoin:") + expected_payment;
    REQUIRE(bitcoin_uri(expected_uri).address() == expected_payment);
}

TEST_CASE("bitcoin uri  address  stealth  expected", "[bitcoin uri]") {
    auto const expected_stealth = "hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i";
    auto const expected_uri = std::string("bitcoin:") + expected_stealth;
    REQUIRE(bitcoin_uri(expected_uri).address() == expected_stealth);
}

TEST_CASE("bitcoin uri  parameter amount  denormalized  normalized", "[bitcoin uri]") {
    REQUIRE(bitcoin_uri("bitcoin:?amount=.0012").parameter("amount") == "0.0012");
}

TEST_CASE("bitcoin uri  parameters all  complex uri  expected", "[bitcoin uri]") {
    bitcoin_uri uri(
        "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
        "amount=0.0012&"
        "label=%26%3D%0A&"
        "message=hello%20bitcoin&"
        "r=http://example.com?purchase%3Dshoes%26user%3Dbob");

    REQUIRE(uri.address() == "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    REQUIRE(uri.parameter("amount") == "0.0012");
    REQUIRE(uri.parameter("label") == "&=\n");
    REQUIRE(uri.parameter("message") == "hello bitcoin");
    REQUIRE(uri.parameter("r") == "http://example.com?purchase=shoes&user=bob");
}

// End Test Suite
