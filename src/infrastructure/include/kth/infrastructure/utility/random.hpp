// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_RANDOM_HPP
#define KTH_INFRASTRUCTURE_RANDOM_HPP

#include <cstdint>

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/utility/asio.hpp>
#include <kth/infrastructure/utility/data.hpp>

namespace kth {

/**
 * Generate a pseudo random number within the domain.
 * @return  The 64 bit number.
 */
KI_API uint64_t pseudo_random_broken_do_not_use();

/**
 * Generate a pseudo random number within [begin, end].
 * @return  The 64 bit number.
 */
KI_API uint64_t pseudo_random_broken_do_not_use(uint64_t begin, uint64_t end);

/**
 * Fill a buffer with randomness using the default random engine.
 * @param[in]  chunk  The buffer to fill with randomness.
 */
KI_API void pseudo_random_broken_do_not_use_fill(data_chunk& chunk);

// /**
//  * Convert a time duration to a value in the range [max/ratio, max].
//  * @param[in]  maximum  The maximum value to return.
//  * @param[in]  ratio    The determinant of the minimum duration as the inverse
//  *                      portion of the maximum duration.
//  * @return              The randomized duration.
//  */
// KI_API asio::duration pseudo_randomize(asio::duration const& maximum,
//     uint8_t ratio=2);


/**
 * Fill a buffer with randomness using the default random engine.
 * @param[in]  chunk  The buffer to fill with randomness.
 */
KI_API void pseudo_random_fill(data_chunk& chunk);

/**
 * Fill a buffer with randomness using the default random engine.
 * @param[in]  out  The buffer to fill with randomness.
 * @param[in]  size  The size of the buffer to fill.
*/
KI_API void pseudo_random_fill(uint8_t* out, size_t size);


} // namespace kth

#endif
