#ifndef DIFFER_SRC_TREE_OPERATION_OPERATION_H
#define DIFFER_SRC_TREE_OPERATION_OPERATION_H

#include <stdio.h>

#include "tree/structs.h"

// typedef operand_t (*op_foo_t) (const operand_t first,    const operand_t second, FILE* out);
// typedef tree_t*   (*op_diff_t)(const tree_t* const tree, tree_t* const pt,       FILE* out);

enum OpNotation
{
    OP_NOTATION_LEFT     = 0,
    OP_NOTATION_MIDDLE   = 1,
    OP_NOTATION_RIGHT    = 2
};

typedef struct Operation
{
    enum OpType     type;

    const char*     name;
    const char*     tex_name;

    // op_foo_t        foo;
    // op_diff_t       diff;

    size_t          count_operands;
    enum OpNotation notation;
} operation_t;

extern const operation_t OPERATIONS[];
extern const size_t      OPERATIONS_SIZE;


#endif /* DIFFER_SRC_TREE_OPERATION_OPERATION_H */