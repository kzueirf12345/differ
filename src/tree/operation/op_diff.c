#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "tree/operation/op_diff.h"
#include "tree/funcs/funcs.h"
#include "logger/liblogger.h"

#define _OPERATION(name, lt, rt) \
        tree_ctor((tree_data_u){.op = OP_TYPE_##name}, NODE_TYPE_OP, NULL, lt, rt)

// FIXME codegen
#define _SUM(lt, rt)    _OPERATION(SUM,     lt, rt)
#define _SUB(lt, rt)    _OPERATION(SUB,     lt, rt)
#define _MUL(lt, rt)    _OPERATION(MUL,     lt, rt)
#define _DIV(lt, rt)    _OPERATION(DIV,     lt, rt)
#define _POW(lt, rt)    _OPERATION(POW,     lt, rt)
#define _SIN(lt, rt)    _OPERATION(SIN,     lt, rt)
#define _COS(lt, rt)    _OPERATION(COS,     lt, rt)
#define _TG(lt, rt)     _OPERATION(TG,      lt, rt)
#define _CTG(lt, rt)    _OPERATION(CTG,     lt, rt)
#define _ARCSIN(lt, rt) _OPERATION(ARCSIN,  lt, rt)
#define _ARCCOS(lt, rt) _OPERATION(ARCCOS,  lt, rt)
#define _ARCTG(lt, rt)  _OPERATION(ARCTG,   lt, rt)
#define _ARCCTG(lt, rt) _OPERATION(ARCCTG,  lt, rt)
#define _SH(lt, rt)     _OPERATION(SH,      lt, rt)
#define _CH(lt, rt)     _OPERATION(CH,      lt, rt)
#define _TH(lt, rt)     _OPERATION(TH,      lt, rt)
#define _CTH(lt, rt)    _OPERATION(CTH,     lt, rt)
#define _LOG(lt, rt)    _OPERATION(LOG,     lt, rt)
#define _LN(lt, rt)     _OPERATION(LN,      lt, rt)

#define _NUM(numm) tree_ctor((tree_data_u){.num = numm}, NODE_TYPE_NUM, NULL, NULL, NULL)

#define _cLT    tree_copy(tree->lt, NULL)
#define _cRT    tree_copy(tree->rt, NULL)
#define _cTREE  tree_copy(tree,     NULL)
#define _dLT    tree_diff(tree->lt, NULL, diff_var, out)
#define _dRT    tree_diff(tree->rt, NULL, diff_var, out)
#define _dTREE  tree_diff(tree,     NULL, diff_var, out)


tree_t* diff_SUM (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUM(_dLT, _dRT);

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_SUB (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUB(_dLT, _dRT);

    tree_fill_pt(new_tree, pt);
    
    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_MUL (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _SUM(_MUL(_dLT, _cRT), _MUL(_dRT, _cLT));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_DIV (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_SUB(_MUL(_dLT, _cRT), _MUL(_dRT, _cLT)), 
                                  _POW(_cRT,             _NUM(2)));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_POW_lt_rt_ (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);
tree_t* diff_POW_lt_nrt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);
tree_t* diff_POW_nlt_rt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);

tree_t* diff_POW (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    const bool lt_include_var = tree_include_var(tree->lt);
    const bool rt_include_var = tree_include_var(tree->rt);

    if (!lt_include_var && !rt_include_var) { 
        return tree_ctor((tree_data_u){.num = 0}, NODE_TYPE_NUM, pt, NULL, NULL); 
    }
    if ( lt_include_var &&  rt_include_var) { return diff_POW_lt_rt_ (tree, pt, diff_var, out);    }
    if ( lt_include_var && !rt_include_var) { return diff_POW_lt_nrt_(tree, pt, diff_var, out);    }
                                              return diff_POW_nlt_rt_(tree, pt, diff_var, out);
}

tree_t* diff_POW_lt_rt_ (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    (void)diff_var;

    tree_t* temp_tree = _POW(_NUM(M_E), _MUL(_cRT, _LN(_cLT, NULL)));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, diff_var, out);

    tree_dtor(temp_tree);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_POW_lt_nrt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_dLT, _MUL(_cRT, _POW(_cLT, _SUB(_cRT, _NUM(1)))));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_POW_nlt_rt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_dRT, _MUL(_LN(_cLT, NULL), _cTREE));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_SIN (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_dLT, _COS(_cLT, NULL));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_COS (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), _MUL(_dLT, _SIN(_cLT, NULL)));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_TG (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dLT, _POW(_COS(_cLT, NULL), _NUM(2)));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_CTG (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), _DIV(_dLT, _POW(_SIN(_cLT, NULL), _NUM(2))));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_ARCSIN (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dLT, _POW(_SUB(_NUM(1), _POW(_cLT, _NUM(2))), 
                                             _DIV(_NUM(1), _NUM(2))));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_ARCCOS (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    (void)diff_var;

    tree_t* const new_tree = _MUL(_NUM(-1), diff_ARCSIN(tree, NULL, diff_var, out));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_ARCTG (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    
    tree_t* const new_tree = _DIV(_dLT, _SUM(_NUM(1), _POW(_cLT, _NUM(2))));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_ARCCTG (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    (void)diff_var;

    tree_t* const new_tree = _MUL(_NUM(-1), diff_ARCTG(tree, NULL, diff_var, out));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_SH (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_CH(_cLT, NULL), _dLT);

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_CH (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_SH(_cLT, NULL), _dLT);

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_TH (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dLT, _POW(_CH(_cLT, NULL), _NUM(2)));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_CTH (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _MUL(_NUM(-1), _DIV(_dLT, _POW(_SH(_cLT, NULL), _NUM(2))));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_LOG_lt_rt_ (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);
tree_t* diff_LOG_lt_nrt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);
tree_t* diff_LOG_nlt_rt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out);

tree_t* diff_LOG (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    const bool lt_include_var = tree_include_var(tree->lt);
    const bool rt_include_var = tree_include_var(tree->rt);

    if (!lt_include_var && !rt_include_var) { 
        return tree_ctor((tree_data_u){.num = 0}, NODE_TYPE_NUM, pt, NULL, NULL);
    }
    if ( lt_include_var &&  rt_include_var) { return diff_LOG_lt_rt_ (tree, pt, diff_var, out);    }
    if ( lt_include_var && !rt_include_var) { return diff_LOG_lt_nrt_(tree, pt, diff_var, out);    }
                                              return diff_LOG_nlt_rt_(tree, pt, diff_var, out);
}

tree_t* diff_LOG_lt_rt_ (const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    (void)diff_var;

    tree_t* temp_tree = _DIV(_LN(_cRT, NULL), _LN(_cLT, NULL));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, diff_var, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_LOG_lt_nrt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    (void)diff_var;

    tree_t* temp_tree = _DIV(_NUM(1), _LN(_cLT, NULL));

    tree_fill_pt(temp_tree, NULL);

    tree_t* const new_tree = tree_diff(temp_tree, pt, diff_var, out);
        
    tree_dtor(temp_tree); temp_tree = NULL;

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_LOG_nlt_rt_(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dRT, _MUL(_LN(_cLT, NULL), _cRT));

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

    return new_tree;
}

tree_t* diff_LN(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* const new_tree = _DIV(_dLT, _cLT);

    tree_fill_pt(new_tree, pt);

    TREE_VERIFY(new_tree);

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
