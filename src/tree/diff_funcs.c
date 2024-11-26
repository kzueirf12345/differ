#include <stdio.h>

#include "tree/funcs.h"
#include "tree/diff_funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

tree_t* diff_SUM (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP, pt, NULL, NULL);
    new_tree->lt = tree_diff(tree->lt, new_tree, out);
    new_tree->rt = tree_diff(tree->rt, new_tree, out);

    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_SUB (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, pt, NULL, NULL);
    new_tree->lt = tree_diff(tree->lt, new_tree, out);
    new_tree->rt = tree_diff(tree->rt, new_tree, out);

    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_MUL (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP, pt,       NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);
    new_tree->rt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree, NULL, NULL);

    new_tree->lt->lt = tree_diff(tree->lt, new_tree->lt, out);
    new_tree->lt->rt = tree_copy(tree->rt, new_tree->lt);

    new_tree->rt->lt = tree_copy(tree->lt, new_tree->rt);
    new_tree->rt->rt = tree_diff(tree->rt, new_tree->rt, out);

    tree_update_size(new_tree->lt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_DIV (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, pt,           NULL, NULL);

    //lt
    new_tree->lt           = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree,     NULL, NULL);
       
    new_tree->lt->lt       = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree->lt, NULL, NULL);
    new_tree->lt->rt       = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

    new_tree->lt->lt->lt   = tree_diff(tree->lt, new_tree->lt->lt, out);
    new_tree->lt->lt->rt   = tree_copy(tree->rt, new_tree->lt->lt);

    new_tree->lt->rt->lt   = tree_copy(tree->lt, new_tree->lt->rt);
    new_tree->lt->rt->rt   = tree_diff(tree->rt, new_tree->lt->rt, out);

    //rt
    new_tree->rt           = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP, new_tree,     NULL, NULL);
       
    new_tree->rt->lt       = tree_copy(tree->rt, new_tree->rt);
    new_tree->rt->rt       = tree_ctor(2,          NODE_TYPE_NUM, new_tree->rt, NULL, NULL);


    tree_update_size(new_tree->lt->lt);
    tree_update_size(new_tree->lt->rt);
    tree_update_size(new_tree->lt);

    tree_update_size(new_tree->rt);

    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_POW_lt_rt_ (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_POW_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_POW_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out);

tree_t* diff_POW (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    const bool lt_include_var = tree_include_var(tree->lt);
    const bool rt_include_var = tree_include_var(tree->rt);

    if (!lt_include_var && !rt_include_var) { return tree_ctor(0, NODE_TYPE_NUM, pt, NULL, NULL); }
    if ( lt_include_var &&  rt_include_var) { return diff_POW_lt_rt_ (tree, pt, out);             }
    if ( lt_include_var && !rt_include_var) { return diff_POW_lt_nrt_(tree, pt, out);             }
                                              return diff_POW_nlt_rt_(tree, pt, out);
}

tree_t* diff_POW_lt_rt_ (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp_tree = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  NULL,      NULL, NULL);
    temp_tree->lt     = tree_ctor(E_NUM,       NODE_TYPE_NUM, temp_tree, NULL, NULL);
    temp_tree->rt     = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  temp_tree, NULL, NULL);

    temp_tree->rt->lt = tree_copy(tree->rt, temp_tree->rt);
    
    temp_tree->rt->rt = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree->rt, NULL, NULL);
    temp_tree->rt->rt->lt = 
        tree_ctor(E_NUM, NODE_TYPE_NUM, temp_tree->rt->rt, NULL, NULL);
    temp_tree->rt->rt->rt = tree_copy(tree->lt, temp_tree->rt->rt);

    tree_update_size(temp_tree->rt->rt);
    tree_update_size(temp_tree->rt);
    tree_update_size(temp_tree);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);

    tree_dtor(temp_tree); temp_tree = NULL;

    return new_tree;
}

tree_t* diff_POW_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,           NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree,     NULL, NULL);
    new_tree->rt           = tree_diff(tree->lt, new_tree, out);

    new_tree->lt->lt       = tree_copy(tree->rt, new_tree->lt);
    new_tree->lt->rt       = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

    new_tree->lt->rt->lt = tree_copy(tree->lt, new_tree->lt->rt);
    new_tree->lt->rt->rt = 
        tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP,  new_tree->lt->rt,     NULL, NULL);

    new_tree->lt->rt->rt->lt = tree_copy(tree->rt, new_tree->lt->rt->rt);
    new_tree->lt->rt->rt->rt
        = tree_ctor(1,         NODE_TYPE_NUM, new_tree->lt->rt->rt, NULL, NULL);

    tree_update_size(new_tree->lt->rt->rt);
    tree_update_size(new_tree->lt->rt);
    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_POW_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,               NULL, NULL);
    new_tree->rt           = tree_diff(tree->rt, new_tree, out);
    new_tree->lt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree,         NULL, NULL);

    new_tree->lt->rt       = tree_copy(tree, new_tree->lt);
    new_tree->lt->lt       = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, new_tree->lt,     NULL, NULL);

    new_tree->lt->lt->lt   = tree_ctor(E_NUM,      NODE_TYPE_NUM, new_tree->lt->lt, NULL, NULL);
    new_tree->lt->lt->rt   = tree_copy(tree->lt, new_tree->lt->lt);

    tree_update_size(new_tree->lt->lt);
    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_SIN (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,       NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_COS, NODE_TYPE_OP, new_tree, NULL, NULL);
    new_tree->rt           = tree_diff(tree->rt, new_tree, out);

    new_tree->lt->rt       = tree_copy(tree->rt, new_tree->lt);

    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_COS (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,           NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree,     NULL, NULL);
    new_tree->rt           = tree_diff(tree->rt, new_tree, out);

    new_tree->lt->lt       = tree_ctor(-1,         NODE_TYPE_NUM, new_tree->lt, NULL, NULL);
    new_tree->lt->rt       = tree_ctor(OP_TYPE_SIN, NODE_TYPE_OP, new_tree->lt, NULL, NULL);

    new_tree->lt->rt->rt   = tree_copy(tree->rt, new_tree->lt->rt);

    tree_update_size(new_tree->lt->rt);
    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_TG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,           NULL, NULL);

    new_tree->lt           = tree_diff(tree->rt, new_tree, out);
    new_tree->rt           = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree,     NULL, NULL);

    new_tree->rt->lt       = tree_ctor(OP_TYPE_COS, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);
    new_tree->rt->rt       = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt, NULL, NULL);

    new_tree->rt->lt->rt   = tree_copy(tree->rt, new_tree->rt->lt);

    tree_update_size(new_tree->rt->lt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_CTG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  pt,           NULL, NULL);

    new_tree->lt           = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
    new_tree->rt           = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

    new_tree->rt->lt       = tree_diff(tree->rt, new_tree->rt, out);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

    new_tree->rt->rt->lt 
        = tree_ctor(OP_TYPE_SIN, NODE_TYPE_OP,  new_tree->rt->rt, NULL, NULL);
    new_tree->rt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

    new_tree->rt->rt->lt->rt = tree_copy(tree->rt, new_tree->rt->rt->lt);

    tree_update_size(new_tree->rt->rt->lt);
    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_ASIN (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,          NULL, NULL);

    new_tree->lt           = tree_diff(tree->rt, new_tree, out);
    new_tree->rt           = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree,    NULL, NULL);

    new_tree->rt->lt       = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree->rt, NULL, NULL);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, new_tree->rt, NULL, NULL);

    new_tree->rt->lt->lt 
        = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->lt, NULL, NULL);
    new_tree->rt->lt->rt 
        = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->lt, NULL, NULL);
    new_tree->rt->rt->lt 
        = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);
    new_tree->rt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);
    
    new_tree->rt->lt->rt->lt = tree_copy(tree->rt, new_tree->rt->lt->rt);
    new_tree->rt->lt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->lt->rt, NULL, NULL);

    tree_update_size(new_tree->rt->lt->rt);
    tree_update_size(new_tree->rt->lt);
    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_ACOS (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  pt,              NULL, NULL);

    new_tree->lt           = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,        NULL, NULL);
    new_tree->rt           = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,        NULL, NULL);

    new_tree->rt->lt       = tree_diff(tree->rt, new_tree->rt, out);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt,    NULL, NULL);

    new_tree->rt->rt->lt   = tree_ctor(OP_TYPE_SUB, NODE_TYPE_OP, new_tree->rt->rt, NULL, NULL);
    new_tree->rt->rt->rt   = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, new_tree->rt->rt, NULL, NULL);

    new_tree->rt->rt->lt->lt 
        = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt->lt, NULL, NULL);
    new_tree->rt->rt->lt->rt 
        = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->rt->lt, NULL, NULL);
    new_tree->rt->rt->rt->lt 
        = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);
    new_tree->rt->rt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);
    
    new_tree->rt->rt->lt->rt->lt = tree_copy(tree->rt, new_tree->rt->rt->lt->rt);
    new_tree->rt->rt->lt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->lt->rt, NULL, NULL);

    tree_update_size(new_tree->rt->rt->lt->rt);
    tree_update_size(new_tree->rt->rt->lt);
    tree_update_size(new_tree->rt->rt->rt);
    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_ATG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,               NULL, NULL);

    new_tree->lt           = tree_diff(tree->rt, new_tree, out);
    new_tree->rt           = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP,  new_tree,         NULL, NULL);

    new_tree->rt->lt       = tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt,     NULL, NULL);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt,     NULL, NULL);
    
    new_tree->rt->rt->lt   = tree_copy(tree->rt, new_tree->rt->rt);
    new_tree->rt->rt->rt   = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_ACTG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP,  pt,           NULL, NULL);
    new_tree->lt           = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
    new_tree->rt           = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

    new_tree->rt->lt       = tree_diff(tree->rt, new_tree->rt, out);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_SUM, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

    new_tree->rt->rt->lt     = 
        tree_ctor(1,           NODE_TYPE_NUM, new_tree->rt->rt,     NULL, NULL);
    new_tree->rt->rt->rt     = 
        tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt->rt,     NULL, NULL);
    
    new_tree->rt->rt->rt->lt = tree_copy(tree->rt, new_tree->rt->rt->rt);
    new_tree->rt->rt->rt->rt = 
        tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt->rt, NULL, NULL);

    tree_update_size(new_tree->rt->rt->rt);
    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_SH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt, NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_CH,  NODE_TYPE_OP, new_tree, NULL, NULL);
    new_tree->rt           = tree_diff(tree->rt, new_tree, out);

    new_tree->lt->rt       = tree_copy(tree->rt, new_tree->lt);

    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_CH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,       NULL, NULL);
    new_tree->lt           = tree_ctor(OP_TYPE_SH,  NODE_TYPE_OP, new_tree, NULL, NULL);
    new_tree->rt           = tree_diff(tree->rt, new_tree, out);

    new_tree->lt->rt       = tree_copy(tree->rt, new_tree->lt);

    tree_update_size(new_tree->lt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_TH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  pt,           NULL, NULL);

    new_tree->lt           = tree_diff(tree->rt, new_tree, out);
    new_tree->rt           = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree,     NULL, NULL);

    new_tree->rt->lt       = tree_ctor(OP_TYPE_CH,  NODE_TYPE_OP,  new_tree->rt, NULL, NULL);
    new_tree->rt->rt       = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt, NULL, NULL);

    new_tree->rt->lt->rt   = tree_copy(tree->rt, new_tree->rt->lt);

    tree_update_size(new_tree->rt->lt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_CTH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, pt,            NULL, NULL);

    new_tree->lt           = tree_ctor(-1,          NODE_TYPE_NUM, new_tree,     NULL, NULL);
    new_tree->rt           = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  new_tree,     NULL, NULL);

    new_tree->rt->lt       = tree_diff(tree->rt, new_tree->rt, out);
    new_tree->rt->rt       = tree_ctor(OP_TYPE_POW, NODE_TYPE_OP,  new_tree->rt, NULL, NULL);

    new_tree->rt->rt->lt 
        = tree_ctor(OP_TYPE_SH,  NODE_TYPE_OP,  new_tree->rt->rt, NULL, NULL);
    new_tree->rt->rt->rt 
        = tree_ctor(2,           NODE_TYPE_NUM, new_tree->rt->rt, NULL, NULL);

    new_tree->rt->rt->lt->rt = tree_copy(tree->rt, new_tree->rt->rt->lt);

    tree_update_size(new_tree->rt->rt->lt);
    tree_update_size(new_tree->rt->rt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}

tree_t* diff_LOG_lt_rt_ (const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_LOG_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out);
tree_t* diff_LOG_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out);

tree_t* diff_LOG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    const bool lt_include_var = tree_include_var(tree->lt);
    const bool rt_include_var = tree_include_var(tree->rt);

    if (!lt_include_var && !rt_include_var) { return tree_ctor(0, NODE_TYPE_NUM, pt, NULL, NULL); }
    if ( lt_include_var &&  rt_include_var) { return diff_LOG_lt_rt_ (tree, pt, out);             }
    if ( lt_include_var && !rt_include_var) { return diff_LOG_lt_nrt_(tree, pt, out);             }
                                              return diff_LOG_nlt_rt_(tree, pt, out);
}

tree_t* diff_LOG_lt_rt_ (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, NULL,          NULL, NULL);

    temp_tree->lt     = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree,     NULL, NULL);
    temp_tree->rt     = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, temp_tree,     NULL, NULL);

    temp_tree->lt->lt = tree_ctor(E_NUM,      NODE_TYPE_NUM, temp_tree->lt, NULL, NULL);
    temp_tree->lt->rt = tree_copy(tree->rt, temp_tree->lt);
    temp_tree->rt->lt = tree_ctor(E_NUM,      NODE_TYPE_NUM, temp_tree->rt, NULL, NULL);
    temp_tree->rt->rt = tree_copy(tree->lt, temp_tree->rt);

    tree_update_size(temp_tree->lt);
    tree_update_size(temp_tree->rt);
    tree_update_size(temp_tree);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    return new_tree;
}

tree_t* diff_LOG_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP,  NULL,      NULL, NULL);

    temp_tree->lt     = tree_ctor(1,           NODE_TYPE_NUM, temp_tree, NULL, NULL);
    temp_tree->rt     = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP,  temp_tree, NULL, NULL);

    temp_tree->rt->lt = tree_copy(tree->rt, temp_tree->rt);
    temp_tree->rt->rt = tree_copy(tree->lt, temp_tree->rt);

    tree_update_size(temp_tree->rt);
    tree_update_size(temp_tree);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    return new_tree;
}

tree_t* diff_LOG_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = tree_ctor(OP_TYPE_DIV, NODE_TYPE_OP, pt,                NULL, NULL);
    new_tree->lt           = tree_diff(tree->rt, new_tree, out);
    new_tree->rt           = tree_ctor(OP_TYPE_MUL, NODE_TYPE_OP, new_tree,          NULL, NULL);

    new_tree->rt->lt       = tree_ctor(OP_TYPE_LOG, NODE_TYPE_OP, new_tree->rt,      NULL, NULL);
    new_tree->rt->rt       = tree_copy(tree->rt, new_tree->rt);

    new_tree->rt->lt->lt   = tree_ctor(E_NUM,      NODE_TYPE_NUM, new_tree->rt->lt,  NULL, NULL);
    new_tree->rt->lt->rt   = tree_copy(tree->lt, new_tree->rt->lt);

    tree_update_size(new_tree->rt->lt);
    tree_update_size(new_tree->rt);
    tree_update_size(new_tree);

    return new_tree;
}