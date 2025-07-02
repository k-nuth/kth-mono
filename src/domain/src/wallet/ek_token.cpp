// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/wallet/ek_token.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/define.hpp>
#include <kth/infrastructure/formats/base_58.hpp>
#include <kth/infrastructure/math/checksum.hpp>

namespace kth::domain::wallet {

ek_token::ek_token()
    : token_()
{}

ek_token::ek_token(std::string const& encoded)
    : ek_token(from_string(encoded)) {
}

ek_token::ek_token(encrypted_token const& value)
    : valid_(true), token_(value) {
}

// Factories.
// ----------------------------------------------------------------------------

ek_token ek_token::from_string(std::string const& encoded) {
    // TODO(legacy): incorporate existing parser here, setting new members.

    encrypted_token key;
    return decode_base58(key, encoded) && verify_checksum(key) ? ek_token(key) : ek_token();
}

// Cast operators.
// ----------------------------------------------------------------------------

ek_token::operator bool() const {
    return valid_;
}

ek_token::operator encrypted_token const&() const {
    return token_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ek_token::encoded() const {
    return encode_base58(token_);
}

// Accessors.
// ----------------------------------------------------------------------------

encrypted_token const& ek_token::token() const {
    return token_;
}

// Operators.
// ----------------------------------------------------------------------------

bool ek_token::operator==(ek_token const& x) const {
    return valid_ == x.valid_ && token_ == x.token_;
}

bool ek_token::operator!=(ek_token const& x) const {
    return !(*this == x);
}

bool ek_token::operator<(ek_token const& x) const {
    return encoded() < x.encoded();
}

std::istream& operator>>(std::istream& in, ek_token& to) {
    std::string value;
    in >> value;
    to = ek_token(value);

    if ( ! to) {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, ek_token const& of) {
    out << of.encoded();
    return out;
}

} // namespace kth::domain::wallet
