// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/config/input.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/chain/input.hpp>
#include <kth/domain/chain/input_point.hpp>
#include <kth/domain/config/point.hpp>
#include <kth/infrastructure/utility/string.hpp>

namespace kth::domain::config {

using namespace boost::program_options;

// input is currently a private encoding in bx.
static
bool decode_input(chain::input& input, std::string const& tuple) {
    auto const tokens = split(tuple, point::delimeter);
    if (tokens.size() != 2 && tokens.size() != 3) {
        return false;
    }

    input.set_sequence(max_input_sequence);
    input.set_previous_output(point(tokens[0] + ":" + tokens[1]));

    if (tokens.size() == 3) {
        input.set_sequence(deserialize<uint32_t>(tokens[2], true));
    }

    return true;
}

// input is currently a private encoding in bx.
static
std::string encode_input(chain::input const& input) {
    std::stringstream result;
    result << point(input.previous_output()) << point::delimeter
           << input.sequence();

    return result.str();
}

input::input(std::string const& tuple) {
    std::stringstream(tuple) >> *this;
}

input::input(chain::input const& value)
    : value_(value) {}

input::input(input const& x)
    : input(x.value_) {}

input::input(chain::input_point const& value)
    : value_({value, {}, max_input_sequence}) {}

input::operator chain::input const&() const {
    return value_;
}

std::istream& operator>>(std::istream& stream, input& argument) {
    std::string tuple;
    stream >> tuple;

    if ( ! decode_input(argument.value_, tuple)) {
        BOOST_THROW_EXCEPTION(invalid_option_value(tuple));
    }

    return stream;
}

std::ostream& operator<<(std::ostream& output, input const& argument) {
    output << encode_input(argument.value_);
    return output;
}

} // namespace kth::domain::config
