#ifndef DIFFER_SRC_TREE_TREE_FUNCS_H
#define DIFFER_SRC_TREE_TREE_FUNCS_H

#include "tree/structs.h"
#include "verification/verification.h"

const char* node_type_to_str(const enum NodeType type);
const char*   op_type_to_str(const enum OpType   type);
enum OpType   str_to_op_type(const char* const str);

tree_t* tree_ctor   (int data, enum NodeType type, 
                     tree_t* const pt, tree_t* const lt, tree_t* const rt);
void    tree_fill_pt(tree_t* const tree, tree_t* const pt);
void    tree_dtor   (tree_t* const tree);
tree_t* tree_copy   (const tree_t* const tree, tree_t* const pt);
tree_t* tree_diff   (const tree_t* const tree, tree_t* const pt, FILE* out);

enum TreeError tree_print_preorder(FILE* out, const tree_t* const tree);
enum TreeError tree_print_inorder (FILE* out, const tree_t* const tree);
enum TreeError tree_print_tex     (FILE* out, const tree_t* const tree);
enum TreeError tree_read_preorder (const char* const in_name, tree_t** tree);
enum TreeError tree_read_inorder  (const char* const in_name, tree_t** tree);

bool tree_include_var(const tree_t* const tree);
void tree_update_size(tree_t* const tree);

#endif /* DIFFER_SRC_TREE_TREE_FUNCS_H */