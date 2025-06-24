// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_SUBSCRIBER_HPP
#define KTH_INFRASTRUCTURE_SUBSCRIBER_HPP

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
class subscriber
    : public enable_shared_from_base<subscriber<Args...>>
    /*, track<subscriber<Args...>>*/
{
public:
    using handler = std::function<void (Args...)>;
    using ptr = std::shared_ptr<subscriber<Args...>>;

    subscriber(threadpool& pool, std::string const& class_name);
    ~subscriber();

    /// Enable new subscriptions.
    void start();

    /// Prevent new subscriptions.
    void stop();

    /// Subscribe to notifications (for one invocation only).
    void subscribe(handler&& notify, Args... stopped_args);

    /// Invoke and clear all handlers sequentially (blocking).
    void invoke(Args... args);

    /// Invoke and clear all handlers sequentially (non-blocking).
    void relay(Args... args);

private:
    using list = std::vector<handler>;

    void do_invoke(Args... args);

    bool stopped_;
    list subscriptions_;
    dispatcher dispatch_;
    mutable upgrade_mutex invoke_mutex_;
    mutable upgrade_mutex subscribe_mutex_;
};

} // namespace kth

#include <kth/infrastructure/impl/utility/subscriber.ipp>

#endif
