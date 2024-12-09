#include <stdio.h>

#include "tree/operation/operation.h"
#include "tree/funcs/funcs.h"
#include "tree/operation/op_math.h"
#include "tree/operation/op_diff.h"

#define OPERATION_HANDLE(_type, _name, _tex_name, _graph_name, _count_operands, _notation, _num)    \
        (operation_t){.type = OP_TYPE_##_type, .name = _name, .tex_name = _tex_name,                \
                      .graph_name = _graph_name, .count_operands = _count_operands,                 \
                      .notation = _notation},


const operation_t OPERATIONS[] =
{
    #include "tree/operation/codegen.h"
};
const size_t OPERATIONS_SIZE = sizeof(OPERATIONS) / sizeof(*OPERATIONS);
