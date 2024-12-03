#include <stdio.h>

#include "logger/liblogger.h"
#include "tree/funcs/funcs.h"
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
        case OP_TYPE_##_type: new_tree = diff_##_type(tree, pt, diff_var, out); break;
    

tree_t* tree_diff(const tree_t* const tree, tree_t* const pt, const char diff_var, FILE* out)
{
    lassert('a' <= diff_var && diff_var <= 'z' && diff_var != 'e', "");
    if (tree == NULL)    return NULL;

    TREE_VERIFY(tree);

    if (tree->type == NODE_TYPE_NUM || (tree->type == NODE_TYPE_VAR && tree->data.var != diff_var))
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

    TREE_VERIFY(new_tree);

    return new_tree;
}
#undef OPERATION_HANDLE

tree_t* tree_ndiff(const tree_t* const tree, const size_t n_diff, const char diff_var, FILE* out)
{
    lassert('a' <= diff_var && diff_var <= 'z' && diff_var != 'e', "");
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    tree_t* new_tree = tree_copy(tree, NULL);

    if (!new_tree)
    {
        fprintf(stderr, "Can't tree copy\n");
        return NULL;
    }

    for (size_t count_diff = 1; count_diff <= n_diff; ++count_diff)
    {
        tree_t* temp_tree = new_tree;

        new_tree = tree_diff(new_tree, NULL, diff_var, out);

        tree_dtor(temp_tree);

        if (!new_tree)
        {
            fprintf(stderr, "Can't diff new tree\n");
            return NULL;
        }

        if (tree_simplify(&new_tree, out))
        {
            fprintf(stderr, "Can't tree simplify\n");
            tree_dtor(new_tree);
            return NULL;
        }
    }

    return new_tree;
}

void tree_var_to_val_(tree_t* const tree, const char var, const operand_t val);

tree_t* tree_val_in_point (const tree_t* const tree, const char diff_var, const operand_t point, 
                           FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    lassert('a' <= diff_var && diff_var <= 'z' && diff_var != 'e', "");

    tree_t* tree_val = tree_copy(tree, NULL);

    tree_var_to_val_(tree_val, diff_var, point);

    if (tree_simplify(&tree_val, out))
    {
        fprintf(stderr, "Can't simplify\n");
        tree_dtor(tree_val);
        return NULL;
    }

    return tree_val;
}

void tree_var_to_val_(tree_t* const tree, const char var, const operand_t val)
{
    if (tree == NULL) return;

    TREE_VERIFY(tree);
    lassert('a' <= var && var <= 'z' && var != 'e', "");

    if (tree->type == NODE_TYPE_VAR && tree->data.var == var)
    {
        tree->type = NODE_TYPE_NUM;
        tree->data.num = val;
    }

    tree_var_to_val_(tree->lt, var, val);
    tree_var_to_val_(tree->rt, var, val);
}

size_t factorial_(const size_t num);

#define _OPERATION(name, lt, rt) \
        tree_ctor((tree_data_u){.op = OP_TYPE_##name}, NODE_TYPE_OP, NULL, lt, rt)

#define _SUM(lt, rt)    _OPERATION(SUM,     lt, rt)
#define _SUB(lt, rt)    _OPERATION(SUB,     lt, rt)
#define _MUL(lt, rt)    _OPERATION(MUL,     lt, rt)
#define _DIV(lt, rt)    _OPERATION(DIV,     lt, rt)
#define _POW(lt, rt)    _OPERATION(POW,     lt, rt)

#define _NUM(numm) tree_ctor((tree_data_u){.num = (operand_t)numm}, NODE_TYPE_NUM, NULL, NULL, NULL)
#define _VAR(name) tree_ctor((tree_data_u){.var = name}, NODE_TYPE_VAR, NULL, NULL, NULL)

tree_t* tree_taylor_monom(const tree_t* const tree, const size_t order, const char diff_var, 
                          const operand_t point, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    lassert('a' <= diff_var && diff_var <= 'z' && diff_var != 'e', "");

    tree_t* ndiff = tree_ndiff(tree, order, diff_var, out);

    tree_t* ans = _MUL(_DIV(tree_val_in_point(ndiff, diff_var, point, out), _NUM(factorial_(order))),
                       _POW(_SUB(_VAR(diff_var), _NUM(point)), _NUM(order)));
    
    tree_fill_pt(ans, NULL);

    tree_dtor(ndiff);

    if (tree_simplify(&ans, out))
    {
        fprintf(stderr, "Can't simplify monom\n");
        tree_dtor(ans);
        return NULL;
    }

    return ans;
}

size_t factorial_(const size_t num)
{
    size_t ans = 1;

    for (size_t i = 2; i <= num; ans *= i++);

    return ans;
}

tree_t* tree_taylor_polynom(const tree_t* const tree, const size_t order, const char diff_var, 
                            const operand_t point, FILE* out)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");
    lassert('a' <= diff_var && diff_var <= 'z' && diff_var != 'e', "");

    tree_t* ans = tree_taylor_monom(tree, 0, diff_var, point, out);
    
    for (size_t count_diff = 1; count_diff <= order; ++count_diff)
    {
        ans = _SUM(ans, tree_taylor_monom(tree, count_diff, diff_var, point, out));
        tree_fill_pt(ans, NULL);
    }

    return ans;
}