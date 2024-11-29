#include <stdio.h>
#include <math.h>

#include "tree/funcs/funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree/operation/operation.h"
#include "tree/operation/op_math.h"

enum TreeError tree_simplify_constants_(tree_t** tree, FILE* out, size_t* const count_changes);
enum TreeError tree_simplify_trivial_  (tree_t** tree, FILE* out, size_t* const count_changes);

enum TreeError tree_simplify(tree_t** tree, FILE* out)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");

    size_t count_changes = 0;
    do
    {
        count_changes = 0;
        TREE_ERROR_HANDLE(tree_simplify_constants_(tree, out, &count_changes));
        TREE_ERROR_HANDLE(tree_simplify_trivial_  (tree, out, &count_changes));
    } while (count_changes);
    
    return TREE_ERROR_SUCCESS;
}

#define OPERATION_HANDLE(name, ...)                                                                 \
    case OP_TYPE_##name:                                                                            \
        *tree = tree_ctor((tree_data_u){.num = math_##name((*tree)->lt->data.num,                   \
                                                           (*tree)->rt->data.num,                   \
                                                           out)},                                   \
                         NODE_TYPE_NUM, (*tree)->pt, NULL, NULL);                                   \
        break;

enum TreeError tree_simplify_constants_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");

    if (!*tree || (*tree)->type != NODE_TYPE_OP)
        return TREE_ERROR_SUCCESS;
        
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    
    TREE_ERROR_HANDLE(tree_simplify_constants_(&(*tree)->lt, out, count_changes));
    TREE_ERROR_HANDLE(tree_simplify_constants_(&(*tree)->rt, out, count_changes));

    tree_update_size((*tree));

    if (((*tree)->lt && ((*tree)->lt->type != NODE_TYPE_NUM))
     || ((*tree)->rt && ((*tree)->rt->type != NODE_TYPE_NUM)))
        return TREE_ERROR_SUCCESS;
    

    tree_t* temp = *tree;

    switch((*tree)->data.op)
    {
        #include "tree/operation/codegen.h"

        case MAX_OP_TYPE:
        default:
            fprintf(stderr, "Unknown data type\n");
            return TREE_ERROR_OP_TYPE_INVALID;
    }

    if (isnan((*tree)->data.num))
    {
        fprintf(stderr, "Incorrect operation\n");
        return TREE_ERROR_NUM_DATA_INVALID;
    }

    temp->pt = temp;
    tree_dtor(temp); temp = NULL;

    ++*count_changes;

    return TREE_ERROR_SUCCESS;
}
#undef OPERATION_HANDLE

enum TreeError tree_simplify_trivial_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");

    if (!*tree || (*tree)->type != NODE_TYPE_OP)
        return TREE_ERROR_SUCCESS;
        
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    
    TREE_ERROR_HANDLE(tree_simplify_trivial_(&(*tree)->lt, out, count_changes));
    TREE_ERROR_HANDLE(tree_simplify_trivial_(&(*tree)->rt, out, count_changes));

    tree_update_size((*tree));

    tree_t* temp = *tree;

    if ((*tree)->data.op == OP_TYPE_POW && (*tree)->rt->type == NODE_TYPE_NUM)
    {
        if (fabs((*tree)->rt->data.num - 1) < __DBL_EPSILON__)
        {
            *tree = (*tree)->lt;
            (*tree)->pt = temp->pt;

            temp->pt = temp;
            temp->lt = NULL;
            tree_update_size(temp);
            tree_dtor(temp); temp = NULL;
        }
        else if (fabs((*tree)->rt->data.num) < __DBL_EPSILON__ )
        {
            *tree = tree_ctor((tree_data_u){.num = 1}, NODE_TYPE_NUM, (*tree)->pt, NULL, NULL);

            temp->pt = temp;
            tree_dtor(temp); temp = NULL;
        }
    }

    return TREE_ERROR_SUCCESS;
}