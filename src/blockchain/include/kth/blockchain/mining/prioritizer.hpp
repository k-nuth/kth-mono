// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_BLOCKCHAIN_MINING_PRIORITIZER_HPP_
#define KTH_BLOCKCHAIN_MINING_PRIORITIZER_HPP_


// #include <thread>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

// #include <boost/thread/condition_variable.hpp>
// #include <boost/thread/locks.hpp>
// #include <boost/thread/shared_mutex.hpp>

namespace kth {
namespace mining {

// template <typename Mutex>
class prioritizer {
public:
    using unique_lock_t = std::unique_lock<std::mutex>;
    using lock_guard_t = std::lock_guard<std::mutex>;

    prioritizer() = default;

#ifndef NDEBUG
    ~prioritizer() {
        assert( ! waiting_);
    }
#endif

    prioritizer(prioritizer const&) = delete;
    prioritizer operator=(prioritizer const&) = delete;

    template <typename F>
    auto low_job(F f) const {
        unique_lock_t lk(gate_);    //Locked
        cv_.wait(lk, [&]{ return ! waiting_; });

        auto res = f();

        // cv_.notify_all();
        cv_.notify_one();

        return res;
    }

    template <typename F>
    auto high_job(F f) const {
        waiting_ = true;
        lock_guard_t lk(gate_);

        waiting_ = false;

        auto res = f();

        // cv_.notify_all();
        cv_.notify_one();

        return res;
    }

private:
    mutable std::condition_variable cv_;
    mutable std::mutex gate_;
            //shared_mutex
    mutable std::atomic<bool> waiting_ {false};
};

}  // namespace mining
}  // namespace kth

#endif  //KTH_BLOCKCHAIN_MINING_PRIORITIZER_HPP_
