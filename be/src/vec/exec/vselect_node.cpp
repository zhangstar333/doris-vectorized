// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "vec/exec/vselect_node.h"

namespace doris {
namespace vectorized {

VSelectNode::VSelectNode(ObjectPool* pool, const TPlanNode& tnode, const DescriptorTbl& descs)
        : ExecNode(pool, tnode, descs), _child_eos(false) {}

Status VSelectNode::init(const TPlanNode& tnode, RuntimeState* state) {
    return ExecNode::init(tnode, state);
}

Status VSelectNode::prepare(RuntimeState* state) {
    return ExecNode::prepare(state);
}

Status VSelectNode::open(RuntimeState* state) {
    RETURN_IF_ERROR(exec_debug_action(TExecNodePhase::OPEN));
    RETURN_IF_ERROR(ExecNode::open(state));
    RETURN_IF_ERROR(child(0)->open(state));
    return Status::OK();
}

Status VSelectNode::get_next(RuntimeState* state, RowBatch* row_batch, bool* eos) {
    return Status::NotSupported("Not Implemented VSelectNode::get_next.");
}

Status VSelectNode::get_next(RuntimeState* state, vectorized::Block* block, bool* eos) {
    RETURN_IF_ERROR(exec_debug_action(TExecNodePhase::GETNEXT));
    RETURN_IF_CANCELLED(state);
    while (true) {
        RETURN_IF_CANCELLED(state);
<<<<<<< HEAD
        RETURN_IF_ERROR(_children[0]->get_next(state, block, &_child_eos));
=======
        RETURN_IF_ERROR(_children[0]->get_next(state, &_child_block, &_child_eos));
>>>>>>> add select node and from_days
        if (reached_limit() || _child_eos) {
            *eos = true;
            break;
        }
<<<<<<< HEAD
        if (block->rows() == state->batch_size()) {
            break;
        }
    }
    RETURN_IF_ERROR(VExprContext::filter_block(_vconjunct_ctx_ptr, block, block->columns()));
=======
        if (_child_block.rows() == state->batch_size()) {
            break;
        }
    }
    RETURN_IF_ERROR(VExprContext::filter_block(_vconjunct_ctx_ptr, &_child_block, _child_block.columns()));
    block->swap(std::move(_child_block));
>>>>>>> add select node and from_days
    return Status::OK();
}

Status VSelectNode::close(RuntimeState* state) {
    if (is_closed()) {
        return Status::OK();
    }
    return ExecNode::close(state);
}

} // namespace vectorized
} // namespace doris