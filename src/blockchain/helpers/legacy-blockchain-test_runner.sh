#!/bin/sh

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS=\
"--run_test=* "\
"--show_progress=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes"


# Run tests.
#==============================================================================
./test/kth-blockchain-test ${BOOST_UNIT_TEST_OPTIONS} > test.log
