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
#include <stdint.h>

#include "logger/liblogger.h"
#include "tree/funcs/funcs.h"
#include "tree/operation/operation.h"
#include "utils/utils.h"

enum DescError
{
    DESC_ERROR_SUCCESS = 0,
    DESC_ERROR_FAILURE = 1
};

typedef struct DescState
{
    size_t ind;
    const char* str;
    enum DescError error;
} desc_state_t;

static enum TreeError init_str_from_file_     (const char* const input_filename, char** str, 
                                               size_t* const str_size);
static enum TreeError init_str_size_from_file_(size_t* const str_size, const int fd);

tree_t*     desc_start              (desc_state_t* const desc_state);

tree_t*     desc_sum                (desc_state_t* const desc_state);
tree_t*     desc_mul                (desc_state_t* const desc_state);
tree_t*     desc_pow                (desc_state_t* const desc_state);

tree_t*     desc_func               (desc_state_t* const desc_state);
tree_t*     desc_binary_func        (desc_state_t* const desc_state);
enum OpType desc_binary_func_names  (desc_state_t* const desc_state);
tree_t*     desc_unary_func         (desc_state_t* const desc_state);
enum OpType desc_unary_func_names   (desc_state_t* const desc_state);

tree_t*     desc_brakets            (desc_state_t* const desc_state);
tree_t*     desc_num_var            (desc_state_t* const desc_state);
tree_t*     desc_double             (desc_state_t* const desc_state);
tree_t*     desc_uint               (desc_state_t* const desc_state);
tree_t*     desc_var                (desc_state_t* const desc_state);

enum TreeError tree_read(const char* const in_name, tree_t** tree)
{
    lassert(!is_invalid_ptr(in_name), "");
    lassert(!is_invalid_ptr(tree), "");

    size_t str_size = 0;
    char*  str      = NULL;
    TREE_ERROR_HANDLE(init_str_from_file_(in_name, &str, &str_size));
    
    desc_state_t desc_state = {.ind = 0, .str = str, .error = DESC_ERROR_SUCCESS};
    *tree = desc_start(&desc_state);

    if (desc_state.error == DESC_ERROR_FAILURE)
    {
        fprintf(stderr, "Can't desc\n");
        return TREE_ERROR_DESC;
    }

    tree_fill_pt(*tree, NULL);

    TREE_VERIFY(*tree);

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

    if (*str == MAP_FAILED)
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


#define _CUR_IND desc_state->ind
#define _SHIFT   ++desc_state->ind
#define _CUR_SYM desc_state->str[desc_state->ind]

#define _RET_FAILURE                                                                                \
        fprintf(stderr, "Can't %s line: %d\n", __func__, __LINE__);                                 \
        desc_state->error = DESC_ERROR_FAILURE;                                                     \
        return NULL

#define _IS_FAILURE (desc_state->error == DESC_ERROR_FAILURE)

#define _NORMALIZE_ERROR desc_state->error = DESC_ERROR_SUCCESS

#define _CHECK_ERROR                                                                                \
        do {                                                                                        \
            lassert(!is_invalid_ptr(desc_state), "");                                               \
            if (desc_state->error == DESC_ERROR_FAILURE) { _RET_FAILURE; }                          \
        } while(0)

#define _OPERATION(name, lt, rt) \
        tree_ctor((tree_data_u){.op = OP_TYPE_##name}, NODE_TYPE_OP, NULL, lt, rt)

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

#define _NUM(numm) tree_ctor((tree_data_u){.num = numm}, NODE_TYPE_NUM, NULL, NULL, NULL)
#define _VAR(name) tree_ctor((tree_data_u){.var = name}, NODE_TYPE_VAR, NULL, NULL, NULL)

tree_t* desc_start(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    tree_t* tree = desc_sum(desc_state);
    if (_CUR_SYM != '\0')
    {
        tree_dtor(tree);
        _RET_FAILURE;
    }
    return tree;
}

tree_t* desc_sum(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    tree_t* tree = desc_mul(desc_state);
    _CHECK_ERROR;

    while(_CUR_SYM == '+' || _CUR_SYM == '-')
    {
        char op = _CUR_SYM;
        _SHIFT;

        tree_t* tree2 = desc_mul(desc_state);
        if (_IS_FAILURE)
        {
            tree_dtor(tree);
            _RET_FAILURE;
        }

        switch (op)
        {
        case '+':
            tree = _SUM(tree, tree2);
            break;
        case '-':
            tree = _SUB(tree, tree2);
            break;
        
        default:
            tree_dtor(tree);
            tree_dtor(tree2);
            _RET_FAILURE;
        }
    }

    return tree;
}

tree_t* desc_mul(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    tree_t* tree = desc_func(desc_state);
    _CHECK_ERROR;

    while(_CUR_SYM == '*' || _CUR_SYM == '/')
    {
        char op = _CUR_SYM;
        _SHIFT;

        tree_t* tree2 = desc_func(desc_state);

        if (_IS_FAILURE)
        {
            tree_dtor(tree);
            _RET_FAILURE;
        }
        
        switch (op)
        {
        case '*':
            tree = _MUL(tree, tree2);
            break;
        case '/':
            tree = _DIV(tree, tree2);
            break;
        
        default:
            tree_dtor(tree);
            tree_dtor(tree2);
            _RET_FAILURE;
        }
    }
    return tree;
}

tree_t* desc_pow(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    tree_t* tree = desc_brakets(desc_state);
    _CHECK_ERROR;

    while(_CUR_SYM == '^')
    {
        char op = _CUR_SYM;
        _SHIFT;

        tree_t* tree2 = desc_func(desc_state);
        if (_IS_FAILURE)
        {
            tree_dtor(tree);
            _RET_FAILURE;
        }
        
        switch (op)
        {
        case '^':
            tree = _POW(tree, tree2);
            break;
        
        default:
            tree_dtor(tree);
            tree_dtor(tree2);
            _RET_FAILURE;
        }
    }
    return tree;
}

tree_t* desc_func(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    size_t old_ind = _CUR_IND;

    tree_t* tree = desc_binary_func(desc_state);

    if (!_IS_FAILURE)
        return tree;
    
    _NORMALIZE_ERROR;
    _CUR_IND = old_ind;

    tree = desc_unary_func(desc_state);
    if (!_IS_FAILURE)
        return tree;
    
    _NORMALIZE_ERROR;
    _CUR_IND = old_ind;

    tree = desc_pow(desc_state);
    _CHECK_ERROR;

    return tree;
}

#define OPERATION_HANDLE(name, ...) \
        case OP_TYPE_##name: return _##name(lt, rt);

tree_t* desc_binary_func(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    enum OpType type = desc_binary_func_names(desc_state);
    _CHECK_ERROR;

    if (_CUR_SYM != '{')
    {
        _RET_FAILURE;
    }
    _SHIFT;

    tree_t* lt = desc_sum(desc_state);
    _CHECK_ERROR;

    if (_CUR_SYM != '}')
    {
        tree_dtor(lt);
        _RET_FAILURE;
    }
    _SHIFT;

    if (_CUR_SYM != '{')
    {
        tree_dtor(lt);
        _RET_FAILURE;
    }
    _SHIFT;

    tree_t* rt = desc_sum(desc_state);

    if (_IS_FAILURE)
    {
        tree_dtor(lt);
        _RET_FAILURE;
    }

    if (_CUR_SYM != '}')
    {
        tree_dtor(lt);
        tree_dtor(rt);
        _RET_FAILURE;
    }
    _SHIFT;

    switch (type)
    {
        #include "tree/operation/codegen.h"

        case MAX_OP_TYPE:
        default:
            tree_dtor(lt);
            tree_dtor(rt);
            _RET_FAILURE;
    }

    tree_dtor(lt);
    tree_dtor(rt);
    _RET_FAILURE;
}

enum OpType desc_binary_func_names(desc_state_t* const desc_state)
{                                                                                     
    lassert(!is_invalid_ptr(desc_state), "");                                               
    if (desc_state->error == DESC_ERROR_FAILURE)
    {
        fprintf(stderr, "Can't %s line: %d\n", __func__, __LINE__); 
        return MAX_OP_TYPE;                                
    }

    enum OpType type = str_to_op_type(desc_state->str + desc_state->ind);

    if (type == MAX_OP_TYPE)
    {
        fprintf(stderr, "Can't %s line: %d\n", __func__, __LINE__);                                 
        desc_state->error = DESC_ERROR_FAILURE;
        return type;
    }

    _CUR_IND += strlen(OPERATIONS[type].name);

    return type;
}  

tree_t* desc_unary_func(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    enum OpType type = desc_unary_func_names(desc_state);
    _CHECK_ERROR;

    tree_t* lt = desc_func(desc_state);
    _CHECK_ERROR;

    tree_t* rt = NULL;

    switch (type)
    {
        #include "tree/operation/codegen.h"

        case MAX_OP_TYPE:
        default:
            tree_dtor(lt);
            _RET_FAILURE;
    }

    tree_dtor(lt);
    _RET_FAILURE;
}

#undef OPERATION_HANDLE

enum OpType desc_unary_func_names(desc_state_t* const desc_state)
{
    lassert(!is_invalid_ptr(desc_state), "");                                               
    if (desc_state->error == DESC_ERROR_FAILURE)
    {
        fprintf(stderr, "Can't %s line: %d\n", __func__, __LINE__); 
        return MAX_OP_TYPE;                                
    }

    enum OpType type = str_to_op_type(desc_state->str + desc_state->ind);

    if (type == MAX_OP_TYPE)
    {
        fprintf(stderr, "Can't %s line: %d\n", __func__, __LINE__);                                 
        desc_state->error = DESC_ERROR_FAILURE;
        return type;
    }

    _CUR_IND += strlen(OPERATIONS[type].name);

    return type;
}

tree_t* desc_brakets(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    if (_CUR_SYM == '(')
    {
        _SHIFT;

        tree_t* tree = desc_sum(desc_state);
        _CHECK_ERROR;

        if (_CUR_SYM != ')')
        {
            tree_dtor(tree);
            _RET_FAILURE;
        }

        _SHIFT;

        return tree;
    }

    tree_t* tree = desc_num_var(desc_state);

    _CHECK_ERROR;
    
    return tree;
}

tree_t* desc_num_var(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    size_t old_ind = _CUR_IND;

    tree_t* tree = desc_double(desc_state);

    if (!_IS_FAILURE)
        return tree;
    
    _NORMALIZE_ERROR;
    _CUR_IND = old_ind;

    tree = desc_var(desc_state);
    _CHECK_ERROR;

    return tree;
}

tree_t* desc_double(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    if (_CUR_SYM == 'e')
    {
        _SHIFT;
        return _NUM(M_E);
    }

    bool minus = false;
    if (_CUR_SYM == '-')
    {
        _SHIFT;
        minus = true;
    }

    tree_t* tree_int = desc_uint(desc_state);

    _CHECK_ERROR;

    double frac_val = 0;
    if (_CUR_SYM == '.')
    {
        _SHIFT;
        size_t factor = 10;
        while('0' <= _CUR_SYM && _CUR_SYM <= '9')
        {
            frac_val += (_CUR_SYM - '0') / (double)factor;
            factor *= 10;
            _SHIFT;
        }
    }

    tree_t* tree = (minus ? _NUM(-tree_int->data.num - frac_val)
                          : _NUM(tree_int->data.num + frac_val));
    
    tree_dtor(tree_int);

    return tree;
}

tree_t* desc_uint(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    uint32_t val = 0;

    if (_CUR_SYM == '0')
    {
        _SHIFT;
        return _NUM(0);
    }

    if ('1' <= _CUR_SYM && _CUR_SYM <= '9')
    {
        val = (uint32_t)(_CUR_SYM - '0');
        _SHIFT;
    }
    else
    {
        _RET_FAILURE;
    }

    while ('0' <= _CUR_SYM && _CUR_SYM <= '9')
    {
        val = val * 10 + (uint32_t)(_CUR_SYM - '0');
        _SHIFT;
    }

    return _NUM((double)val);
}

tree_t* desc_var(desc_state_t* const desc_state)
{
    _CHECK_ERROR;

    if ('a' <= _CUR_SYM && _CUR_SYM <= 'z' && _CUR_SYM != 'e')
    {
        char sym = _CUR_SYM;
        _SHIFT;
        return _VAR(sym);
    }

    _RET_FAILURE;
}