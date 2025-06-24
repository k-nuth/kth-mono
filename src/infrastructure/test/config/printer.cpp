// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <filesystem>
#include <iostream>
#include <vector>

#if ! defined(__EMSCRIPTEN__)
#include <boost/program_options.hpp>
#endif

#include <test_helpers.hpp>
#include <kth/infrastructure.hpp>

namespace po = boost::program_options;
using namespace kth::infrastructure::config;

// Start Test Suite: printer tests

#define CONFIG_APPLICATION "APP"
#define CONFIG_DESCRIPTION "DESCRIPTION"
#define CONFIG_COMMAND "COMMAND"
//#define CONFIG_CATEGORY "CATEGORY"

#define CONFIG_PRINTER_SETUP_ARGUMENTS(initializer) \
    po::options_description options; \
    po::positional_options_description arguments; \
    initializer; \
    printer help(options, arguments, CONFIG_APPLICATION, CONFIG_DESCRIPTION, CONFIG_COMMAND)

#define CONFIG_PRINTER_SETUP() \
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options())

#define CONFIG_PRINTER_INITIALIZE(number_of_parameters, number_of_names) \
    help.initialize(); \
    REQUIRE(help.get_parameters().size() == number_of_parameters); \
    REQUIRE(help.get_argument_names().size() == number_of_names)

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  columnize

TEST_CASE("printer  columnize  paragraph empty width 0  empty", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("", 0);
    REQUIRE(rows.size() == 0u);
}

TEST_CASE("printer  columnize  paragraph empty width 1  empty", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("", 1);
    REQUIRE(rows.size() == 0u);
}

TEST_CASE("printer  columnize  short word width 10  one word row", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("foo", 10);
    REQUIRE(rows.size() == 1u);
    REQUIRE(rows.front() == "foo");
}

TEST_CASE("printer  columnize  two short words width 10  two word row", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("foo bar", 10);
    REQUIRE(rows.size() == 1u);
    REQUIRE(rows.front() == "foo bar");
}

TEST_CASE("printer  columnize  overflow width 10  two rows", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("foo bar overflow", 10);
    REQUIRE(rows.size() == 2u);
    REQUIRE(rows[0] == "foo bar");
    REQUIRE(rows[1] == "overflow");
}

TEST_CASE("printer  columnize  first word overflow width 10  two rows", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("morethantenchars foo bar", 10);
    REQUIRE(rows.size() == 2u);
    REQUIRE(rows[0] == "morethantenchars");
    REQUIRE(rows[1] == "foo bar");
}

TEST_CASE("printer  columnize  middle word overflow width 10  three rows", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("foo bar morethantenchars test", 10);
    REQUIRE(rows.size() == 3u);
    REQUIRE(rows[0] == "foo bar");
    REQUIRE(rows[1] == "morethantenchars");
    REQUIRE(rows[2] == "test");
}

TEST_CASE("printer  columnize  last word overflow width 10  two rows", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("foo bar morethantenchars", 10);
    REQUIRE(rows.size() == 2u);
    REQUIRE(rows[0] == "foo bar");
    REQUIRE(rows[1] == "morethantenchars");
}

TEST_CASE("printer  columnize  excess whitespace width 10  space removed", "[printer  columnize]") {
    CONFIG_PRINTER_SETUP();
    auto rows = help.columnize("  \tfoo   bar \n\n  morethantenchars\r\n  ", 10);
    REQUIRE(rows.size() == 3u);
    REQUIRE(rows[0] == "\tfoo bar");
    REQUIRE(rows[1] == "\n\n");
    REQUIRE(rows[2] == "morethantenchars\r\n");
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  format parameters table

TEST_CASE("printer  format parameters table  positional empty  empty", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP();
    REQUIRE(help.format_parameters_table(true) == "");
}

TEST_CASE("printer  format parameters table  named empty  empty", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP();
    REQUIRE(help.format_parameters_table(false) == "");
}

TEST_CASE("printer  format parameters table  named three options  three options", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("long", "Long name only.")
        (",m", "Short name only.")
        ("short_long,s", "Long and short name."));

    CONFIG_PRINTER_INITIALIZE(3u, 0u);

    REQUIRE(help.format_parameters_table(false) ==
        "--long               Long name only.                                     \n"
        "-m                   Short name only.                                    \n"
        "-s [--short_long]    Long and short name.                                \n"
    );
}

TEST_CASE("printer  format parameters table  reversed named three options  three sorted options", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("short_long,s", "Long and short name.")
        (",m", "Short name only.")
        ("long", "Long name only."));
    CONFIG_PRINTER_INITIALIZE(3u, 0u);
    REQUIRE(help.format_parameters_table(false) ==
        "--long               Long name only.                                     \n"
        "-m                   Short name only.                                    \n"
        "-s [--short_long]    Long and short name.                                \n"
    );
}

TEST_CASE("printer  format parameters table  unsorted named three options no matching arguments  three sorted options", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("second,x", "Second option description.")
        ("third", "Third option description.");
        arguments.add("forty-two", 42);
        arguments.add("THIRD", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 2u);
    REQUIRE(help.format_parameters_table(false) ==
        "--third              Third option description.                           \n"
        "-f [--first]         First option description.                           \n"
        "-x [--second]        Second option description.                          \n"
    );
}

TEST_CASE("printer  format parameters table  named three options two matching arguments  one option", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("second,x", "Second option description.")
        ("THIRD", "Third option description.");
        arguments.add("FIRST", 1);
        arguments.add("second", 42);
        arguments.add("THIRD", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 3u);
    REQUIRE(help.format_parameters_table(false) ==
        "-f [--first]         First option description.                           \n"
    );
}

TEST_CASE("printer  format parameters table  positional three options  empty", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("long", "Long name only.")
        ("short_long,s", "Long and short name.")
        (",m", "Short name only."));
    CONFIG_PRINTER_INITIALIZE(3u, 0u);
    REQUIRE(help.format_parameters_table(true) == "");
}

TEST_CASE("printer  format parameters table  positional three options one matching argument  one argument", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("second,x", "Second option description.")
        ("THIRD", "Third option description.");
        arguments.add("FIRST", 1);
        arguments.add("SECOND", 42);
        arguments.add("THIRD", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 3u);
    REQUIRE(help.format_parameters_table(true) ==
        "THIRD                Third option description.                           \n"
    );
}

TEST_CASE("printer  format parameters table  reverse positional three options three matching arguments  three unsorted arguments", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("third", "Third option description.")
        ("SECOND", "Second option description.")
        ("FIRST", "First option description.");
        arguments.add("FIRST", 1);
        arguments.add("SECOND", 42);
        arguments.add("third", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 3u);
    REQUIRE(help.format_parameters_table(true) ==
        "THIRD                Third option description.                           \n"
        "SECOND               Second option description.                          \n"
        "FIRST                First option description.                           \n"
    );
}

TEST_CASE("printer  format parameters table  positional three options two matching arguments overflow  two arguments overflow", "[printer  format parameters table]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("SECOND,x",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor "
            "in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum.")
        ("THIRD", "Third option description.");
    arguments.add("FIRST", 1);
    arguments.add("SECOND", 42);
    arguments.add("THIRD", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 3u);
    auto table = help.format_parameters_table(true);
    REQUIRE(table ==
        "SECOND               Lorem ipsum dolor sit amet, consectetur adipiscing  \n"
        "                     elit, sed do eiusmod tempor incididunt ut labore et \n"
        "                     dolore magna aliqua. Ut enim ad minim veniam, quis  \n"
        "                     nostrud exercitation ullamco laboris nisi ut aliquip\n"
        "                     ex ea commodo consequat. Duis aute irure dolor in   \n"
        "                     reprehenderit in voluptate velit esse cillum dolore \n"
        "                     eu fugiat nulla pariatur. Excepteur sint occaecat   \n"
        "                     cupidatat non proident, sunt in culpa qui officia   \n"
        "                     deserunt mollit anim id est laborum.                \n"
        "THIRD                Third option description.                           \n"
    );
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  format usage parameters

TEST_CASE("printer  format usage parameters  unsorted two options one arg  sorted", "[printer  format usage parameters]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("second,x", "Second option description.")
        ("first,f", "First option description.")
        ("THIRD", "Third option description.");
        arguments.add("FIRST", 1);
        arguments.add("SECOND", 42);
        arguments.add("THIRD", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 3u);
    REQUIRE(help.format_usage_parameters() == "[-fx] [THIRD]...");
}

TEST_CASE("printer  format usage parameters  unsorted multiple parameters  sorted parameters", "[printer  format usage parameters]") {
    using namespace std::filesystem;
    #if ! defined(__EMSCRIPTEN__)
using namespace boost::program_options;
#endif
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("short_long,s", "Long and short name.")
        (",m", "Short name only.")
        ("longy", value<int>()->required(), "Long name only.")
        ("SIMPLE", value<std::string>(), "Simple string.")
        ("defaulty", value<bool>()->default_value(true), "Defaulted bool.")
        ("required", value<path>()->required(), "Required path.")
        ("untoggled", value<bool>()->zero_tokens(), "Zero token but not short.")
        ("toggled,t", value<bool>()->zero_tokens(), "Toggled, zero token and short.")
        ("ARRAY", value<std::vector<std::string>>(), "String vector.")
        ("multy", value<int>()->multitoken(), "Multi-token int.");
        arguments.add("required", 1);
        arguments.add("SIMPLE", 1);
        arguments.add("ARRAY", -1));
    CONFIG_PRINTER_INITIALIZE(10u, 3u);
    REQUIRE(help.format_usage_parameters() == "[-mst] --longy value [--untoggled] [--defaulty value] [--multy value]... REQUIRED [SIMPLE] [ARRAY]...");
}
// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  generate argument names

#define BX_PRINTER_GENERATE_ARGUMENT_NAMES(number_of_names) \
    help.generate_argument_names(); \
    REQUIRE(help.get_argument_names().size() == number_of_names)

TEST_CASE("printer  generate argument names  empty arguments empty options  empty", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP();
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(0u);
}

TEST_CASE("printer  generate argument names  empty arguments multiple options  empty", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("long", "Long name only.")
        ("short_long,s", "Long and short name.")
        (",m", "Short name only."));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(0u);
}

TEST_CASE("printer  generate argument names  one argument 1  one name 1", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(arguments.add("one", 1));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(1u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "one");
    REQUIRE(names[0].second == 1);
}

TEST_CASE("printer  generate argument names  one argument 42  one name 42", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(arguments.add("forty-two", 42));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(1u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "forty-two");
    REQUIRE(names[0].second == 42);
}

TEST_CASE("printer  generate argument names  one argument max arguments  one name max arguments", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(arguments.add("max_arguments", printer::max_arguments));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(1u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "max_arguments");
    REQUIRE(names[0].second == printer::max_arguments);
}

TEST_CASE("printer  generate argument names  one argument max arguments plus 1  one name negative 1", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(arguments.add("max_arguments+1", printer::max_arguments + 1));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(1u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "max_arguments+1");
    REQUIRE(names[0].second == -1);
}

TEST_CASE("printer  generate argument names  one argument negative 1  one name negative 1", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(arguments.add("negative-one", -1));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(1u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "negative-one");
    REQUIRE(names[0].second == -1);
}

TEST_CASE("printer  generate argument names  multiple arguments  expected names", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(
        arguments.add("forty-two", 42);
        arguments.add("max_arguments", printer::max_arguments));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(2u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "forty-two");
    REQUIRE(names[0].second == 42);
    REQUIRE(names[1].first == "max_arguments");
    REQUIRE(names[1].second == printer::max_arguments);
}

TEST_CASE("printer  generate argument names  multiple arguments negative 1  expected names", "[printer  generate argument names]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(
        arguments.add("forty-two", 42);
        arguments.add("max_arguments", printer::max_arguments);
        arguments.add("negative-one", -1));
    BX_PRINTER_GENERATE_ARGUMENT_NAMES(3u);
    auto& names = help.get_argument_names();
    REQUIRE(names[0].first == "forty-two");
    REQUIRE(names[0].second == 42);
    REQUIRE(names[1].first == "max_arguments");
    REQUIRE(names[1].second == printer::max_arguments);
    REQUIRE(names[2].first == "negative-one");
    REQUIRE(names[2].second == -1);
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  generate parameters

#define BX_PRINTER_GENERATE_PARAMETERS(number_of_parameters) \
    help.generate_parameters(); \
    REQUIRE(help.get_parameters().size() == number_of_parameters)

TEST_CASE("printer  generate parameters  empty  empty parameters", "[printer  generate parameters]") {
    CONFIG_PRINTER_SETUP();
    BX_PRINTER_GENERATE_PARAMETERS(0u);
}

TEST_CASE("printer  generate parameters  one option  expected parameter", "[printer  generate parameters]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("foo_bar,f", "Foobar option name."));
    BX_PRINTER_GENERATE_PARAMETERS(1u);
    auto& parameters = help.get_parameters();
    REQUIRE(parameters[0].get_short_name() == 'f');
}

TEST_CASE("printer  generate parameters  unsorted three options  expected sorted parameters", "[printer  generate parameters]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("second,x", "Second option description.")
        ("third", "Third option description."));
    BX_PRINTER_GENERATE_PARAMETERS(3u);
    auto& parameters = help.get_parameters();
    REQUIRE(parameters[0].get_long_name() == "third");
    REQUIRE(parameters[1].get_short_name() == 'f');
    REQUIRE(parameters[2].get_description() == "Second option description.");
}

// End Test Suite

// ------------------------------------------------------------------------- //
// Start Test Suite: printer  initialize

TEST_CASE("printer  initialize  unsorted multitple options  expected sorted parameters", "[printer  initialize]") {
    CONFIG_PRINTER_SETUP_ARGUMENTS(options.add_options()
        ("first,f", "First option description.")
        ("second,x", "Second option description.")
        ("third", "Third option description.");
        arguments.add("forty-two", 42);
        arguments.add("negative-one", -1));
    CONFIG_PRINTER_INITIALIZE(3u, 2u);
    auto& names = help.get_argument_names();
    auto& parameters = help.get_parameters();
    REQUIRE(names[0].first == "forty-two");
    REQUIRE(names[0].second == 42);
    REQUIRE(names[1].first == "negative-one");
    REQUIRE(names[1].second == -1);
    REQUIRE(parameters[0].get_long_name() == "third");
    REQUIRE(parameters[1].get_short_name() == 'f');
    REQUIRE(parameters[2].get_description() == "Second option description.");
}

// End Test Suite

// End Test Suite
