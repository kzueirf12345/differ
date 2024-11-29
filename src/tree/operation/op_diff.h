#ifndef DIFFER_SRC_TREE_OPERATION_DIFF_FOO_H
#define DIFFER_SRC_TREE_OPERATION_DIFF_FOO_H

#include <stdio.h>

#include "tree/structs.h"

#define OPERATION_HANDLE(_type, _name, _tex_name, _count_operands, _notation, _num)                       \
        tree_t* diff_##_type (const tree_t* const tree, tree_t* const pt, FILE* out);

#include "tree/operation/codegen.h"

#undef OPERATION_HANDLE

#endif /* DIFFER_SRC_TREE_OPERATION_DIFF_FOO_H */