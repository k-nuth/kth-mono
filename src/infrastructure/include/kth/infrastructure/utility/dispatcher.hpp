// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_DISPATCHER_HPP
#define KTH_INFRASTRUCTURE_DISPATCHER_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <kth/infrastructure/utility/noncopyable.hpp>
#include <kth/infrastructure/utility/threadpool.hpp>

#if ! defined(__EMSCRIPTEN__)

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/error.hpp>
#include <kth/infrastructure/utility/asio.hpp>
#include <kth/infrastructure/utility/deadline.hpp>
#include <kth/infrastructure/utility/delegates.hpp>
#include <kth/infrastructure/utility/synchronizer.hpp>
#include <kth/infrastructure/utility/work.hpp>

#endif // ! defined(__EMSCRIPTEN__)


namespace kth {

#if ! defined(__EMSCRIPTEN__)

#define FORWARD_ARGS(args) std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))
#define BIND_ARGS(args) std::bind(FORWARD_ARGS(args))

// Collection dispatch doesn't forward args as move args can only forward once.
#define BIND_RACE(args, call) std::bind(args..., call)
#define BIND_ELEMENT(args, element, call) std::bind(args..., element, call)

/// This  class is thread safe.
/// If the ios service is stopped jobs will not be dispatched.
class KI_API dispatcher : noncopyable {
public:
    using delay_handler = std::function<void (code const &)>;

    dispatcher(threadpool& pool, std::string const& name);

    ////size_t ordered_backlog();
    ////size_t unordered_backlog();
    ////size_t concurrent_backlog();
    ////size_t combined_backlog();

    /// Invokes a job on the current thread. Equivalent to invoking std::bind.
    template <typename... Args>
    static
    void bound(Args&&... args) {
        BIND_ARGS(args)();
    }

    /// Posts a job to the service. Concurrent and not ordered.
    template <typename... Args>
    void concurrent(Args&&... args) {
        heap_->concurrent(BIND_ARGS(args));
    }

    /// Post a job to the strand. Ordered and not concurrent.
    template <typename... Args>
    void ordered(Args&&... args) {
        heap_->ordered(BIND_ARGS(args));
    }

    /// Posts a strand-wrapped job to the service. Not ordered or concurrent.
    /// The wrap provides non-concurrency, order is prevented by service post.
    template <typename... Args>
    void unordered(Args&&... args) {
        heap_->unordered(BIND_ARGS(args));
    }

    /// Posts an asynchronous job to the sequencer. Ordered and not concurrent.
    /// The sequencer provides both non-concurrency and ordered execution.
    template <typename... Args>
    void lock(Args&&... args) {
        heap_->lock(BIND_ARGS(args));
    }

    /// Complete sequential execution.
    inline
    void unlock() {
        heap_->unlock();
    }

    /// Posts job to service after specified delay. Concurrent and not ordered.
    /// The timer cannot be canceled so delay should be within stop criteria.
    inline
    void delayed(asio::duration const& delay, delay_handler const& handler) {
        auto timer = std::make_shared<deadline>(pool_, delay);
        timer->start([handler, timer](code const& ec) {
            handler(ec);
            timer->stop();
        });
    }

    /// Returns a delegate that will execute the job on the current thread.
    template <typename... Args>
    static
    auto bound_delegate(Args&&... args) -> delegates::bound<decltype(BIND_ARGS(args))> {
        return {
            BIND_ARGS(args)
        };
    }

    /// Returns a delegate that will post the job via the service.
    template <typename... Args>
    auto concurrent_delegate(Args&&... args) -> delegates::concurrent<decltype(BIND_ARGS(args))> {
        return {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Returns a delegate that will post the job via the strand.
    template <typename... Args>
    auto ordered_delegate(Args&&... args) -> delegates::ordered<decltype(BIND_ARGS(args))> {
        return {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Returns a delegate that will post a wrapped job via the service.
    template <typename... Args>
    auto unordered_delegate(Args&&... args) -> delegates::unordered<decltype(BIND_ARGS(args))> {
        return {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Returns a delegate that will post a job via the sequencer.
    template <typename... Args>
    auto sequence_delegate(Args&&... args) -> delegates::sequence<decltype(BIND_ARGS(args))> {
        return {
            BIND_ARGS(args),
            heap_
        };
    }

    /////// Executes multiple identical jobs concurrently until one completes.
    ////template <typename Count, typename Handler, typename... Args>
    ////void race(Count count, std::string const& name, Handler&& handler,
    ////    Args... args)
    ////{
    ////    // The first fail will also terminate race and return the code.
    ////    static size_t const clearance_count = 1;
    ////    auto const call = synchronize(FORWARD_HANDLER(handler),
    ////        clearance_count, name, false);
    ////
    ////    for (Count iteration = 0; iteration < count; ++iteration)
    ////        concurrent(BIND_RACE(args, call));
    ////}
    ////
    /////// Executes the job against each member of a collection concurrently.
    ////template <typename Element, typename Handler, typename... Args>
    ////void parallel(const std::vector<Element>& collection,
    ////    std::string const& name, Handler&& handler, Args... args)
    ////{
    ////    // Failures are suppressed, success always returned to handler.
    ////    auto const call = synchronize(FORWARD_HANDLER(handler),
    ////        collection.size(), name, true);
    ////
    ////    for (auto const& element: collection)
    ////        concurrent(BIND_ELEMENT(args, element, call));
    ////}
    ////
    /////// Disperses the job against each member of a collection without order.
    ////template <typename Element, typename Handler, typename... Args>
    ////void disperse(const std::vector<Element>& collection,
    ////    std::string const& name, Handler&& handler, Args... args)
    ////{
    ////    // Failures are suppressed, success always returned to handler.
    ////    auto const call = synchronize(FORWARD_HANDLER(handler),
    ////        collection.size(), name, true);
    ////
    ////    for (auto const& element: collection)
    ////        unordered(BIND_ELEMENT(args, element, call));
    ////}
    ////
    /////// Disperses the job against each member of a collection with order.
    ////template <typename Element, typename Handler, typename... Args>
    ////void serialize(const std::vector<Element>& collection,
    ////    std::string const& name, Handler&& handler, Args... args)
    ////{
    ////    // Failures are suppressed, success always returned to handler.
    ////    auto const call = synchronize(FORWARD_HANDLER(handler),
    ////        collection.size(), name, true);
    ////
    ////    for (auto const& element: collection)
    ////        ordered(BIND_ELEMENT(args, element, call));
    ////}
    ////
    /////// Sequences the job against each member of a collection with order.
    ////template <typename Element, typename Handler, typename... Args>
    ////void sequential(const std::vector<Element>& collection,
    ////    std::string const& name, Handler&& handler, Args... args)
    ////{
    ////    // Failures are suppressed, success always returned to handler.
    ////    auto const call = synchronize(FORWARD_HANDLER(handler),
    ////        collection.size(), name, true);
    ////
    ////    for (auto const& element: collection)
    ////        sequence(BIND_ELEMENT(args, element, call));
    ////}

    /// The size of the dispatcher's threadpool at the time of calling.
    inline
    size_t size() const {
        return pool_.size();
    }

private:
    // This is thread safe.
    work::ptr heap_;
    threadpool& pool_;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER
#undef BIND_ARGS
#undef BIND_RACE
#undef BIND_ELEMENT

#else

class KI_API dispatcher : noncopyable {
public:
    dispatcher(threadpool& pool, std::string const& name);

    template <typename... Args>
    void concurrent(Args&&... args) {
        std::invoke(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void ordered(Args&&... args) {
        std::invoke(std::forward<Args>(args)...);
    }

    inline
    size_t size() const {
        return 1;
    }
};

#endif // ! defined(__EMSCRIPTEN__)

} // namespace kth

#endif
