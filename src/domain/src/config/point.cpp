// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/config/point.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/chain/output_point.hpp>
#include <kth/infrastructure/config/hash256.hpp>
#include <kth/infrastructure/math/hash.hpp>
#include <kth/infrastructure/utility/string.hpp>

namespace kth::domain::config {

using namespace boost::program_options;
using namespace infrastructure::config;

// point format is currently private to bx.
static
bool decode_point(chain::output_point& point, std::string const& tuple) {
    auto const tokens = split(tuple, point::delimeter);
    if (tokens.size() != 2) {
        return false;
    }

    // validate and deserialize the transaction hash
    hash256 const digest(tokens[0]);
    hash_digest const& txhash = digest;

    // copy the input point values
    std::copy(txhash.begin(), txhash.end(), point.hash().begin());
    point.set_index(deserialize<uint32_t>(tokens[1], true));

    return true;
}

// point format is currently private to bx.
static
std::string encode_point(const chain::output_point& point) {
    std::stringstream result;
    result << hash256(point.hash()) << point::delimeter << point.index();
    return result.str();
}

std::string const point::delimeter = ":";

point::point(std::string const& tuple) {
    std::stringstream(tuple) >> *this;
}

point::point(const chain::output_point& value)
    : value_(value) {
}

point::point(point const& x)
    : point(x.value_) {
}

point::operator const chain::output_point&() const {
    return value_;
}

std::istream& operator>>(std::istream& input, point& argument) {
    std::string tuple;
    input >> tuple;

    if ( ! decode_point(argument.value_, tuple)) {
        BOOST_THROW_EXCEPTION(invalid_option_value(tuple));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, point const& argument) {
    output << encode_point(argument.value_);
    return output;
}

} // namespace kth::domain::config
