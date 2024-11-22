#ifndef DIFFER_SRC_TREE_TREE_FUNCS_H
#define DIFFER_SRC_TREE_TREE_FUNCS_H

#include "tree/structs.h"
#include "verification/verification.h"

const char* node_type_to_str(const enum NodeType type);
const char*   op_type_to_str(const enum OpType   type);
enum OpType   str_to_op_type(const char* const str);

tree_t* tree_ctor(int data, enum NodeType type, 
                  tree_t* const pt, tree_t* const lt, tree_t* const rt);
void    tree_dtor(tree_t* const tree);

enum TreeError tree_print_preorder(FILE* out, const tree_t* const tree);
enum TreeError tree_print_inorder (FILE* out, const tree_t* const tree);
enum TreeError tree_read_preorder (const char* const in_name, tree_t** tree);
enum TreeError tree_read_inorder  (const char* const in_name, tree_t** tree);

#endif /* DIFFER_SRC_TREE_TREE_FUNCS_H */