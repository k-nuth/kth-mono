// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/message/double_spend_proof.hpp>

#include <initializer_list>

// #include <kth/infrastructure/message/message_tools.hpp>
#include <kth/domain/message/version.hpp>
#include <kth/domain/multi_crypto_support.hpp>
#include <kth/infrastructure/math/sip_hash.hpp>
#include <kth/infrastructure/message/message_tools.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/limits.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::message {

std::string const double_spend_proof::command = "dsproof-beta";
uint32_t const double_spend_proof::version_minimum = version::level::minimum;
uint32_t const double_spend_proof::version_maximum = version::level::maximum;

double_spend_proof::double_spend_proof(chain::output_point const& out_point, spender const& spender1, spender const& spender2)
    : out_point_(out_point)
    , spender1_(spender1)
    , spender2_(spender2)
{}

bool double_spend_proof::is_valid() const {
    return out_point_.is_valid()
        && spender1_.is_valid()
        && spender2_.is_valid();
}

void double_spend_proof::reset() {
    out_point_.reset();
    spender1_.reset();
    spender2_.reset();
}

data_chunk double_spend_proof::to_data(size_t version) const {
    data_chunk data;
    auto const size = serialized_size(version);
    data.reserve(size);
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void double_spend_proof::to_data(size_t version, data_sink& stream) const {
    ostream_writer sink_w(stream);
    to_data(version, sink_w);
}

hash_digest double_spend_proof::hash() const {
    return sha256_hash(to_data(0));
}

[[nodiscard]]
chain::output_point const& double_spend_proof::out_point() const {
    return out_point_;
}

void double_spend_proof::set_out_point(chain::output_point const& x) {
    out_point_ = x;
}

[[nodiscard]]
double_spend_proof::spender const& double_spend_proof::spender1() const {
    return spender1_;
}

void double_spend_proof::set_spender1(double_spend_proof::spender const& x) {
    spender1_ = x;
}

[[nodiscard]]
double_spend_proof::spender const& double_spend_proof::spender2() const {
    return spender2_;
}

void double_spend_proof::set_spender2(double_spend_proof::spender const& x) {
    spender2_ = x;
}

hash_digest hash(double_spend_proof const& x) {
    return x.hash();
}

} // namespace kth::domain::message
