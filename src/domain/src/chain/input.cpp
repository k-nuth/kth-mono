// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/chain/input.hpp>

#include <algorithm>
#include <sstream>

#include <kth/domain/chain/script.hpp>

#if defined(KTH_SEGWIT_ENABLED)
#include <kth/domain/chain/witness.hpp>
#endif

#include <kth/domain/constants.hpp>
#include <kth/domain/wallet/payment_address.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>

namespace kth::domain::chain {

using namespace kth::domain::wallet;
using namespace kth::domain::machine;

// Constructors.
//-----------------------------------------------------------------------------

// Private cache access for copy/move construction.
input::addresses_ptr input::addresses_cache() const {
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(mutex_);

    return addresses_;
    ///////////////////////////////////////////////////////////////////////////
}

input::input(input const& x)
    : input_basis(x)
    , addresses_(x.addresses_cache())
{}

input::input(input&& x) noexcept
    : input_basis(std::move(x))
    , addresses_(x.addresses_cache())
{}

// Operators.
//-----------------------------------------------------------------------------

input& input::operator=(input const& x) {
    input_basis::operator=(x);
    addresses_ = x.addresses_cache();
    return *this;
}

input& input::operator=(input&& x) noexcept {
    input_basis::operator=(std::move(static_cast<input_basis&&>(x)));
    addresses_ = x.addresses_cache();
    return *this;
}


// Accessors.
//-----------------------------------------------------------------------------

void input::set_script(chain::script const& value) {
    input_basis::set_script(value);
    invalidate_cache();
}

void input::set_script(chain::script&& value) {
    input_basis::set_script(std::move(value));
    invalidate_cache();
}

#if defined(KTH_SEGWIT_ENABLED)
void input::set_witness(chain::witness const& value) {
    input_basis::set_witness(value);
    invalidate_cache();
}

void input::set_witness(chain::witness&& value) {
    input_basis::set_witness(std::move(value));
    invalidate_cache();
}
#endif // KTH_CURRENCY_BCH

// protected
void input::invalidate_cache() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (addresses_) {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        addresses_.reset();
        //---------------------------------------------------------------------
        mutex_.unlock_and_lock_upgrade();
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

#else
    {
        std::shared_lock lock(mutex_);
        if ( ! addresses_) {
            return;
        }
    }

    std::unique_lock lock(mutex_);
    addresses_.reset();
#endif
}

payment_address input::address() const {
    auto const value = addresses();
    return value.empty() ? payment_address{} : value.front();
}

payment_address::list input::addresses() const {
#if ! defined(__EMSCRIPTEN__)
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if ( ! addresses_) {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();

        // TODO(legacy): expand to include segregated witness address extraction.
        addresses_ = std::make_shared<payment_address::list>(payment_address::extract_input(script()));
        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    auto addresses = *addresses_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
#else
    {
        std::shared_lock lock(mutex_);

        if (addresses_) {
            return *addresses_;
        }
    }

    std::unique_lock lock(mutex_);
    if ( ! addresses_) {
        addresses_ = std::make_shared<payment_address::list>(payment_address::extract_input(script()));
    }

    auto addresses = *addresses_;
#endif

    return addresses;
}

} // namespace kth::domain::chain
