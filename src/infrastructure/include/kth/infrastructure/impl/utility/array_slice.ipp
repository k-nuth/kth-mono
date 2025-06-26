// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ARRAY_SLICE_IPP
#define KTH_INFRASTRUCTURE_ARRAY_SLICE_IPP

namespace kth {

template <typename Iterable>
template <typename Container>
array_slice<Iterable>::array_slice(const Container& container)
    : begin_(container.data())
    , end_(container.data() + container.size())
{}

template <typename Iterable>
array_slice<Iterable>::array_slice(Iterable const* begin, Iterable const* end)
    : begin_(begin), end_(end)
{}

template <typename Iterable>
Iterable const* array_slice<Iterable>::begin() const {
    return begin_;
}

template <typename Iterable>
Iterable const* array_slice<Iterable>::end() const {
    return end_;
}

template <typename Iterable>
Iterable const* array_slice<Iterable>::data() const {
    return begin_;
}

template <typename Iterable>
std::size_t array_slice<Iterable>::size() const {
    return end_ - begin_;
}

template <typename Iterable>
bool array_slice<Iterable>::empty() const {
    return end_ == begin_;
}

} // namespace kth

#endif

