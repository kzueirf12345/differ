#include <string.h>

#include "tree_funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"


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


const char* op_type_to_str(const enum OpType type)
{
    switch (type)
    {
        case OP_TYPE_SUM:   return "+";
        case OP_TYPE_SUB:   return "-";
        case OP_TYPE_MUL:   return "*";
        case OP_TYPE_DIV:   return "/";
        case OP_TYPE_POW:   return "^";
        case OP_TYPE_SIN:   return "sin";
        case OP_TYPE_COS:   return "cos";
        case OP_TYPE_TG:    return "tg";
        case OP_TYPE_CTG:   return "ctg";
        case OP_TYPE_ASIN:  return "arcsin";
        case OP_TYPE_ACOS:  return "arccos";
        case OP_TYPE_ATG:   return "arctg";
        case OP_TYPE_ACTG:  return "arcctg";
        case OP_TYPE_SH:    return "sh";
        case OP_TYPE_CH:    return "ch";
        case OP_TYPE_TH:    return "th";
        case OP_TYPE_CTH:   return "cth";
        case OP_TYPE_LOG:   return "log";
    default:    
        return "UNKNOWN_OP_TYPE";
    }
    return "UNKNOWN_OP_TYPE";
}

tree_t* tree_ctor(int data, enum NodeType type, 
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

    TREE_VERIFY(tree);
    return tree;
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