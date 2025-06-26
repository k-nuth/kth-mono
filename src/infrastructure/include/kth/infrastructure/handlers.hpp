// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_HANDLERS_HPP
#define KTH_INFRASTRUCTURE_HANDLERS_HPP

#include <functional>

#include <kth/infrastructure/error.hpp>

namespace kth {

using handle0 = std::function<void(code const&)>;

template <typename Type>
using handle1 = std::function<void(code const&, const Type&)>;

template <typename Type1, typename Type2>
using handle2 = std::function<void(code const&, const Type1&, const Type2&)>;

template <typename Type1, typename Type2, typename Type3>
using handle3 = std::function<void(code const&, const Type1&, const Type2&, const Type3&)>;

} // namespace kth

#endif
