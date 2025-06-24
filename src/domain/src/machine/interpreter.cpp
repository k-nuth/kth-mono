// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/domain/machine/interpreter.hpp>

// #include <kth/domain/constants.hpp>
// #include <kth/domain/machine/operation.hpp>
// #include <kth/domain/machine/program.hpp>
// #include <kth/infrastructure/error.hpp>

namespace kth::domain::machine {

code interpreter::run(program& program) {
    code ec;

    if ( ! program.is_valid()) {
        return error::invalid_script;
    }

    for (auto const& op : program) {
        if (op.is_oversized()) {
            return error::invalid_push_data_size;
        }

        if (op.is_disabled()) {
            return error::op_disabled;
        }

        if ( ! program.increment_operation_count(op)) {
            return error::invalid_operation_count;
        }

        if (program.if_(op)) {
            if ((ec = run_op(op, program))) {
                return ec;
            }

            if (program.is_stack_overflow()) {
                return error::invalid_stack_size;
            }
        }
    }

    return program.closed() ? error::success : error::invalid_stack_scope;
}

code interpreter::run(operation const& op, program& program) {
    return run_op(op, program);
}

} // namespace kth::domain::machine
