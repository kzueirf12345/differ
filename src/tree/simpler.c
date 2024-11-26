#include <stdio.h>

#include "logger/liblogger.h"
#include "tree/funcs.h"
#include "utils/utils.h"

enum TreeError tree_reduce_const_(tree_t* const tree, FILE* out);

enum TreeError tree_simplify(tree_t* const tree, FILE* out)
{
    if (tree == NULL) 
        return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");


}

enum TreeError tree_reduce_const_(tree_t* const tree, FILE* out)
{
    if (tree == NULL) 
        return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    if (tree->type != NODE_TYPE_OP) 
        return TREE_ERROR_SUCCESS;

    if (tree->lt->type == NODE_TYPE_NUM && tree->rt->type == NODE_TYPE_NUM)
    {
        
    }
}