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
tree_t* tree_diff   (const tree_t* const tree, tree_t* const pt,    const char diff_var, FILE* out);
tree_t* tree_ndiff  (const tree_t* const tree, const size_t n_diff, const char diff_var, FILE* out);

tree_t* tree_val_in_point (const tree_t* const tree, const char diff_var, const operand_t point, 
                           FILE* out);
tree_t* tree_taylor_monom(const tree_t* const tree, const size_t order, const char diff_var, 
                          const operand_t point, FILE* out);
tree_t* tree_taylor_polynom(const tree_t* const tree, const size_t order, const char diff_var, 
                            const operand_t point, FILE* out);

enum TreeError tree_to_str   (const tree_t* const tree, char* const str, const size_t str_size);
enum TreeError tree_print_tex(FILE* out, const tree_t* const tree);
enum TreeError tree_read     (const char* const in_name, tree_t** tree);
enum TreeError tree_create_graphic(const tree_t* const tree, const tree_t* const subtree,
                                   const char var, const char* const filename, char* const name);

enum TreeError tree_simplify(tree_t** tree, FILE* out);

bool tree_include_var(const tree_t* const tree);
void tree_update_size(tree_t* const tree);

#endif /* DIFFER_SRC_TREE_TREE_FUNCS_H */