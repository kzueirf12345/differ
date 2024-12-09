#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

#include "tree/funcs/funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree/operation/operation.h"

const char* node_type_to_str(const enum NodeType type)
{
    switch (type)
    {
        case NODE_TYPE_NUM: return "NUM";
        case NODE_TYPE_VAR: return "VAR";
        case NODE_TYPE_OP:  return "OP";
    default:
        return "UNKNOWN_NODE_TYPE";
    }
    return "UNKNOWN_NODE_TYPE";
}

#define OPERATION_HANDLE(_type, _name, _tex_name, temp, _count_operands, _notation, _num)                 \
        if (strncmp(_name, str, sizeof(_name) - 1) == 0) return OP_TYPE_##_type;

enum OpType str_to_op_type(const char* const str)
{
    lassert(!is_invalid_ptr(str), "");

    #include "tree/operation/codegen.h"

    return MAX_OP_TYPE;
}

tree_t* tree_ctor(tree_data_u data, enum NodeType type, 
                  tree_t* const pt, tree_t* const lt, tree_t* const rt)
{
    tree_t* tree = (tree_t*)calloc(1, sizeof(tree_t));

    if (!tree)
    {
        perror("Can't calloc tree tree");
        return NULL;
    }

    tree->size = 1;
    
    if (pt)
    {
        tree->pt = pt;
    }
    else
    {
        tree->pt = tree;
    }
    if (lt)
    {
        tree->lt = lt;
        tree->size += lt->size;
    }
    if (rt)
    {
        tree->rt = rt;
        tree->size += rt->size;
    }

    tree->data = data;
    tree->type = type;

    // TREE_VERIFY(tree); i remove it eeeee
    return tree;
}

void tree_fill_pt(tree_t* const tree, tree_t* const pt)
{
    if (tree == NULL || tree->pt != tree) return;

    lassert(!is_invalid_ptr(tree), "");

    tree->pt = (pt ? pt : tree);

    tree_fill_pt(tree->lt, tree);
    tree_fill_pt(tree->rt, tree);
}

void tree_dtor(tree_t* const tree)
{
    TREE_VERIFY(tree);

#ifndef NDEBUG
    tree->size = 0;
    tree->pt   = NULL;
#endif /*NDEBUG*/

    if(tree->lt)
    {
        tree_dtor(tree->lt);
        tree->lt   = NULL;  
    }
    if(tree->rt)
    {
        tree_dtor(tree->rt);
        tree->rt   = NULL;  
    }

    free(tree);
}

bool tree_include_var(const tree_t* const tree)
{
    if (tree == NULL) return false;

    TREE_VERIFY(tree);

    return (tree->type == NODE_TYPE_VAR) 
        || (tree_include_var(tree->lt) )
        || (tree_include_var(tree->rt) );
}

void tree_update_size(tree_t* const tree)
{
    lassert(!is_invalid_ptr(tree), "");

    tree->size = (tree->lt ? tree->lt->size : 0) + (tree->rt ? tree->rt->size : 0) + 1;
}