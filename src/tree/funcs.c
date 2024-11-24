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
        case OP_TYPE_ASIN:      return "arcsin";
        case OP_TYPE_ACOS:      return "arccos";
        case OP_TYPE_ATG:       return "arctg";
        case OP_TYPE_ACTG:      return "arcctg";
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
    if (strncmp("arcsin",   str, sizeof("arcsin")   - 1)   == 0)   return OP_TYPE_ASIN; 
    if (strncmp("arccos",   str, sizeof("arccos")   - 1)   == 0)   return OP_TYPE_ACOS; 
    if (strncmp("arctg",    str, sizeof("arctg")    - 1)   == 0)   return OP_TYPE_ATG; 
    if (strncmp("arcctg",   str, sizeof("arcctg")   - 1)   == 0)   return OP_TYPE_ACTG; 
    if (strncmp("sh",       str, sizeof("sh")       - 1)   == 0)   return OP_TYPE_SH; 
    if (strncmp("ch",       str, sizeof("ch")       - 1)   == 0)   return OP_TYPE_CH; 
    if (strncmp("th",       str, sizeof("th")       - 1)   == 0)   return OP_TYPE_TH; 
    if (strncmp("cth",      str, sizeof("cth")      - 1)   == 0)   return OP_TYPE_CTH; 
    if (strncmp("log",      str, sizeof("log")      - 1)   == 0)   return OP_TYPE_LOG; 

    return OP_TYPE_UNKNOWN;
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

tree_t* tree_copy_recursive_(const tree_t* const tree, tree_t* const pt);

tree_t* tree_copy(const tree_t* const tree)
{
    TREE_VERIFY(tree);

    tree_t* const new_tree = tree_copy_recursive_(tree, NULL);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* tree_copy_recursive_(const tree_t* const tree, tree_t* const pt)
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

    new_tree->lt = tree_copy_recursive_(tree->lt, new_tree);
    new_tree->rt = tree_copy_recursive_(tree->rt, new_tree);

    TREE_VERIFY(new_tree);

    return new_tree;
}

