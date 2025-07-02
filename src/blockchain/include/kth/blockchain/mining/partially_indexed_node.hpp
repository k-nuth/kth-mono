// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_MINING_PARTIALLY_INDEXED_NODE_HPP_
#define KTH_BLOCKCHAIN_MINING_PARTIALLY_INDEXED_NODE_HPP_

#include <list>
#include <vector>

#include <kth/mining/common.hpp>

#include <kth/domain.hpp>

namespace kth {
namespace mining {

template <typename I, typename T>
    // requires(Regular<T>)
class partially_indexed_node {
public:
    using value_type = T;

    partially_indexed_node(I index, T const& x)
        : index_(index)
        , x_(x) {}

    partially_indexed_node(I index, T&& x)
        : index_(index)
        , x_(std::move(x)) {}

    // template <typename... Args>
    // partially_indexed_node(std::piecewise_construct_t, I index, Args&&... args)
    //     : index_(index)
    //     , x_(std::forward<Args>(args)...)
    // {}

    template <typename... Args>
    partially_indexed_node(I index, Args&&... args)
        : index_(index)
        , x_(std::forward<Args>(args)...) {}

    I const& index() const {
        return index_;
    }

    T& element() & {
        return x_;
    }

    T&& element() && {
        return std::move(x_);
    }

    T const& element() const& {
        return x_;
    }

    void set_index(I index) {
        index_ = index;
    }

    void set_element(T const& x) {
        x_ = x;
    }

    void set_element(T&& x) {
        x_ = std::move(x);
    }

private:
    I index_;
    T x_;
};

}  // namespace mining
}  // namespace kth

#endif  //KTH_BLOCKCHAIN_MINING_PARTIALLY_INDEXED_NODE_HPP_
