// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/wallet/ek_private.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/define.hpp>
#include <kth/infrastructure/formats/base_58.hpp>
#include <kth/infrastructure/math/checksum.hpp>

namespace kth::domain::wallet {

ek_private::ek_private()
    : private_()
{}

ek_private::ek_private(std::string const& encoded)
    : ek_private(from_string(encoded)) {
}

ek_private::ek_private(encrypted_private const& value)
    : valid_(true), private_(value) {
}

// Factories.
// ----------------------------------------------------------------------------

ek_private ek_private::from_string(std::string const& encoded) {
    // TODO(legacy): incorporate existing parser here, setting new members.

    encrypted_private key;
    return decode_base58(key, encoded) && verify_checksum(key) ? ek_private(key) : ek_private();
}

// Cast operators.
// ----------------------------------------------------------------------------

ek_private::operator bool() const {
    return valid_;
}

ek_private::operator encrypted_private const&() const {
    return private_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ek_private::encoded() const {
    return encode_base58(private_);
}

// Accessors.
// ----------------------------------------------------------------------------

encrypted_private const& ek_private::private_key() const {
    return private_;
}

// Operators.
// ----------------------------------------------------------------------------

bool ek_private::operator==(ek_private const& x) const {
    return valid_ == x.valid_ && private_ == x.private_;
}

bool ek_private::operator!=(ek_private const& x) const {
    return !(*this == x);
}

bool ek_private::operator<(ek_private const& x) const {
    return encoded() < x.encoded();
}

std::istream& operator>>(std::istream& in, ek_private& to) {
    std::string value;
    in >> value;
    to = ek_private(value);

    if ( ! to) {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, ek_private const& of) {
    out << of.encoded();
    return out;
}

} // namespace kth::domain::wallet
