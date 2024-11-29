#include <stdio.h>

#include "logger/liblogger.h"
#include "tree/funcs.h"
#include "tree/operation/op_diff.h"

tree_t* tree_copy(const tree_t* const tree, tree_t* const pt)
{
    if (tree == NULL) return NULL;

    TREE_VERIFY(tree);

    tree_t* const new_tree = tree_ctor(tree->data, tree->type, pt, NULL, NULL);

    if (!new_tree)
    {
        fprintf(stderr, "Can't ctor new tree\n");
        return NULL;
    } 

    new_tree->size = tree->size;

    new_tree->lt = tree_copy(tree->lt, new_tree);
    new_tree->rt = tree_copy(tree->rt, new_tree);

    TREE_VERIFY(new_tree);

    return new_tree;
}

#define OPERATION_HANDLE(_type, _name, _tex_name, _count_operands, _notation, _num)                 \
        case OP_TYPE_##_type: new_tree = diff_##_type(tree, pt, out); break;
    

tree_t* tree_diff(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    if (tree == NULL) return NULL;

    TREE_VERIFY(tree);

    if (tree->type == NODE_TYPE_NUM)
        return tree_ctor((tree_data_u){.num = 0}, NODE_TYPE_NUM, pt, NULL, NULL);
    
    if (tree->type == NODE_TYPE_VAR)
        return tree_ctor((tree_data_u){.num = 1}, NODE_TYPE_NUM, pt, NULL, NULL);
    
    tree_t* new_tree = NULL;

    switch (tree->data.op)
    { 
        #include "tree/operation/codegen.h"
               
        case MAX_OP_TYPE:
        default:
            new_tree = NULL;
            break;
    }

    // tree_dumb(tree);

    TREE_VERIFY(new_tree);

    return new_tree;
}

#undef OPERATION_HANDLE