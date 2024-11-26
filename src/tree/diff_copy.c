#include <stdio.h>

#include "logger/liblogger.h"
#include "tree/funcs.h"
#include "tree/diff_funcs.h"

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

#define DIFF_OP_(op_name) \
        case OP_TYPE_##op_name: new_tree = diff_##op_name(tree, pt, out); break
    

tree_t* tree_diff(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    if (tree == NULL) return NULL;

    TREE_VERIFY(tree);

// //file
//     if (out && tree_print_tex(out, tree))
//     {
//         fprintf(stderr, "Can't tree_print_tex\n");
//         return NULL;
//     }
// //file // TODO

    if (tree->type == NODE_TYPE_NUM)
        return tree_ctor(0, NODE_TYPE_NUM, pt, NULL, NULL);
    
    if (tree->type == NODE_TYPE_VAR)
        return tree_ctor(1, NODE_TYPE_NUM, pt, NULL, NULL);
    
    tree_t* new_tree = NULL;

    switch ((enum OpType)tree->data)
    { 
        DIFF_OP_(SUM);
        DIFF_OP_(SUB);
        DIFF_OP_(MUL);
        DIFF_OP_(DIV);
        DIFF_OP_(POW);
        DIFF_OP_(SIN);
        DIFF_OP_(COS);
        DIFF_OP_(TG);
        DIFF_OP_(CTG);
        DIFF_OP_(ARCSIN);
        DIFF_OP_(ARCCOS);
        DIFF_OP_(ARCTG);
        DIFF_OP_(ARCCTG);
        DIFF_OP_(SH);
        DIFF_OP_(CH);
        DIFF_OP_(TH);
        DIFF_OP_(CTH);
        DIFF_OP_(LOG);
               
        case OP_TYPE_UNKNOWN:
        default:
            new_tree = NULL;
            break;
    }

    TREE_VERIFY(new_tree);

    return new_tree;
}

#undef DIFF_OP_