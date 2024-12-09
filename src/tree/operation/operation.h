#ifndef DIFFER_SRC_TREE_OPERATION_OPERATION_H
#define DIFFER_SRC_TREE_OPERATION_OPERATION_H

#include <stdio.h>

#include "tree/structs.h"

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
    const char*     graph_name;

    size_t          count_operands;
    enum OpNotation notation;
} operation_t;

extern const operation_t OPERATIONS[];
extern const size_t      OPERATIONS_SIZE;


#endif /* DIFFER_SRC_TREE_OPERATION_OPERATION_H */