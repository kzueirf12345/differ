#ifndef DIFFER_SRC_TREE_TREE_FUNCS_H
#define DIFFER_SRC_TREE_TREE_FUNCS_H

#include "tree_structs.h"
#include "verification/verification.h"

const char* node_type_to_str(const enum NodeType type);
const char*   op_type_to_str(const enum OpType   type);

tree_t* tree_ctor(int data, enum NodeType type, 
                  tree_t* const pt, tree_t* const lt, tree_t* const rt);
void    tree_dtor(tree_t* const tree);

#endif /* DIFFER_SRC_TREE_TREE_FUNCS_H */