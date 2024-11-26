#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

#include "tree/funcs.h"
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
        case OP_TYPE_SUM:       return "+";
        case OP_TYPE_SUB:       return "-";
        case OP_TYPE_MUL:       return "*";
        case OP_TYPE_DIV:       return "/";
        case OP_TYPE_POW:       return "^";
        case OP_TYPE_SIN:       return "sin";
        case OP_TYPE_COS:       return "cos";
        case OP_TYPE_TG:        return "tg";
        case OP_TYPE_CTG:       return "ctg";
        case OP_TYPE_ARCSIN:    return "arcsin";
        case OP_TYPE_ARCCOS:    return "arccos";
        case OP_TYPE_ARCTG:     return "arctg";
        case OP_TYPE_ARCCTG:    return "arcctg";
        case OP_TYPE_SH:        return "sh";
        case OP_TYPE_CH:        return "ch";
        case OP_TYPE_TH:        return "th";
        case OP_TYPE_CTH:       return "cth";
        case OP_TYPE_LOG:       return "log";
        case OP_TYPE_UNKNOWN:   return "UNKNOWN";
    default:    
        return "UNKNOWN_OP_TYPE";
    }
    return "UNKNOWN_OP_TYPE";
}

enum OpType str_to_op_type(const char* const str)
{
    lassert(!is_invalid_ptr(str), "");

    if (strncmp("+",        str, sizeof("+")        - 1)   == 0)   return OP_TYPE_SUM;
    if (strncmp("-",        str, sizeof("-")        - 1)   == 0)   return OP_TYPE_SUB;
    if (strncmp("*",        str, sizeof("*")        - 1)   == 0)   return OP_TYPE_MUL;
    if (strncmp("/",        str, sizeof("/")        - 1)   == 0)   return OP_TYPE_DIV;
    if (strncmp("^",        str, sizeof("^")        - 1)   == 0)   return OP_TYPE_POW;
    if (strncmp("sin",      str, sizeof("sin")      - 1)   == 0)   return OP_TYPE_SIN; 
    if (strncmp("cos",      str, sizeof("cos")      - 1)   == 0)   return OP_TYPE_COS; 
    if (strncmp("tg",       str, sizeof("tg")       - 1)   == 0)   return OP_TYPE_TG; 
    if (strncmp("ctg",      str, sizeof("ctg")      - 1)   == 0)   return OP_TYPE_CTG; 
    if (strncmp("arcsin",   str, sizeof("arcsin")   - 1)   == 0)   return OP_TYPE_ARCSIN; 
    if (strncmp("arccos",   str, sizeof("arccos")   - 1)   == 0)   return OP_TYPE_ARCCOS; 
    if (strncmp("arctg",    str, sizeof("arctg")    - 1)   == 0)   return OP_TYPE_ARCTG; 
    if (strncmp("arcctg",   str, sizeof("arcctg")   - 1)   == 0)   return OP_TYPE_ARCCTG; 
    if (strncmp("sh",       str, sizeof("sh")       - 1)   == 0)   return OP_TYPE_SH; 
    if (strncmp("ch",       str, sizeof("ch")       - 1)   == 0)   return OP_TYPE_CH; 
    if (strncmp("th",       str, sizeof("th")       - 1)   == 0)   return OP_TYPE_TH; 
    if (strncmp("cth",      str, sizeof("cth")      - 1)   == 0)   return OP_TYPE_CTH; 
    if (strncmp("log",      str, sizeof("log")      - 1)   == 0)   return OP_TYPE_LOG; 

    return OP_TYPE_UNKNOWN;
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