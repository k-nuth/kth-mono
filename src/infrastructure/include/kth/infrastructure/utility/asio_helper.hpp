// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_ASIO_HELPER_HPP_
#define KTH_INFRASTRUCTURE_ASIO_HELPER_HPP_

#if defined(ASIO_STANDALONE)
#include <asio.hpp>
#else

#if ! defined(__EMSCRIPTEN__)
#include <boost/asio.hpp>
#else
#include <boost/asio/error.hpp>
#endif

namespace asio = boost::asio;
#endif

#endif // KTH_INFRASTRUCTURE_ASIO_HELPER_HPP_
