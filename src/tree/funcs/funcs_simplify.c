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
                                                           (*tree)->rt ? (*tree)->rt->data.num : 0, \
                                                           out)},                                   \
                         NODE_TYPE_NUM, ((*tree)->pt == *tree ? NULL : (*tree)->pt), NULL, NULL);   \
        break;

enum TreeError tree_simplify_constants_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(!is_invalid_ptr(count_changes), "");

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

enum TreeError tree_simplify_POW_(tree_t** tree, FILE* out, size_t* const count_changes);
enum TreeError tree_simplify_MUL_(tree_t** tree, FILE* out, size_t* const count_changes);
enum TreeError tree_simplify_SUM_(tree_t** tree, FILE* out, size_t* const count_changes);
enum TreeError tree_simplify_SUB_(tree_t** tree, FILE* out, size_t* const count_changes);

enum TreeError tree_simplify_trivial_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(!is_invalid_ptr(count_changes), "");

    if (!*tree || (*tree)->type != NODE_TYPE_OP)
        return TREE_ERROR_SUCCESS;
        
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    
    TREE_ERROR_HANDLE(tree_simplify_trivial_(&(*tree)->lt, out, count_changes));
    TREE_ERROR_HANDLE(tree_simplify_trivial_(&(*tree)->rt, out, count_changes));

    tree_update_size((*tree));


    if ((*tree)->data.op == OP_TYPE_POW)
    {
        TREE_ERROR_HANDLE(tree_simplify_POW_(tree, out, count_changes));
    }
    else if ((*tree)->data.op == OP_TYPE_MUL)
    {
        TREE_ERROR_HANDLE(tree_simplify_MUL_(tree, out, count_changes));
    }
    else if ((*tree)->data.op == OP_TYPE_SUM)
    {
        TREE_ERROR_HANDLE(tree_simplify_SUM_(tree, out, count_changes));
    }
    else if ((*tree)->data.op == OP_TYPE_SUB)
    {
        TREE_ERROR_HANDLE(tree_simplify_SUB_(tree, out, count_changes));
    }

    return TREE_ERROR_SUCCESS;
}

#define _IS_ZERO_RT                                                                                 \
        ((*tree)->rt->type == NODE_TYPE_NUM && (fabs((*tree)->rt->data.num)     < __DBL_EPSILON__))
#define _IS_ZERO_LT                                                                                 \
        ((*tree)->lt->type == NODE_TYPE_NUM && (fabs((*tree)->lt->data.num)     < __DBL_EPSILON__))
#define _IS_ONE_RT                                                                                  \
        ((*tree)->rt->type == NODE_TYPE_NUM && (fabs((*tree)->rt->data.num - 1) < __DBL_EPSILON__))
#define _IS_ONE_LT                                                                                  \
        ((*tree)->lt->type == NODE_TYPE_NUM && (fabs((*tree)->lt->data.num - 1) < __DBL_EPSILON__))

void switch_tree_to_lt_ (tree_t** tree, FILE* out);
void switch_tree_to_rt_ (tree_t** tree, FILE* out);
void switch_tree_to_num_(tree_t** tree, FILE* out, const double num);

#define _TREE_TO_LT                                                                                 \
        switch_tree_to_lt_(tree, out);                                                              \
        ++*count_changes

#define _TREE_TO_RT                                                                                 \
        switch_tree_to_rt_(tree, out);                                                              \
        ++*count_changes

#define _TREE_TO_ZERO                                                                               \
        switch_tree_to_num_(tree, out, 0);                                                          \
        ++*count_changes

#define _TREE_TO_ONE                                                                                \
        switch_tree_to_num_(tree, out, 1);                                                          \
        ++*count_changes

enum TreeError tree_simplify_POW_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    lassert(!is_invalid_ptr(count_changes), "");

    if (_IS_ONE_RT)
    {
        _TREE_TO_LT;
    }
    else if (_IS_ZERO_RT)
    {
        _TREE_TO_ONE;
    }
    
    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_simplify_MUL_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    lassert(!is_invalid_ptr(count_changes), "");

    if (_IS_ZERO_LT || _IS_ZERO_RT)
    {
        _TREE_TO_ZERO;
    }
    else if (_IS_ONE_RT)
    {
        _TREE_TO_LT;
    }
    else if (_IS_ONE_LT)
    {
        _TREE_TO_RT;
    }

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_simplify_SUM_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    lassert(!is_invalid_ptr(count_changes), "");

    if (_IS_ZERO_RT)
    {
        _TREE_TO_LT;
    }
    else if (_IS_ZERO_LT)
    {
        _TREE_TO_RT;
    }

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_simplify_SUB_(tree_t** tree, FILE* out, size_t* const count_changes)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");
    lassert(!is_invalid_ptr(count_changes), "");

    if (_IS_ZERO_RT)
    {
        _TREE_TO_LT;
    }
    else if (_IS_ZERO_LT)
    {
        (*tree)->data.op = OP_TYPE_MUL;
        (*tree)->lt->data.num = -1;
        ++*count_changes;
    }

    return TREE_ERROR_SUCCESS;
}

void switch_tree_to_lt_ (tree_t** tree, FILE* out)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp = *tree;

    *tree = (*tree)->lt;
    (*tree)->pt = (temp->pt == temp ? (*tree) : temp->pt);

    temp->pt = temp;
    temp->lt = NULL;
    tree_update_size(temp);
    tree_dtor(temp); temp = NULL;
}

void switch_tree_to_rt_ (tree_t** tree, FILE* out)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp = *tree;

    *tree = (*tree)->rt;
    (*tree)->pt = (temp->pt == temp ? (*tree) : temp->pt);

    temp->pt = temp;
    temp->rt = NULL;
    tree_update_size(temp);
    tree_dtor(temp); temp = NULL;
}

void switch_tree_to_num_(tree_t** tree, FILE* out, const double num)
{
    lassert(!is_invalid_ptr(tree), "");
    TREE_VERIFY(*tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp = *tree;

    *tree = tree_ctor((tree_data_u){.num = num}, NODE_TYPE_NUM, 
                      ((*tree)->pt == *tree ? NULL : (*tree)->pt), NULL, NULL);

    temp->pt = temp;
    tree_dtor(temp); temp = NULL;
}