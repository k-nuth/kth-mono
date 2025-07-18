// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/config/transaction.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/chain/transaction.hpp>
#include <kth/infrastructure/config/base16.hpp>

namespace kth::domain::config {

using namespace boost::program_options;
using namespace infrastructure::config;

transaction::transaction(std::string const& hexcode) {
    std::stringstream(hexcode) >> *this;
}

transaction::transaction(chain::transaction const& value)
    : value_(value) {
}

transaction::transaction(transaction const& x)
    : transaction(x.value_) {
}

chain::transaction& transaction::data() {
    return value_;
}

transaction::operator chain::transaction const&() const {
    return value_;
}

std::istream& operator>>(std::istream& input, transaction& argument) {
    std::string hexcode;
    input >> hexcode;

    data_chunk const bytes = base16(hexcode);
    byte_reader reader(bytes);
    auto transaction_exp = chain::transaction::from_data(reader, true);
    if ( ! transaction_exp) {
        BOOST_THROW_EXCEPTION(invalid_option_value(hexcode));
    }
    argument.value_ = std::move(*transaction_exp);
    return input;
}

std::ostream& operator<<(std::ostream& output, transaction const& argument) {
    output << base16(argument.value_.to_data());
    return output;
}

} // namespace kth::domain::config
