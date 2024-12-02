#ifndef DIFFER_SRC_TREE_TREE_FUNCS_H
#define DIFFER_SRC_TREE_TREE_FUNCS_H

#include "tree/structs.h"
#include "tree/verification/verification.h"

const char* node_type_to_str(const enum NodeType type);
enum OpType   str_to_op_type(const char* const str);

tree_t* tree_ctor   (tree_data_u data, enum NodeType type, 
                     tree_t* const pt, tree_t* const lt, tree_t* const rt);
void    tree_fill_pt(tree_t* const tree, tree_t* const pt);
void    tree_dtor   (tree_t* const tree);

tree_t* tree_copy   (const tree_t* const tree, tree_t* const pt);
tree_t* tree_diff   (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);

enum TreeError tree_print_tex(FILE* out, const tree_t* const tree);
enum TreeError tree_read     (const char* const in_name, tree_t** tree);

enum TreeError tree_simplify(tree_t** tree, FILE* out);

bool tree_include_var(const tree_t* const tree);
void tree_update_size(tree_t* const tree);

#endif /* DIFFER_SRC_TREE_TREE_FUNCS_H */