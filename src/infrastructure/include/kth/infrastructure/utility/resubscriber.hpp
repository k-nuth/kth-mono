// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_RESUBSCRIBER_HPP
#define KTH_INFRASTRUCTURE_RESUBSCRIBER_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <kth/infrastructure/utility/dispatcher.hpp>
#include <kth/infrastructure/utility/enable_shared_from_base.hpp>
#include <kth/infrastructure/utility/thread.hpp>
#include <kth/infrastructure/utility/threadpool.hpp>
////#include <kth/infrastructure/utility/track.hpp>

namespace kth {

template <typename... Args>
class resubscriber
    : public enable_shared_from_base<resubscriber<Args...>>
        /*, track<resubscriber<Args...>>*/
{
public:
    using handler = std::function<bool (Args...)>;
    using ptr = std::shared_ptr<resubscriber<Args...>>;

    /// Construct an instance. The class_name is for debugging.
    resubscriber(threadpool& pool, std::string const& class_name);
    ~resubscriber();

    /// Enable new subscriptions.
    void start();

    /// Prevent new subscriptions.
    void stop();

    /// Subscribe to notifications with an option to resubscribe.
    /// Return true from the handler to resubscribe to notifications.
    void subscribe(handler&& notify, Args... stopped_args);

    /// Invoke all handlers sequentially (blocking).
    void invoke(Args... args);

    /// Invoke all handlers sequentially (non-blocking).
    void relay(Args... args);

private:
    using list = std::vector<handler>;

    void do_invoke(Args... args);

    bool stopped_;
    list subscriptions_;
    dispatcher dispatch_;
#if ! defined(__EMSCRIPTEN__)
    mutable upgrade_mutex invoke_mutex_;
    mutable upgrade_mutex subscribe_mutex_;
#else
    mutable shared_mutex invoke_mutex_;
    mutable shared_mutex subscribe_mutex_;
#endif
};

} // namespace kth

#include <kth/infrastructure/impl/utility/resubscriber.ipp>

#endif
