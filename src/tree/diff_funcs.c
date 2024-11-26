#include <stdio.h>

#include "tree/funcs.h"
#include "tree/diff_funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"


#define _SUM(lt, rt)    tree_ctor(OP_TYPE_SUM,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _SUB(lt, rt)    tree_ctor(OP_TYPE_SUB,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _MUL(lt, rt)    tree_ctor(OP_TYPE_MUL,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _DIV(lt, rt)    tree_ctor(OP_TYPE_DIV,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _POW(lt, rt)    tree_ctor(OP_TYPE_POW,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _SIN(lt, rt)    tree_ctor(OP_TYPE_SIN,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _COS(lt, rt)    tree_ctor(OP_TYPE_COS,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _TG(lt, rt)     tree_ctor(OP_TYPE_TG,      NODE_TYPE_OP,    NULL,   lt,     rt)
#define _CTG(lt, rt)    tree_ctor(OP_TYPE_CTG,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _ARCSIN(lt, rt) tree_ctor(OP_TYPE_ARCSIN,  NODE_TYPE_OP,    NULL,   lt,     rt)
#define _ARCCOS(lt, rt) tree_ctor(OP_TYPE_ARCCOS,  NODE_TYPE_OP,    NULL,   lt,     rt)
#define _ARCTG(lt, rt)  tree_ctor(OP_TYPE_ARCTG,   NODE_TYPE_OP,    NULL,   lt,     rt)
#define _ARCCTG(lt, rt) tree_ctor(OP_TYPE_ARCCTG,  NODE_TYPE_OP,    NULL,   lt,     rt)
#define _SH(lt, rt)     tree_ctor(OP_TYPE_SH,      NODE_TYPE_OP,    NULL,   lt,     rt)
#define _CH(lt, rt)     tree_ctor(OP_TYPE_CH,      NODE_TYPE_OP,    NULL,   lt,     rt)
#define _TH(lt, rt)     tree_ctor(OP_TYPE_TH,      NODE_TYPE_OP,    NULL,   lt,     rt)
#define _CTH(lt, rt)    tree_ctor(OP_TYPE_CTH,     NODE_TYPE_OP,    NULL,   lt,     rt)
#define _LOG(lt, rt)    tree_ctor(OP_TYPE_LOG,     NODE_TYPE_OP,    NULL,   lt,     rt)

#define _NUM(num)       tree_ctor(num,             NODE_TYPE_NUM,   NULL,   NULL,   NULL)

#define _cLT    tree_copy(tree->lt, NULL)
#define _cRT    tree_copy(tree->rt, NULL)
#define _cTREE  tree_copy(tree,     NULL)
#define _dLT    tree_diff(tree->lt, NULL, out)
#define _dRT    tree_diff(tree->rt, NULL, out)
#define _dTREE  tree_diff(tree,     NULL, out)


tree_t* diff_SUM (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUM(_dLT, _dRT);

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_SUB (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUB(_dLT, _dRT);

    tree_fill_pt(new_tree, pt);
    
    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_MUL (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUM(_MUL(_dLT, _cRT), _MUL(_cLT, _dRT));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_DIV (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_SUB(_MUL(_dLT, _cRT), _MUL(_cLT, _dRT)), 
                                  _POW(_cRT,             _NUM(2)));

    tree_fill_pt(new_tree, pt);

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

    tree_t* temp_tree = _POW(_NUM(E_NUM), _MUL(_cRT, _LOG(_NUM(E_NUM), _cLT)));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);

    tree_dtor(temp_tree);

    return new_tree;
}

tree_t* diff_POW_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_MUL(_cRT, _POW(_cLT, _SUB(_cRT, _NUM(1)))), _dLT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_POW_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_MUL(_LOG(_NUM(E_NUM), _cLT), _cTREE), _dRT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_SIN (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_COS(NULL, _cRT), _dRT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_COS (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_MUL(_NUM(-1), _SIN(NULL, _cRT)), _dRT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_TG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dRT, _POW(_COS(NULL, _cRT), _NUM(2)));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_CTG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), _DIV(_dRT, _POW(_SIN(NULL, _cRT), _NUM(2))));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_ARCSIN (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dRT, _POW(_SUB(_NUM(1), _POW(_cRT, _NUM(2))), 
                                             _DIV(_NUM(1), _NUM(2))));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_ARCCOS (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), diff_ARCSIN(tree, NULL, out));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_ARCTG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    
    tree_t* const new_tree = _DIV(_dRT, _SUM(_NUM(1), _POW(_cRT, _NUM(2))));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_ARCCTG (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), diff_ARCTG(tree, NULL, out));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_SH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_CH(NULL, _cRT), _dRT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_CH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_SH(NULL, _cRT), _dRT);

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_TH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dRT, _POW(_CH(NULL, _cRT), _NUM(2)));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

tree_t* diff_CTH (const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), _DIV(_dRT, _POW(_SH(NULL, _cRT), _NUM(2))));

    tree_fill_pt(new_tree, pt);

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

    tree_t* temp_tree = _DIV(_LOG(_NUM(E_NUM), _cRT), _LOG(_NUM(E_NUM), _cLT));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    return new_tree;
}

tree_t* diff_LOG_lt_nrt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* temp_tree = _DIV(_NUM(1), _LOG(_NUM(E_NUM), _cLT));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    return new_tree;
}

tree_t* diff_LOG_nlt_rt_(const tree_t* const tree, tree_t* const pt, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dRT, _MUL(_LOG(_NUM(E_NUM), _cLT), _cRT));

    tree_fill_pt(new_tree, pt);

    return new_tree;
}

#undef _SUM
#undef _SUB
#undef _MUL
#undef _DIV
#undef _POW
#undef _SIN
#undef _COS
#undef _TG
#undef _CTG
#undef _ARCSIN
#undef _ARCCOS
#undef _ARCTG
#undef _ARCCTG
#undef _SH
#undef _CH
#undef _TH
#undef _CTH
#undef _LOG

#undef _NUM

#undef _cLT
#undef _cRT
#undef _dLT
#undef _dRT
#undef _cTREE
#undef _dTREE
