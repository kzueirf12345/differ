#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include "tree/funcs/funcs.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree/operation/operation.h"
#include "gnuplot/libgnuplot.h"

static enum TreeError tree_print_data_(FILE* out, const tree_data_u data, enum NodeType type);

bool add_braket(const tree_t* const tree);

enum TreeError tree_print_tex_recursive_(FILE* out, const tree_t* const tree);

enum TreeError tree_print_tex (FILE* out, const tree_t* const tree)
{
    if (!tree) return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    fprintf(out, "\n$$\n");
    
    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree));

    fprintf(out, "\n$$\n");

    return TREE_ERROR_SUCCESS;
}
enum TreeError tree_print_tex_recursive_(FILE* out, const tree_t* const tree)
{
    if (!tree) return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    const bool do_add_braket = add_braket(tree);
    if (do_add_braket && fputs("\\left(", out) < 0)
    {
        perror("Can't fprintf (");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (tree->type == NODE_TYPE_OP && OPERATIONS[tree->data.op].notation == OP_NOTATION_LEFT)
    {
        if (fprintf(out, "%s{", OPERATIONS[tree->data.op].tex_name) <= 0)
        {
            perror("Can't fprintf left notation op");
            return TREE_ERROR_STANDARD_ERRNO;
        }
    }

    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree->lt));

    if (tree->type == NODE_TYPE_OP && OPERATIONS[tree->data.op].notation == OP_NOTATION_LEFT)
    {
        if (fputs("}", out) < 0)
        {
            perror("Can't fputs } left nontation op 1");
            return TREE_ERROR_STANDARD_ERRNO;
        }
    }

    if (tree->type != NODE_TYPE_OP || OPERATIONS[tree->data.op].notation != OP_NOTATION_LEFT)
    {
        TREE_ERROR_HANDLE(tree_print_data_(out, tree->data, tree->type));
    }

    if (tree->type == NODE_TYPE_OP && OPERATIONS[tree->data.op].count_operands == 2)
    {
        if (fputs("{", out) < 0)
        {
            perror("Can't fputs { left nontation op 2");
            return TREE_ERROR_STANDARD_ERRNO;
        }
    }

    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree->rt));

    if (tree->type == NODE_TYPE_OP && OPERATIONS[tree->data.op].count_operands == 2)
    {
        if (fputs("}", out) < 0)
        {
            perror("Can't fputs } left nontation op 2");
            return TREE_ERROR_STANDARD_ERRNO;
        }
    } 

    if (do_add_braket && fputs("\\right)", out) < 0)
    {
        perror("Can't fprintf )");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    return TREE_ERROR_SUCCESS;
}

bool add_braket(const tree_t* const tree)
{
    TREE_VERIFY(tree);

    if (tree->pt == tree)
        return false;

    if (tree->type != NODE_TYPE_OP)
    {
        if (tree->type == NODE_TYPE_NUM && tree->data.num < 0)
            return true;
        
        return false;
    }

    switch (tree->pt->data.op)
    {
    case OP_TYPE_SIN:
    case OP_TYPE_COS:
    case OP_TYPE_TG:
    case OP_TYPE_CTG:
    case OP_TYPE_ARCSIN:
    case OP_TYPE_ARCCOS:
    case OP_TYPE_ARCTG:
    case OP_TYPE_ARCCTG:
    case OP_TYPE_SH:
    case OP_TYPE_CH:
    case OP_TYPE_TH:
    case OP_TYPE_CTH:
    case OP_TYPE_LN:
    {
        return (tree->type == NODE_TYPE_OP) && (tree->data.op != OP_TYPE_DIV);
    }

    case OP_TYPE_LOG:
    {
        return tree->pt->rt == tree;
    }

    case OP_TYPE_SUB:
    {
        return (tree->pt->rt == tree) && (tree->type == NODE_TYPE_OP) 
            && ((tree->data.op == OP_TYPE_SUM) 
            ||  (tree->data.op == OP_TYPE_SUB));
    }

    case OP_TYPE_SUM:
    case OP_TYPE_DIV:
        return false;

    case OP_TYPE_MUL:
    {
        return (tree->type == NODE_TYPE_OP) 
            && ((tree->data.op == OP_TYPE_SUM) 
            ||  (tree->data.op == OP_TYPE_SUB));
    }

    case OP_TYPE_POW:
    {
        return (tree->pt->lt == tree) && (tree->type == NODE_TYPE_OP);
    }

    case MAX_OP_TYPE:
        return true;
    
    default:
        return true;
    }

    return true;
}

static enum TreeError tree_print_data_(FILE* out, const tree_data_u data, enum NodeType type)
{
    lassert(!is_invalid_ptr(out), "");

    switch (type)
    {
    case NODE_TYPE_NUM:
        if (fabs(data.num - M_E) < __DBL_EPSILON__)
        {
            if (fprintf(out, "e") <= 0)
            {
                perror("Can't fprintf data e");
                return TREE_ERROR_STANDARD_ERRNO;
            }
        }
        else if (fprintf(out, "%g", data.num) <= 0)
        {
            perror("Can't fprintf data");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    case NODE_TYPE_OP:
        if (fputs(OPERATIONS[data.op].tex_name, out) <= 0)
        {
            perror("Can't fputs data");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    case NODE_TYPE_VAR:
        if (fputc(data.var, out) <= 0)
        {
            perror("Can't fputs var");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    
    default:
        return TREE_ERROR_UNKNOWN;
    }

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_create_graphic(const tree_t* const tree, const char var, 
                                   const char* const filename, char* const name,
                                   const size_t npoints)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(filename), "");
    lassert(!is_invalid_ptr(name), "");
    lassert('a' <= var && var <= 'z' && var != 'e', "");
    lassert(npoints, "");

    gnuplot_ctrl* handler = gnuplot_init();

    double* xarr = calloc(npoints, sizeof(double));
    if (!xarr)
    {
        perror("Can't calloc xarr");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    double* yarr = calloc(npoints, sizeof(double));
    if (!yarr)
    {
        perror("Can't calloc xarr");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    gnuplot_resetplot(handler);

    gnuplot_cmd(handler, "set terminal png");
    gnuplot_cmd(handler, "set output '%s.png'", filename);

    gnuplot_setstyle     (handler, "lines");
    gnuplot_set_axislabel(handler, "x", "var label");
    gnuplot_set_axislabel(handler, "y", "foo label");

    for (size_t point = 0; point < npoints; ++point)
    {
        xarr[point] = (double)point - npoints/2;
        tree_t* tree_val = tree_val_in_point(tree, var, (operand_t)point - npoints/2, stdout); //FIXME out fix
        if (!tree_val || tree_val->type != NODE_TYPE_NUM)
        {
            fprintf(stderr, "Can't calculate val in point %zu\n", point);
            if (tree_val) tree_dtor(tree_val);
            return TREE_ERROR_NUM_DATA_INVALID;
        }
        yarr[point] = tree_val->data.num / 10000; 
        tree_dtor(tree_val);
    }

    gnuplot_plot_coordinates(handler, xarr, yarr, (int)npoints, name);

    gnuplot_close(handler);
    free(xarr); xarr = NULL;
    free(yarr); yarr = NULL;

    return TREE_ERROR_SUCCESS;
}