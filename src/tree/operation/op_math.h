#ifndef DIFFER_SRC_TREE_OPERATION_OP_math_H
#define DIFFER_SRC_TREE_OPERATION_OP_math_H

#include <stdio.h>

#include "tree/operation/operation.h"

#define OPERATION_HANDLE(_type, _name, _tex_name, temp, _count_operands, _notation, _num)                 \
        operand_t math_##_type (const operand_t first, const operand_t second, FILE* out);

#include "tree/operation/codegen.h"

#undef OPERATION_HANDLE

#endif /* DIFFER_SRC_TREE_OPERATION_OP_math_H */