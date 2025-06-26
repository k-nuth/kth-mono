// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_LOG_SINK_HPP
#define KTH_INFRASTRUCTURE_LOG_SINK_HPP

#if defined(KTH_LOG_LIBRARY_BOOST)
#include <iostream>

#include <boost/smart_ptr.hpp>

#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/log/rotable_file.hpp>
#include <kth/infrastructure/log/severity.hpp>
#include <kth/infrastructure/unicode/ofstream.hpp>
#elif defined(KTH_LOG_LIBRARY_SPDLOG)
#include <string>
#endif

namespace kth::log {

#if defined(KTH_LOG_LIBRARY_BOOST)
using file = boost::shared_ptr<kth::ofstream>;

/// Initializes null (as opposed to default) logging sinks.
void initialize();

/// Initializes default non-rotable logging sinks and formats.
void initialize(log::file& debug_file, log::file& error_file, log::stream& output_stream, log::stream& error_stream, bool verbose);
void initialize(log::file& debug_file, log::file& error_file, bool verbose);

/// Initializes default rotable logging sinks and formats.
void initialize(rotable_file const& debug_file, rotable_file const& error_file, log::stream& output_stream, log::stream& error_stream, bool verbose);
void initialize(rotable_file const& debug_file, rotable_file const& error_file, bool verbose);

/// Log stream operator.
formatter& operator<<(formatter& stream, severity value);

#elif defined(KTH_LOG_LIBRARY_SPDLOG)

void initialize(std::string const& debug_file, std::string const& error_file, bool stdout_enabled, bool verbose);

#endif

} // namespace kth::log

#endif
