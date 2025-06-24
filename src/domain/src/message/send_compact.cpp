// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/message/send_compact.hpp>

#include <cstdint>

#include <kth/domain/message/version.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::message {

std::string const send_compact::command = "sendcmpct";
uint32_t const send_compact::version_minimum = version::level::bip152;
uint32_t const send_compact::version_maximum = version::level::bip152;

size_t send_compact::satoshi_fixed_size(uint32_t /*version*/) {
    return 9;
}

send_compact::send_compact(bool high_bandwidth_mode, uint64_t version)
    : high_bandwidth_mode_(high_bandwidth_mode),
      version_(version) {
}

bool send_compact::operator==(send_compact const& x) const {
    return (high_bandwidth_mode_ == x.high_bandwidth_mode_) &&
           (version_ == x.version_);
}

bool send_compact::operator!=(send_compact const& x) const {
    return !(*this == x);
}

bool send_compact::is_valid() const {
    return (version_ != 0);
}

void send_compact::reset() {
    high_bandwidth_mode_ = false;
    version_ = 0;
}

data_chunk send_compact::to_data(uint32_t version) const {
    data_chunk data;
    auto const size = serialized_size(version);
    data.reserve(size);
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    KTH_ASSERT(data.size() == size);
    return data;
}

void send_compact::to_data(uint32_t version, data_sink& stream) const {
    ostream_writer sink_w(stream);
    to_data(version, sink_w);
}

size_t send_compact::serialized_size(uint32_t version) const {
    return send_compact::satoshi_fixed_size(version);
}

bool send_compact::high_bandwidth_mode() const {
    return high_bandwidth_mode_;
}

void send_compact::set_high_bandwidth_mode(bool mode) {
    high_bandwidth_mode_ = mode;
}

uint64_t send_compact::version() const {
    return version_;
}

void send_compact::set_version(uint64_t version) {
    version_ = version;
}

} // namespace kth::domain::message
