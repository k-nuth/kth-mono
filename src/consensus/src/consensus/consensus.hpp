// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_CONSENSUS_CONSENSUS_HPP
#define KTH_CONSENSUS_CONSENSUS_HPP

#include <cstddef>
#include <kth/consensus/define.hpp>
#include <kth/consensus/export.hpp>
#include "script/script_error.h"

namespace kth::consensus {

// These are not published in the public header but are exposed here for test.
#if defined(KTH_CURRENCY_BCH)
BCK_API verify_result_type script_error_to_verify_result(ScriptError code);
#else
BCK_API verify_result_type script_error_to_verify_result(ScriptError_t code);
#endif

BCK_API unsigned int verify_flags_to_script_flags(unsigned int flags);

char const* version(); //TODO(knuth): use BCK_API and simililars in the rest of the projects

} // namespace kth::consensus

#endif
