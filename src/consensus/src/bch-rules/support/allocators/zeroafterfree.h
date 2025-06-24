// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017-2021 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <support/cleanse.h>

#include <memory>
#include <vector>

template <typename T>
struct zero_after_free_allocator : public std::allocator<T> {
    // MSVC8 default copy constructor is broken
    using base = std::allocator<T>;
    using size_type = typename base::size_type;
    using difference_type = typename base::difference_type;
    using value_type = typename base::value_type;

    zero_after_free_allocator() noexcept {}

    zero_after_free_allocator(const zero_after_free_allocator &a) noexcept
        : base(a)
    {}

    template <typename U>
    zero_after_free_allocator(const zero_after_free_allocator<U> &a) noexcept
        : base(a) {}

    ~zero_after_free_allocator() noexcept {}

    template <typename _Other> struct rebind {
        typedef zero_after_free_allocator<_Other> other;
    };

    void deallocate(T *p, std::size_t n) {
        if (p != nullptr) memory_cleanse(p, sizeof(T) * n);
        std::allocator<T>::deallocate(p, n);
    }
};

// Byte-vector that clears its contents before deletion.
using CSerializeData = std::vector<char, zero_after_free_allocator<char>>;
