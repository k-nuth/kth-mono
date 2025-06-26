// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/wallet/ek_public.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <kth/domain/define.hpp>
#include <kth/infrastructure/formats/base_58.hpp>
#include <kth/infrastructure/math/checksum.hpp>

namespace kth::domain::wallet {

ek_public::ek_public()
    : public_()
{}

ek_public::ek_public(std::string const& encoded)
    : ek_public(from_string(encoded)) {
}

ek_public::ek_public(encrypted_public const& value)
    : valid_(true), public_(value) {
}

// Factories.
// ----------------------------------------------------------------------------

ek_public ek_public::from_string(std::string const& encoded) {
    // TODO(legacy): incorporate existing parser here, setting new members.

    encrypted_public key;
    return decode_base58(key, encoded) && verify_checksum(key) ? ek_public(key) : ek_public();
}

// Cast operators.
// ----------------------------------------------------------------------------

ek_public::operator bool() const {
    return valid_;
}

ek_public::operator encrypted_public const&() const {
    return public_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ek_public::encoded() const {
    return encode_base58(public_);
}

// Accessors.
// ----------------------------------------------------------------------------

encrypted_public const& ek_public::public_key() const {
    return public_;
}

// Operators.
// ----------------------------------------------------------------------------

bool ek_public::operator<(ek_public const& x) const {
    return encoded() < x.encoded();
}

bool ek_public::operator==(ek_public const& x) const {
    return valid_ == x.valid_ && public_ == x.public_;
}

bool ek_public::operator!=(ek_public const& x) const {
    return !(*this == x);
}

std::istream& operator>>(std::istream& in, ek_public& to) {
    std::string value;
    in >> value;
    to = ek_public(value);

    if ( ! to) {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, ek_public const& of) {
    out << of.encoded();
    return out;
}

} // namespace kth::domain::wallet
