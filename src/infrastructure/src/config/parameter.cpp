// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/config/parameter.hpp>

#include <iostream>

#include <boost/algorithm/string.hpp>

#if ! defined(__EMSCRIPTEN__)
#include <boost/program_options.hpp>
#endif

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/utility/collection.hpp>
#include <kth/infrastructure/utility/string.hpp>

namespace po = boost::program_options;
using namespace kth::infrastructure::config;

int const parameter::not_positional = -1;
char const parameter::no_short_name = 0x00;
char const parameter::option_prefix_char = '-';

// 100% component coverage, common scenarios.
// A required argument may only be preceeded by required arguments.
// Requiredness may be in error if the metadata is inconsistent.
void parameter::initialize(const po::option_description& option, argument_list const& arguments) {
    set_position(position(option, arguments));
    set_args_limit(arguments_limit(get_position(), option, arguments));
    set_required(option.semantic()->is_required());
    set_long_name(option.long_name());
    set_short_name(short_name(option));
    set_description(option.description());
    set_format_name(option.format_name());
    set_format_parameter(option.format_parameter());
}

// 100% component coverage, all three scenarios (long, short, both)
int parameter::position(const po::option_description& option, argument_list const& arguments) const {
    return find_pair_position(arguments, option.long_name());
}

// 100% unit coverage, all three scenarios (long, short, both)
char parameter::short_name(const po::option_description& option) const {
    // This call requires boost 1.50, don't use it.
    //auto name = option.canonical_display_name(
    //    search_options::dashed_short_prefer_short);

    //TODO(fernando): check if that is necessary right now (we are using Boost 1.72.0 or newer)
    // This is a substitute that allows us to use boost 1.49.
    auto const name = split(option.format_name()).front();
    auto is_short_name = name[0] == option_prefix_char && name[1] != option_prefix_char;
    return is_short_name ? name[1] : no_short_name;
}

// 100% component coverage
unsigned parameter::arguments_limit(int position, const po::option_description& option, argument_list const& arguments) const {
    if (position == parameter::not_positional) {
        return option.semantic()->max_tokens();
    }

    return arguments[position].second;
}
