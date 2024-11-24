#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#define _USE_MATH_DEFINES
#include <math.h>

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

tree_t* tree_diff_recursive_(const tree_t* const tree, tree_t* const pt, FILE* out);

tree_t* tree_diff(const tree_t* const tree, FILE* out)
{
    TREE_VERIFY(tree);

    tree_t* const new_tree = tree_diff_recursive_(tree, NULL, out);

    TREE_VERIFY(new_tree);

    return new_tree;
}

bool tree_include_var_(const tree_t* const tree);
void tree_update_size_(tree_t* const tree);

tree_t* tree_diff_recursive_(const tree_t* const tree, tree_t* const pt, FILE* out)
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
        case OP_TYPE_SUM:
        case OP_TYPE_SUB:
        {
            new_tree = tree_ctor(tree->data, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_diff_recursive_(tree->lt, new_tree, out);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);

            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_MUL:
        {
            new_tree     = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP, pt,       NULL, NULL);
            new_tree->lt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);
            new_tree->rt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);


            new_tree->lt->lt = tree_diff_recursive_(tree->lt, new_tree->lt, out);
            new_tree->lt->rt = tree_copy_recursive_(tree->rt, new_tree->lt);

            new_tree->rt->lt = tree_copy_recursive_(tree->lt, new_tree->rt);
            new_tree->rt->rt = tree_diff_recursive_(tree->rt, new_tree->rt, out);


            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_DIV:
        {
            new_tree         = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, pt,           NULL, NULL);

            //lt
            new_tree->lt     = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree,     NULL, NULL);

            new_tree->lt->lt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree->lt, NULL, NULL);
            new_tree->lt->rt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

            new_tree->lt->lt->lt = tree_diff_recursive_(tree->lt, new_tree->lt->lt, out);
            new_tree->lt->lt->rt = tree_copy_recursive_(tree->rt, new_tree->lt->lt);

            new_tree->lt->rt->lt = tree_copy_recursive_(tree->lt, new_tree->lt->rt);
            new_tree->lt->rt->rt = tree_diff_recursive_(tree->rt, new_tree->lt->rt, out);

            //rt
            new_tree->rt     = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP, new_tree, NULL, NULL);

            new_tree->rt->lt = tree_copy_recursive_(tree->rt, new_tree->rt);
            new_tree->rt->rt = tree_ctor(2, NODE_TYPE_NUM, new_tree->rt, NULL, NULL);


            tree_update_size_(new_tree->lt->lt);
            tree_update_size_(new_tree->lt->rt);
            tree_update_size_(new_tree->lt);

            tree_update_size_(new_tree->rt);

            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_POW:
        {
            const bool lt_include_var = tree_include_var_(tree->lt);
            const bool rt_include_var = tree_include_var_(tree->rt);

            if (!lt_include_var && !rt_include_var)
            {
                new_tree = tree_ctor(0, NODE_TYPE_NUM, pt, NULL, NULL);
                break;
            }

            if (lt_include_var && rt_include_var)
            {
                tree_t* temp_tree = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  NULL,      NULL, NULL);
                temp_tree->lt     = tree_ctor(E_NUM,       NODE_TYPE_NUM, temp_tree, NULL, NULL);
                temp_tree->rt     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  temp_tree, NULL, NULL);

                temp_tree->rt->lt = tree_copy_recursive_(tree->rt, temp_tree->rt);
                
                temp_tree->rt->rt = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree->rt, NULL, NULL);
                temp_tree->rt->rt->lt = 
                    tree_ctor(E_NUM, NODE_TYPE_NUM, temp_tree->rt->rt, NULL, NULL);
                temp_tree->rt->rt->rt = tree_copy_recursive_(tree->lt, temp_tree->rt->rt);

                tree_update_size_(temp_tree->rt->rt);
                tree_update_size_(temp_tree->rt);
                tree_update_size_(temp_tree);

                new_tree = tree_diff_recursive_(temp_tree, pt, out);

                tree_dtor(temp_tree);
                break;
            }

            if (lt_include_var && !rt_include_var)
            {
                new_tree     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,       NULL, NULL);
                new_tree->lt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);
                new_tree->rt = tree_diff_recursive_(tree->lt, new_tree, out);

                new_tree->lt->lt = tree_copy_recursive_(tree->rt, new_tree->lt);
                new_tree->lt->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

                new_tree->lt->rt->lt = tree_copy_recursive_(tree->lt, new_tree->lt->rt);
                new_tree->lt->rt->rt = 
                    tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree->lt->rt, NULL, NULL);
                
                new_tree->lt->rt->rt->lt = tree_copy_recursive_(tree->rt, new_tree->lt->rt->rt);
                new_tree->lt->rt->rt->rt
                    = tree_ctor(1, NODE_TYPE_NUM, new_tree->lt->rt->rt, NULL, NULL);
                
                tree_update_size_(new_tree->lt->rt->rt);
                tree_update_size_(new_tree->lt->rt);
                tree_update_size_(new_tree->lt);
                tree_update_size_(new_tree);
                break;
            }

            new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->lt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);

            new_tree->lt->rt = tree_copy_recursive_(tree, new_tree->lt);
            new_tree->lt->lt = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

            new_tree->lt->lt->lt = tree_ctor(E_NUM, NODE_TYPE_NUM, new_tree->lt->lt, NULL, NULL);
            new_tree->lt->lt->rt = tree_copy_recursive_(tree->lt, new_tree->lt->lt);

            tree_update_size_(new_tree->lt->lt);
            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_SIN:
        {
            new_tree     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_ctor(OP_TYPE_COS, NODE_TYPE_OP, new_tree, NULL, NULL);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);

            new_tree->lt->rt = tree_copy_recursive_(tree->rt, new_tree->lt);

            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_COS:
        {
            new_tree     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);

            new_tree->lt->lt = tree_ctor(-1, NODE_TYPE_NUM, new_tree->lt, NULL, NULL);
            new_tree->lt->rt = tree_ctor(OP_TYPE_SIN, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

            new_tree->lt->rt->rt = tree_copy_recursive_(tree->rt, new_tree->lt->rt);

            tree_update_size_(new_tree->lt->rt);
            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_TG:
        {
            new_tree     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,       NULL, NULL);

            new_tree->lt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree, NULL, NULL);

            new_tree->rt->lt = tree_ctor(OP_TYPE_COS, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);
            new_tree->rt->rt = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt, NULL, NULL);

            new_tree->rt->lt->rt = tree_copy_recursive_(tree->rt, new_tree->rt->lt);

            tree_update_size_(new_tree->rt->lt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }        
        case OP_TYPE_CTG:
        {
            new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);

            new_tree->lt     = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
            new_tree->rt     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

            new_tree->rt->lt = tree_diff_recursive_(tree->rt, new_tree->rt, out);
            new_tree->rt->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

            new_tree->rt->rt->lt 
                = tree_ctor(OP_TYPE_SIN, NODE_TYPE_OP,  new_tree->rt->rt, NULL, NULL);
            new_tree->rt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

            new_tree->rt->rt->lt->rt = tree_copy_recursive_(tree->rt, new_tree->rt->rt->lt);

            tree_update_size_(new_tree->rt->rt->lt);
            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_ASIN:
        {
            new_tree     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,       NULL, NULL);

            new_tree->lt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree, NULL, NULL);

            new_tree->rt->lt = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree->rt, NULL, NULL);
            new_tree->rt->rt = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, new_tree->rt, NULL, NULL);

            new_tree->rt->lt->lt 
                = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->lt, NULL, NULL);
            new_tree->rt->lt->rt 
                = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->lt, NULL, NULL);
            new_tree->rt->rt->lt 
                = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);
            new_tree->rt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);
            
            new_tree->rt->lt->rt->lt = tree_copy_recursive_(tree->rt, new_tree->rt->lt->rt);
            new_tree->rt->lt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->lt->rt, NULL, NULL);

            tree_update_size_(new_tree->rt->lt->rt);
            tree_update_size_(new_tree->rt->lt);
            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }      
        case OP_TYPE_ACOS:
        {
            new_tree         = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  pt,           NULL, NULL);

            new_tree->lt     = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
            new_tree->rt     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

            new_tree->rt->lt = tree_diff_recursive_(tree->rt, new_tree->rt, out);
            new_tree->rt->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

            new_tree->rt->rt->lt = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree->rt->rt, NULL, NULL);
            new_tree->rt->rt->rt = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, new_tree->rt->rt, NULL, NULL);

            new_tree->rt->rt->lt->lt 
                = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt->lt, NULL, NULL);
            new_tree->rt->rt->lt->rt 
                = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->rt->lt, NULL, NULL);
            new_tree->rt->rt->rt->lt 
                = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);
            new_tree->rt->rt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);
            
            new_tree->rt->rt->lt->rt->lt = tree_copy_recursive_(tree->rt, new_tree->rt->rt->lt->rt);
            new_tree->rt->rt->lt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->lt->rt, NULL, NULL);

            tree_update_size_(new_tree->rt->rt->lt->rt);
            tree_update_size_(new_tree->rt->rt->lt);
            tree_update_size_(new_tree->rt->rt->rt);
            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }      
        case OP_TYPE_ATG:
        {
            new_tree     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,       NULL, NULL);

            new_tree->lt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->rt = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP,  new_tree, NULL, NULL);

            new_tree->rt->lt = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt,     NULL, NULL);
            new_tree->rt->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt,     NULL, NULL);
            
            new_tree->rt->rt->lt = tree_copy_recursive_(tree->rt, new_tree->rt->rt);
            new_tree->rt->rt->rt = tree_ctor(2,       NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_ACTG:
        {
            new_tree         = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  pt,           NULL, NULL);
            new_tree->lt     = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
            new_tree->rt     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

            new_tree->rt->lt = tree_diff_recursive_(tree->rt, new_tree->rt, out);
            new_tree->rt->rt = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

            new_tree->rt->rt->lt = 
                tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt,     NULL, NULL);
            new_tree->rt->rt->rt = 
                tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->rt,     NULL, NULL);
            
            new_tree->rt->rt->rt->lt = tree_copy_recursive_(tree->rt, new_tree->rt->rt->rt);
            new_tree->rt->rt->rt->rt = 
                tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);

            tree_update_size_(new_tree->rt->rt->rt);
            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }      
        case OP_TYPE_SH:
        {
            new_tree     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_ctor(OP_TYPE_CH,  NODE_TYPE_OP, new_tree, NULL, NULL);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);

            new_tree->lt->rt = tree_copy_recursive_(tree->rt, new_tree->lt);

            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree);
            break;
        }        
        case OP_TYPE_CH:
        {
            new_tree     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_ctor(OP_TYPE_SH,  NODE_TYPE_OP, new_tree, NULL, NULL);
            new_tree->rt = tree_diff_recursive_(tree->rt, new_tree, out);

            new_tree->lt->rt = tree_copy_recursive_(tree->rt, new_tree->lt);

            tree_update_size_(new_tree->lt);
            tree_update_size_(new_tree);
            break;
        }        
        case OP_TYPE_TH:
        {
            new_tree     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,       NULL, NULL);

            new_tree->lt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree, NULL, NULL);

            new_tree->rt->lt = tree_ctor(OP_TYPE_CH,  NODE_TYPE_OP,  new_tree->rt, NULL, NULL);
            new_tree->rt->rt = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt, NULL, NULL);

            new_tree->rt->lt->rt = tree_copy_recursive_(tree->rt, new_tree->rt->lt);

            tree_update_size_(new_tree->rt->lt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }        
        case OP_TYPE_CTH:
        {
            new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);

            new_tree->lt     = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
            new_tree->rt     = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

            new_tree->rt->lt = tree_diff_recursive_(tree->rt, new_tree->rt, out);
            new_tree->rt->rt = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

            new_tree->rt->rt->lt 
                = tree_ctor(OP_TYPE_SH,  NODE_TYPE_OP,  new_tree->rt->rt, NULL, NULL);
            new_tree->rt->rt->rt 
                = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

            new_tree->rt->rt->lt->rt = tree_copy_recursive_(tree->rt, new_tree->rt->rt->lt);

            tree_update_size_(new_tree->rt->rt->lt);
            tree_update_size_(new_tree->rt->rt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_LOG:
        {
            if (tree_include_var_(tree->lt))
            {
                tree_t* temp_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, NULL, NULL, NULL);

                if(tree_include_var_(tree->rt))
                {
                    temp_tree->lt     = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree, NULL, NULL);
                    temp_tree->rt     = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree, NULL, NULL);

                    temp_tree->lt->lt = tree_ctor(E_NUM, NODE_TYPE_NUM, temp_tree->lt, NULL, NULL);
                    temp_tree->lt->rt = tree_copy_recursive_(tree->rt, temp_tree->lt);
                    temp_tree->rt->lt = tree_ctor(E_NUM, NODE_TYPE_NUM, temp_tree->rt, NULL, NULL);
                    temp_tree->rt->rt = tree_copy_recursive_(tree->lt, temp_tree->rt);

                    tree_update_size_(temp_tree->lt);
                    tree_update_size_(temp_tree->rt);
                }
                else
                {
                    temp_tree->lt = tree_ctor(1,           NODE_TYPE_NUM, temp_tree, NULL, NULL);
                    temp_tree->rt = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP,  temp_tree, NULL, NULL);

                    temp_tree->rt->lt = tree_copy_recursive_(tree->rt, temp_tree->rt);
                    temp_tree->rt->rt = tree_copy_recursive_(tree->lt, temp_tree->rt);

                    tree_update_size_(temp_tree->rt);
                }

                tree_update_size_(temp_tree);

                new_tree = tree_diff_recursive_(temp_tree, pt, out);
                
                tree_dtor(temp_tree); temp_tree = NULL;
                break;
            }

            new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, pt, NULL, NULL);
            new_tree->lt = tree_diff_recursive_(tree->rt, new_tree, out);
            new_tree->rt = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);

            new_tree->rt->lt = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, new_tree->rt, NULL, NULL);
            new_tree->rt->rt = tree_copy_recursive_(tree->rt, new_tree->rt);

            new_tree->rt->lt->lt = tree_ctor(E_NUM, NODE_TYPE_NUM, new_tree->rt->lt, NULL, NULL);
            new_tree->rt->lt->rt = tree_copy_recursive_(tree->lt, new_tree->rt->lt);

            tree_update_size_(new_tree->rt->lt);
            tree_update_size_(new_tree->rt);
            tree_update_size_(new_tree);
            break;
        }       
        case OP_TYPE_UNKNOWN:
        {
            break;
        }   
    
    default:
        break;
    }


    TREE_VERIFY(new_tree);

    return new_tree;
}

bool tree_include_var_(const tree_t* const tree)
{
    if (tree == NULL) return false;

    TREE_VERIFY(tree);

    return (tree->type == NODE_TYPE_VAR) 
        || (tree_include_var_(tree->lt) )
        || (tree_include_var_(tree->rt) );
}

void tree_update_size_(tree_t* const tree)
{
    lassert(!is_invalid_ptr(tree), "");

    tree->size = (tree->lt ? tree->lt->size : 0) + (tree->rt ? tree->rt->size : 0) + 1;
}