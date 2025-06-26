// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_DELEGATES_HPP
#define KTH_INFRASTRUCTURE_DELEGATES_HPP

#include <functional>

#if ! defined(__EMSCRIPTEN__)
#include <kth/infrastructure/utility/work.hpp>
#endif

namespace kth {
namespace delegates {

#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))

/// Binding delegate (current thread).
template <typename Handler>
struct bound
{
    template <typename... Args>
    void operator()(Args&&... args) {
        work::bound(BIND_HANDLER(handler, args));
    }

    Handler handler;
};

/// Asynchronous delegate.
template <typename Handler>
struct concurrent
{
    template <typename... Args>
    void operator()(Args&&... args) {
        heap->concurrent(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work::ptr heap;
};

/// Ordered synchronous delegate.
template <typename Handler>
struct ordered
{
    template <typename... Args>
    void operator()(Args&&... args) {
        heap->ordered(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work::ptr heap;
};

/// Unordered synchronous delegate.
template <typename Handler>
struct unordered
{
    template <typename... Args>
    void operator()(Args&&... args) {
        heap->unordered(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work::ptr heap;
};

/// Sequence ordering delegate.
template <typename Handler>
struct sequence
{
    template <typename... Args>
    void operator()(Args&&... args) {
        heap->lock(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work::ptr heap;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER

} // namespace delegates
} // namespace kth

#endif
