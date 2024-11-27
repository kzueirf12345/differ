#ifndef DIFFER_SRC_TREE_DIFF_FUNCS_H
#define DIFFER_SRC_TREE_DIFF_FUNCS_H

#include <stdio.h>

#include "tree/structs.h"

tree_t* diff_SUM        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_SUB        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_MUL        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_DIV        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_POW        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_SIN        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_COS        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_TG         (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_CTG        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_ARCSIN     (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_ARCCOS     (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_ARCTG      (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_ARCCTG     (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_SH         (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_CH         (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_TH         (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_CTH        (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_LOG        (const tree_t* const tree, tree_t* const pt, FILE* out);
#endif /* DIFFER_SRC_TREE_DIFF_FUNCS_H */