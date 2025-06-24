// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ARRAY_SLICE_HPP
#define KTH_INFRASTRUCTURE_ARRAY_SLICE_HPP

#include <cstddef>

namespace kth {

/**
 * An read-only STL-style wrapper for array-style collections.
 *
 * This class allows std::array, std::vector, std::string, and c-style arrays
 * to be used interchangeably in functions that expect an iterable container.
 * This is not a substitute for move overloads. Consider also that a contiguous
 * buffer (c-style array) is more performant than the iterator abstraction.
 */
template <typename Iterable>
class array_slice {
public:
    //implicit
    template <typename Container>
    array_slice(Container const& container);        //NOLINT

    array_slice(Iterable const* begin, Iterable const* end);

    Iterable const* begin() const;
    Iterable const* end() const;
    Iterable const* data() const;
    std::size_t size() const;
    bool empty() const;

private:
    Iterable const* begin_;
    Iterable const* end_;
};

} // namespace kth

#include <kth/infrastructure/impl/utility/array_slice.ipp>

#endif

