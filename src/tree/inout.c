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

//==================================================================================================

static enum TreeError init_str_from_file_     (const char* const input_filename, char** str, 
                                               size_t* const str_size);
static enum TreeError init_str_size_from_file_(size_t* const str_size, const int fd);

static size_t  size_counter_ = 0;
enum TreeError tree_read_preorder_from_str_(char** cur_str, tree_t** tree, tree_t* const pt);
enum TreeError tree_read_inorder_from_str_ (char** cur_str, tree_t** tree, tree_t* const pt);


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

static enum TreeError init_str_from_file_(const char* const input_filename, char** str, 
                                          size_t* const str_size)
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