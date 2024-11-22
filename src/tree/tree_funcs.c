#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

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

static enum TreeError tree_print_data_(FILE* out, const int data, enum NodeType type);

enum TreeError tree_print_preorder(FILE* out, const tree_t* const tree)
{
    if (!tree) return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    if (fputs("(", out) < 0)
    {
        perror("Can't fprintf (");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(tree_print_data_(out, tree->data, tree->type));

    TREE_ERROR_HANDLE(tree_print_preorder(out, tree->lt));
    TREE_ERROR_HANDLE(tree_print_preorder(out, tree->rt));

    if (fputs(")", out) < 0)
    {
        perror("Can't fprintf )");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_print_inorder(FILE* out, const tree_t* const tree)
{
    if (!tree) return TREE_ERROR_SUCCESS;

    TREE_VERIFY(tree);
    lassert(!is_invalid_ptr(out), "");

    if (fputs("(", out) < 0)
    {
        perror("Can't fprintf (");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(tree_print_inorder(out, tree->lt));

    TREE_ERROR_HANDLE(tree_print_data_(out, tree->data, tree->type));

    TREE_ERROR_HANDLE(tree_print_inorder(out, tree->rt));

    if (fputs(")", out) < 0)
    {
        perror("Can't fprintf )");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    return TREE_ERROR_SUCCESS;
}

static enum TreeError tree_print_data_(FILE* out, const int data, enum NodeType type)
{
    lassert(!is_invalid_ptr(out), "");

    switch (type)
    {
    case NODE_TYPE_NUM:
        if (fprintf(out, "%d", data) <= 0)
        {
            perror("Can't fprintf data");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    case NODE_TYPE_OP:
        if (fputs(op_type_to_str((enum OpType)data), out) <= 0)
        {
            perror("Can't fputs data");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    case NODE_TYPE_VAR: //TODO more vars
        if (fputs("x", out) <= 0)
        {
            perror("Can't fputs x");
            return TREE_ERROR_STANDARD_ERRNO;
        }
        break;
    
    default:
        return TREE_ERROR_UNKNOWN;
    }

    return TREE_ERROR_SUCCESS;
}


static enum TreeError init_str_from_file_(const char* const input_filename, 
                                          char** str, size_t* const str_size);
static enum TreeError init_str_size_from_file_(size_t* const str_size, const int fd);


enum TreeError tree_read_preorder_from_str_(char** cur_str, tree_t** tree, tree_t* const pt);

enum TreeError tree_read_preorder(const char* const in_name, tree_t** tree)
{
    lassert(!is_invalid_ptr(in_name), "");
    lassert(!is_invalid_ptr(tree), "");

    size_t str_size = 0;
    char* str = NULL;
    TREE_ERROR_HANDLE(init_str_from_file_(in_name, &str, &str_size));
    
    TREE_ERROR_HANDLE(tree_read_preorder_from_str_(&str, tree, NULL));

    TREE_VERIFY(*tree);
    return TREE_ERROR_SUCCESS;
}

static size_t  size_counter_ = 0;

enum TreeError tree_read_preorder_from_str_(char** cur_str, tree_t** tree, tree_t* const pt)
{
    lassert(!is_invalid_ptr(cur_str), "");
    lassert(!is_invalid_ptr(tree), "");

    if (sscanf(*cur_str, "(") < 0)
    {
        perror("Can't sscanf (");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    ++*cur_str;

    int num_data = 0;
    char str_data[DATA_STR_MAX_SIZE] = {};
    if (sscanf(*cur_str, "%d%*1[()]", &num_data) == 1)
    {
        *tree = tree_ctor(num_data, NODE_TYPE_NUM, pt, NULL, NULL);
    }
    else if (sscanf(*cur_str, "%[^()]", str_data) == 1)
    {
        // fprintf(stderr, "str_data: %s\n", str_data);
        if (str_data[1] == '\0' && isalpha(str_data[0]))
        {
            *tree = tree_ctor((int)str_data[0], NODE_TYPE_VAR, pt, NULL, NULL);
        }
        else
        {
            *tree = tree_ctor(str_to_op_type(str_data), NODE_TYPE_OP, pt, NULL, NULL);
        }
    }
    else
    {
        perror("Can't sscanf data");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (!(*cur_str = strpbrk(*cur_str, "()")))
    {
        perror("Can't strpbrk ()");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    size_t cur_size = 0;
    size_counter_   = 0;

    if (**cur_str == '(')
    {
        TREE_ERROR_HANDLE(tree_read_preorder_from_str_(cur_str, &(*tree)->lt, *tree));

        cur_size = size_counter_;
                   size_counter_ = 0;

        TREE_ERROR_HANDLE(tree_read_preorder_from_str_(cur_str, &(*tree)->rt, *tree));
    }

    cur_size += size_counter_ + 1;
                size_counter_ = cur_size;

    (*tree)->size =cur_size;

    ++*cur_str;

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_read_inorder_from_str_(char** cur_str, tree_t** tree, tree_t* const pt);

enum TreeError tree_read_inorder(const char* const in_name, tree_t** tree)
{
    lassert(!is_invalid_ptr(in_name), "");
    lassert(!is_invalid_ptr(tree), "");

    size_t str_size = 0;
    char* str = NULL;
    TREE_ERROR_HANDLE(init_str_from_file_(in_name, &str, &str_size));
    
    TREE_ERROR_HANDLE(tree_read_inorder_from_str_(&str, tree, NULL));

    TREE_VERIFY(*tree);
    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_read_inorder_from_str_(char** cur_str, tree_t** tree, tree_t* const pt)
{
    lassert(!is_invalid_ptr(cur_str), "");
    lassert(!is_invalid_ptr(tree), "");

    if (sscanf(*cur_str, "(") < 0)
    {
        perror("Can't sscanf (");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    ++*cur_str;

    *tree = tree_ctor(0, NODE_TYPE_NUM, NULL, NULL, NULL);

    size_t cur_size = 0;
    size_counter_   = 0;

    if (**cur_str == '(')
    {
        TREE_ERROR_HANDLE(tree_read_inorder_from_str_(cur_str, &(*tree)->lt, *tree));
    }

    int num_data = 0;
    char str_data[DATA_STR_MAX_SIZE] = {};
    if (sscanf(*cur_str, "%d%*1[()]", &num_data) == 1)
    {
        (*tree)->data = num_data;
        (*tree)->type = NODE_TYPE_NUM;
        (*tree)->pt   = (pt ? pt : *tree);
    }
    else if (sscanf(*cur_str, "%[^()]", str_data) == 1)
    {
        // fprintf(stderr, "str_data: %s\n", str_data);
        if (str_data[1] == '\0' && isalpha(str_data[0]))
        {
            (*tree)->data = (int)str_data[0];
            (*tree)->type = NODE_TYPE_VAR;
            (*tree)->pt   = (pt ? pt : *tree);
        }
        else
        {
            (*tree)->data = str_to_op_type(str_data);
            (*tree)->type = NODE_TYPE_OP;
            (*tree)->pt   = (pt ? pt : *tree);
        }
    }
    else
    {
        perror("Can't sscanf data");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (!(*cur_str = strpbrk(*cur_str, "()")))
    {
        perror("Can't strpbrk ()");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    cur_size = size_counter_;
               size_counter_ = 0;

    if (**cur_str == '(')
    {
        TREE_ERROR_HANDLE(tree_read_inorder_from_str_(cur_str, &(*tree)->rt, *tree));
    }

    cur_size += size_counter_ + 1;
                size_counter_ = cur_size;

    (*tree)->size =cur_size;

    ++*cur_str;

    return TREE_ERROR_SUCCESS;
}

static enum TreeError init_str_from_file_(const char* const input_filename, 
                                          char** str, size_t* const str_size)
{
    lassert(!is_invalid_ptr(input_filename), "");
    lassert(!is_invalid_ptr(str), "");
    lassert(str_size, "");

    int fd = open(input_filename, O_RDWR);
    if (fd == -1)
    {
        perror("Can't fopen input file");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(init_str_size_from_file_(str_size, fd));

    *str = mmap(NULL, *str_size, PROT_READ, MAP_PRIVATE, fd, 0);


    if (*str  == MAP_FAILED)
    {
        perror("Can't mmap");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (close(fd))
    {
        perror("Can't fclose input file");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    IF_DEBUG(fd = -1;)

    return TREE_ERROR_SUCCESS;
}

static enum TreeError init_str_size_from_file_ (size_t* const str_size, const int fd)
{
    lassert(str_size, "");
    lassert(fd != -1, "");

    struct stat fd_stat = {};

    if (fstat(fd, &fd_stat))
    {
        perror("Can't fstat");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    *str_size = (size_t)fd_stat.st_size + 1;

    return TREE_ERROR_SUCCESS;
}
