// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/utility/work.hpp>

#include <memory>
#include <string>

#include <kth/infrastructure/utility/delegates.hpp>
#include <kth/infrastructure/utility/threadpool.hpp>

namespace kth {

work::work(threadpool& pool, std::string const& name)
  : name_(name),
    ////ordered_(std::make_shared<monitor::count>(0)),
    ////unordered_(std::make_shared<monitor::count>(0)),
    ////concurrent_(std::make_shared<monitor::count>(0)),
    ////sequential_(std::make_shared<monitor::count>(0)),
    service_(pool.service()),
    strand_(service_),
    sequence_(service_)
{
}

////size_t work::ordered_backlog()
////{
////    return ordered_->load();
////}
////
////size_t work::unordered_backlog()
////{
////    return unordered_->load();
////}
////
////size_t work::concurrent_backlog()
////{
////    return concurrent_->load();
////}
////
////size_t work::sequential_backlog()
////{
////    return sequential_->load();
////}
////
////size_t work::combined_backlog()
////{
////    return ordered_backlog() + unordered_backlog() + concurrent_backlog();
////}

} // namespace kth
