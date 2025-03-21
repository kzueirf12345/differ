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

    fprintf(out, "\n\\[\n");
    
    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree));

    fprintf(out, "\n\\]\n");

    return TREE_ERROR_SUCCESS;
}

#define _IS_OP     (tree->type == NODE_TYPE_OP)
#define _IS_UNARY  (_IS_OP && OPERATIONS[tree->data.op].count_operands == 1)
#define _IS_BINARY (_IS_OP && OPERATIONS[tree->data.op].count_operands == 2)
#define _IS_PREF   (_IS_OP && OPERATIONS[tree->data.op].notation == OP_NOTATION_LEFT)
#define _IS_INF    (_IS_OP && OPERATIONS[tree->data.op].notation == OP_NOTATION_MIDDLE)
#define _IS_POST   (_IS_OP && OPERATIONS[tree->data.op].notation == OP_NOTATION_RIGHT)

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

    if (_IS_PREF && fprintf(out, "%s{", OPERATIONS[tree->data.op].tex_name) <= 0)
    {
        perror("Can't fprintf left notation op");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree->lt));

    if (_IS_PREF && fputs("}", out) < 0)
    {
        perror("Can't fputs } left nontation op 1");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (!_IS_OP || _IS_INF)
    {
        TREE_ERROR_HANDLE(tree_print_data_(out, tree->data, tree->type));
    }

    if (_IS_BINARY && fputs("{", out) < 0)
    {
        perror("Can't fputs { left nontation op 2");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(tree_print_tex_recursive_(out, tree->rt));

    if (_IS_BINARY && fputs("}", out) < 0)
    {
        perror("Can't fputs } left nontation op 2");
        return TREE_ERROR_STANDARD_ERRNO;
    } 

    if (do_add_braket && fputs("\\right)", out) < 0)
    {
        perror("Can't fprintf )");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    return TREE_ERROR_SUCCESS;
}

static enum TreeError tree_strncat_data_(char* str, const size_t str_size, const tree_data_u data, 
                                         enum NodeType type);

enum TreeError tree_to_str(const tree_t* const tree, char* const str, const size_t str_size)
{
    if (!tree) return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(str), "");

    if (!strncat(str, "(", str_size))
    {
        perror("Can't strncat (");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (_IS_PREF && tree->data.op != OP_TYPE_DIV)
    {
        if (!strncat(str, OPERATIONS[tree->data.op].graph_name, str_size))
        {
            perror("Can't strncat left notation op");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        if (tree->data.op == OP_TYPE_LOG && !strncat(str, "_", str_size))
        {
            perror("Can't strncat left notation op");
            return TREE_ERROR_STANDARD_ERRNO;
        }
    }

    TREE_ERROR_HANDLE(tree_to_str(tree->lt, str, str_size));

    if (!_IS_OP || _IS_INF || tree->data.op == OP_TYPE_DIV)
    {
        TREE_ERROR_HANDLE(tree_strncat_data_(str, str_size, tree->data, tree->type));
    }

    TREE_ERROR_HANDLE(tree_to_str(tree->rt, str, str_size));

    if (!strncat(str, ")", str_size))
    {
        perror("Can't strncat )");
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

static enum TreeError tree_strncat_data_(char* str, const size_t str_size, const tree_data_u data, 
                                         enum NodeType type)
{
    lassert(!is_invalid_ptr(str), "");

    switch (type)
    {
    case NODE_TYPE_NUM:
        char temp[DATA_STR_MAX_SIZE] = {};
        if (snprintf(temp, DATA_STR_MAX_SIZE, "%f", data.num) < 0)
        {
            perror("Can't snprintf data");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        *strchr(temp, ',') = '.';
        if (!strncat(str, temp, str_size))
        {
            perror("Can't strncat data e");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;

    case NODE_TYPE_OP:
        if (!strncat(str, (data.op == OP_TYPE_POW ? "**" : OPERATIONS[data.op].graph_name), str_size))
        {
            perror("Can't strncat op");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;

    case NODE_TYPE_VAR:
        const size_t size = strlen(str);
        str[size] = data.var;
        str[size+1] = '\0';
        break;
    
    default:
        return TREE_ERROR_UNKNOWN;
    }

    return TREE_ERROR_SUCCESS;
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


#define PLOT_EQ_SIZE 4096
enum TreeError tree_create_graphic(const tree_t* const tree, const tree_t* const subtree,
                                   const char var, const char* const filename, char* const name)
{
    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(filename), "");
    lassert(!is_invalid_ptr(name), "");
    lassert('a' <= var && var <= 'z' && var != 'e', "");

    gnuplot_ctrl* handler = gnuplot_init();

    gnuplot_resetplot(handler);

    gnuplot_cmd(handler, "set terminal png");
    gnuplot_cmd(handler, "set output '%s.png'", filename);
    gnuplot_cmd(handler, "set xrange [-1:1]");
    // gnuplot_cmd(handler, "set yrange [-30:30]");

    gnuplot_setstyle     (handler, "lines");
    gnuplot_set_axislabel(handler, "x", "var label");
    gnuplot_set_axislabel(handler, "y", "foo label");

    // tree_dumb(tree);

    char plot_eq[PLOT_EQ_SIZE] = {};
    TREE_ERROR_HANDLE(tree_to_str(tree, plot_eq, PLOT_EQ_SIZE));
    char sub_plot_eq[PLOT_EQ_SIZE] = {};
    TREE_ERROR_HANDLE(tree_to_str(subtree, sub_plot_eq, PLOT_EQ_SIZE));

    

    gnuplot_cmd(handler, "plot %s, %s", plot_eq, sub_plot_eq);
    // fprintf(stderr, "plot %s, %s\n", plot_eq, sub_plot_eq);

    gnuplot_close(handler);

    return TREE_ERROR_SUCCESS;
}