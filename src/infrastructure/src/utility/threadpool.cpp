// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/utility/threadpool.hpp>

// #include <iostream>
#include <thread>

#include <kth/infrastructure/utility/asio.hpp>
#include <kth/infrastructure/utility/assert.hpp>
#include <kth/infrastructure/utility/thread.hpp>

namespace kth {

threadpool::threadpool(std::string const& name, size_t number_threads, thread_priority priority)
    : name_(name)
    , size_(0)
{
    // std::cout << "threadpool::threadpool() - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
    spawn(number_threads, priority);
}

threadpool::~threadpool() {
    // std::cout << "threadpool::~threadpool() - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
    shutdown();
    join();
}

// Should not be called during spawn.
bool threadpool::empty() const {
    return size() != 0;
}

// Should not be called during spawn.
size_t threadpool::size() const {
    return size_.load();
}

// This is not thread safe.
void threadpool::spawn(size_t number_threads, thread_priority priority) {
    // This allows the pool to be restarted.
    service_.reset();
    // std::cout << "threadpool::spawn() - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;

    for (size_t i = 0; i < number_threads; ++i) {
        spawn_once(priority);
    }
}

void threadpool::spawn_once(thread_priority priority) {
    // std::cout << "threadpool::spawn_once() - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    work_mutex_.lock_upgrade();

    // Work prevents the service from running out of work and terminating.
    if ( ! work_) {
        work_mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        work_ = std::make_shared<asio::service::work>(service_);

        work_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    work_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(threads_mutex_);

    threads_.emplace_back([this, priority]() {
        set_priority(priority);
        // std::cout << "threadpool::spawn_once() *** BEFORE run() *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
        service_.run();
        // std::cout << "threadpool::spawn_once() *** AFTER  run() *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
    });

    ++size_;
    ///////////////////////////////////////////////////////////////////////////
}

void threadpool::abort() {
    // std::cout << "threadpool::abort() *** BEFORE stop *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
    service_.stop();
    // std::cout << "threadpool::abort() *** AFTER stop *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
}

void threadpool::shutdown() {
    abort();
    // std::cout << "threadpool::shutdown() *** BEFORE lock *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;

    // {
    // ///////////////////////////////////////////////////////////////////////////
    // // Critical Section
    // unique_lock lock(work_mutex_);

    // work_.reset();
    // ///////////////////////////////////////////////////////////////////////////
    // }
    // std::cout << "threadpool::shutdown() *** AFTER lock *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;
}

void threadpool::join() {
    // std::cout << "threadpool::join() *** BEFORE lock *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;

    {
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(threads_mutex_);

    DEBUG_ONLY(auto const this_id = boost::this_thread::get_id();)
    // auto const this_id = boost::this_thread::get_id();

    // for (auto& thread: threads_) {
    //     KTH_ASSERT(this_id != thread.get_id());
    //     KTH_ASSERT(thread.joinable());

    //     std::cout << "threadpool::join() - this_id != thread.get_id(): " << (this_id != thread.get_id()) << std::endl;
    //     std::cout << "threadpool::join() - thread.joinable(): " << thread.joinable() << " - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;

    //     std::cout << "threadpool::join() *** BEFORE join *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;
    //     thread.join();
    //     std::cout << "threadpool::join() *** AFTER  join *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;
    // }

    for (auto i = threads_.rbegin(); i != threads_.rend(); ++i ) {
        auto& thread = *i;

        KTH_ASSERT(this_id != thread.get_id());
        KTH_ASSERT(thread.joinable());

        // std::cout << "threadpool::join() - this_id != thread.get_id(): " << (this_id != thread.get_id()) << std::endl;
        // std::cout << "threadpool::join() - thread.joinable(): " << thread.joinable() << " - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;

        // std::cout << "threadpool::join() *** BEFORE join *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;
        // thread.join();
        // std::cout << "threadpool::join() *** AFTER  join *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;

        // std::cout << "threadpool::join() *** BEFORE detach *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;
        thread.detach();
        // std::cout << "threadpool::join() *** AFTER  detach *** - name: " << name_ << " - thread id: " << thread.get_id() << std::endl;
    }

    threads_.clear();
    size_.store(0);
    ///////////////////////////////////////////////////////////////////////////
    }

    // std::cout << "threadpool::join() *** AFTER lock *** - name: " << name_ << " - thread id: " << std::this_thread::get_id() << std::endl;

}

asio::service& threadpool::service() {
    return service_;
}

const asio::service& threadpool::service() const {
    return service_;
}

} // namespace kth
