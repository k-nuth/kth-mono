// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>

#if ! defined(__EMSCRIPTEN__)
#include <boost/program_options.hpp>
#endif

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

namespace po = boost::program_options;
using namespace kth::infrastructure::config;

enum opt {
    short_long = 0,
    shorty,
    longy,
    simple,
    defaulty,
    required,
    toggled,
    vector,
    multitoken
};

static
void load_test_options(po::options_description& options) {
    using namespace std::filesystem;
    #if ! defined(__EMSCRIPTEN__)
using namespace boost::program_options;
#endif
    options.add_options()
        ("short_long,s", "Long and short name.")
        (",m", "Short name only.")
        ("longy", value<std::string>()->required(), "Long name only.")
        ("simple", value<std::string>(), "Simple string.")
        ("defaulty", value<bool>()->default_value(true), "Defaulted bool.")
        ("required", value<path>()->required(), "Required path.")
        ("toggled", value<bool>()->zero_tokens(), "Toggle only bool.")
        /* The enumerability of the data types does not control multiple,
           instance behavior but it is necessary to capture multiples. */
        ("VECTOR", value<std::vector<std::string>>(), "String vector.")
        ("multitoken", value<int>()->multitoken(), "Multi-token int.");
}

static
void load_test_arguments(argument_list& arguments) {
    arguments.push_back(argument_pair("longy", 1));
    arguments.push_back(argument_pair("simple", 2));
    arguments.push_back(argument_pair("defaulty", 3));
    arguments.push_back(argument_pair("required", 4));
    arguments.push_back(argument_pair("VECTOR", 5));
    arguments.push_back(argument_pair("multitoken", -1));
}

#define CONFIG_TEST_PARAMETER_SETUP(index) \
    po::options_description options; \
    load_test_options(options); \
    argument_list names; \
    load_test_arguments(names); \
    auto option = *(options.options()[index]); \
    kth::infrastructure::config::parameter parameter

#define CONFIG_TEST_PARAMETER_OPTIONS_SETUP(index) \
    po::options_description options; \
    load_test_options(options); \
    auto option = *(options.options()[index]); \
    kth::infrastructure::config::parameter parameter

// Start Test Suite: parameter tests

// ------------------------------------------------------------------------- //
// Start Test Suite: parameter  position

TEST_CASE("parameter  position  short and long  returns not positional", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::short_long);
    REQUIRE(parameter.position(option, names) == parameter::not_positional);
}

TEST_CASE("parameter  position  short only  returns not positional", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::shorty);
    REQUIRE(parameter.position(option, names) == parameter::not_positional);
}

TEST_CASE("parameter  position  long only  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::longy);
    REQUIRE(parameter.position(option, names) == 0);
}

TEST_CASE("parameter  position  simple  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::simple);
    REQUIRE(parameter.position(option, names) == 1);
}

TEST_CASE("parameter  position  defaulty  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::defaulty);
    REQUIRE(parameter.position(option, names) == 2);
}

TEST_CASE("parameter  position  required  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::required);
    REQUIRE(parameter.position(option, names) == 3);
}

TEST_CASE("parameter  position  toggled  returns not positional", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::toggled);
    REQUIRE(parameter.position(option, names) == parameter::not_positional);
}

TEST_CASE("parameter  position  vector  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::vector);
    REQUIRE(parameter.position(option, names) == 4);
}

TEST_CASE("parameter  position  multitoken  returns expected position", "[parameter  position]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::multitoken);
    REQUIRE(parameter.position(option, names) == 5);
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: parameter  initialize

TEST_CASE("parameter  initialize  short long  sets limit 0", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::short_long);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == parameter::not_positional);
    REQUIRE(parameter.get_args_limit() == 0u);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == 's');
    REQUIRE(parameter.get_long_name() == "short_long");
    REQUIRE(parameter.get_format_name() == "-s [ --short_long ]");
    REQUIRE(parameter.get_format_parameter() == "");
    REQUIRE(parameter.get_description() == "Long and short name.");
}

TEST_CASE("parameter  initialize  short only  sets limit 0", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::shorty);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == parameter::not_positional);
    REQUIRE(parameter.get_args_limit() == 0u);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == 'm');
    REQUIRE(parameter.get_long_name() == "");
    // A boost 1.49 bug causes this test failure:
    // parameter.get_format_name() == "-m" failed [-m [ -- ] != -m]
    // But this behavior is not critical to operations, so we overlook it.
    REQUIRE(parameter.get_format_name() == "-m");
    REQUIRE(parameter.get_format_parameter() == "");
    REQUIRE(parameter.get_description() == "Short name only.");
}

TEST_CASE("parameter  initialize  long only  sets limit 0", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::longy);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 0);
    REQUIRE(parameter.get_args_limit() == 1u);
    REQUIRE(parameter.get_required() == true);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "longy");
    REQUIRE(parameter.get_format_name() == "--longy");
    REQUIRE(parameter.get_format_parameter() == "arg");
    REQUIRE(parameter.get_description() == "Long name only.");
}

TEST_CASE("parameter  initialize  simple  sets limit 2", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::simple);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 1);
    REQUIRE(parameter.get_args_limit() == 2u);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "simple");
    REQUIRE(parameter.get_format_name() == "--simple");
    REQUIRE(parameter.get_format_parameter() == "arg");
    REQUIRE(parameter.get_description() == "Simple string.");
}

TEST_CASE("parameter  initialize  defaulted  sets limit 3", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::defaulty);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 2);
    REQUIRE(parameter.get_args_limit() == 3u);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "defaulty");
    REQUIRE(parameter.get_format_name() == "--defaulty");
    REQUIRE(parameter.get_description() == "Defaulted bool.");

    // The (=1) appears to be the default value (as int), i.e. (=true)
    // This makes the bool type indistinguishable at the metadata level from
    // an integer. So we must use zero_tokens() instead (see 'toggled' case).
    REQUIRE(parameter.get_format_parameter() == "arg (=1)");
}

TEST_CASE("parameter  initialize  required  sets limit 4", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::required);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 3);
    REQUIRE(parameter.get_args_limit() == 4u);
    REQUIRE(parameter.get_required() == true);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "required");
    REQUIRE(parameter.get_format_name() == "--required");
    REQUIRE(parameter.get_format_parameter() == "arg");
    REQUIRE(parameter.get_description() == "Required path.");
}

//
TEST_CASE("parameter  initialize  toggled  sets limit 0", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::toggled);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "toggled");
    REQUIRE(parameter.get_format_name() == "--toggled");
    REQUIRE(parameter.get_format_parameter() == "");
    REQUIRE(parameter.get_description() == "Toggle only bool.");

    // This combination uniquely implies that the option is a toggle.
    REQUIRE(parameter.get_position() == parameter::not_positional);
    REQUIRE(parameter.get_args_limit() == 0u);
}

TEST_CASE("parameter  initialize  vector  sets limit 1", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::vector);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 4);
    REQUIRE(parameter.get_args_limit() == 5u);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "VECTOR");
    REQUIRE(parameter.get_format_name() == "--VECTOR");
    REQUIRE(parameter.get_format_parameter() == "arg");
    REQUIRE(parameter.get_description() == "String vector.");
}

TEST_CASE("parameter  initialize  multitoken  sets unlimited", "[parameter  initialize]") {
    CONFIG_TEST_PARAMETER_SETUP(opt::multitoken);
    parameter.initialize(option, names);
    REQUIRE(parameter.get_position() == 5);
    /* The positional "unlimited" limit is different than the named limit. */
    REQUIRE(parameter.get_args_limit() == 0xFFFFFFFFu);
    REQUIRE(parameter.get_required() == false);
    REQUIRE(parameter.get_short_name() == '\0');
    REQUIRE(parameter.get_long_name() == "multitoken");
    REQUIRE(parameter.get_format_name() == "--multitoken");
    REQUIRE(parameter.get_format_parameter() == "arg");
    REQUIRE(parameter.get_description() == "Multi-token int.");
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: parameter  short name

TEST_CASE("parameter  short name  short and long  returns short", "[parameter  short name]") {
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::short_long);
    REQUIRE(parameter.short_name(option) == 's');
}

TEST_CASE("parameter  short name  short only  returns short", "[parameter  short name]") {
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::shorty);
    REQUIRE(parameter.short_name(option) == 'm');
}

TEST_CASE("parameter  short name  long only  returns null char", "[parameter  short name]") {
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::longy);
    REQUIRE(parameter.short_name(option) == parameter::no_short_name);
}

// End Test Suite

// End Test Suite
