// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/utility/pseudo_random.hpp>

#include <chrono>
#include <cstdint>
#include <random>

#include <boost/thread.hpp>

#include <kth/infrastructure/utility/asio.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/thread.hpp>

namespace kth {

using namespace kth::asio;
using namespace std::chrono;

void pseudo_random_fill(data_chunk& out) {
    return pseudo_random::fill<data_chunk>(out);
}

void pseudo_random_fill(uint8_t* out, size_t size) {
    return pseudo_random::fill(out, size);
}

} // namespace kth
