// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cstdint>
#include <numeric>

#include <kth/domain/chain/point_value.hpp>
#include <kth/domain/chain/points_value.hpp>
#include <kth/infrastructure/utility/limits.hpp>

namespace kth::domain::chain {

uint64_t points_value::value() const {
    auto const sum = [](uint64_t total, point_value const& point) {
        return *safe_add(total, point.value());
    };

    return std::accumulate(points.begin(), points.end(), uint64_t(0), sum);
}

} // namespace kth::domain::chain
