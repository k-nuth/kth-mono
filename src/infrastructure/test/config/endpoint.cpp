// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <sstream>

#if ! defined(__EMSCRIPTEN__)
#include <boost/program_options.hpp>
#endif

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

using namespace kth;
using namespace kth::infrastructure::config;
#if ! defined(__EMSCRIPTEN__)
using namespace boost::program_options;
#endif

// Start Test Suite: endpoint tests

// ------------------------------------------------------------------------- //

// Start Test Suite: endpoint  construct

//TODO(fernando): fix this test
// TEST_CASE("endpoint  construct  empty  throws invalid option", "[endpoint  construct]") {
//     REQUIRE_THROWS_AS([](){endpoint url("");}, invalid_option_value);
// }

//TODO(fernando): fix this test
// TEST_CASE("endpoint  construct  no host  throws invalid option value", "[endpoint  construct]") {
//     REQUIRE_THROWS_AS([](){endpoint host("tcp://");}, invalid_option_value);
// }

//TODO(fernando): fix this test
// TEST_CASE("endpoint  construct  port only  throws invalid option value", "[endpoint  construct]") {
//     REQUIRE_THROWS_AS([](){endpoint host(":42");}, invalid_option_value);
// }

TEST_CASE("endpoint  construct  default  localhost", "[endpoint  construct]") {
    endpoint host;
    REQUIRE(host.scheme() == "");
    REQUIRE(host.host() == "localhost");
    REQUIRE(host.port() == 0u);
}

TEST_CASE("endpoint  construct  host  expected values", "[endpoint  construct]") {
    endpoint host("foo");
    REQUIRE(host.scheme() == "");
    REQUIRE(host.host() == "foo");
    REQUIRE(host.port() == 0u);
}

TEST_CASE("endpoint  construct  host port  expected values", "[endpoint  construct]") {
    endpoint endpoint("foo.bar:42");
    REQUIRE(endpoint.scheme() == "");
    REQUIRE(endpoint.host() == "foo.bar");
    REQUIRE(endpoint.port() == 42u);
}

TEST_CASE("endpoint  construct  scheme host port  expected values", "[endpoint  construct]") {
    endpoint host("tcp://foo.bar:42");
    REQUIRE(host.scheme() == "tcp");
    REQUIRE(host.host() == "foo.bar");
    REQUIRE(host.port() == 42u);
}

TEST_CASE("endpoint  construct  scheme host  expected values", "[endpoint  construct]") {
    endpoint host("tcp://foo.bar");
    REQUIRE(host.scheme() == "tcp");
    REQUIRE(host.host() == "foo.bar");
    REQUIRE(host.port() == 0u);
}

// End Test Suite

// End Test Suite
