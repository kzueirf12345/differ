#ifndef DIFFER_SRC_TREE_VERIFICATION_VERIFICATION_H
#define DIFFER_SRC_TREE_VERIFICATION_VERIFICATION_H

#include <assert.h>
#include <stdbool.h>

#include "tree/structs.h"
#include "tree/verification/dumb.h"

enum TreeError
{
    TREE_ERROR_SUCCESS              = 0,
    TREE_ERROR_STANDARD_ERRNO       = 1,
    TREE_ERROR_TREE_IS_NULL         = 2,
    TREE_ERROR_TREE_IS_INVALID      = 3,
    TREE_ERROR_DATA_LESS_ZERO       = 4,
    TREE_ERROR_PT_IS_NULL           = 6,
    TREE_ERROR_PT_IS_INVALID        = 7,
    TREE_ERROR_LT_PT_INCORRECT      = 8,
    TREE_ERROR_RT_PT_INCORRECT      = 9,
    TREE_ERROR_SIZE_GREATER         = 10,
    TREE_ERROR_SIZE_LESSER          = 11,

    TREE_ERROR_UNKNOWN              = 20,
};
static_assert(TREE_ERROR_SUCCESS == 0);

const char* tree_strerror(const enum TreeError error);

#define TREE_ERROR_HANDLE(call_func, ...)                                                           \
    do {                                                                                            \
        enum TreeError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            tree_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)


#ifndef NDEBUG

enum TreeError tree_verify_NOT_USE(const tree_t* const tree);

#define TREE_VERIFY(tree)                                                                           \
        do {                                                                                        \
            const enum TreeError error = tree_verify_NOT_USE(tree);                                 \
            if (error)                                                                              \
            {                                                                                       \
                tree_dumb(tree);                                                                    \
                tree_dumb_dtor();                                                                   \
                lassert(false, "Tree error: %s", tree_strerror(error));                             \
            }                                                                                       \
        } while(0)

#else /*NDEBUG*/

#define TREE_VERIFY(tree) do {} while(0)

#endif /*NDEBUG*/

#endif /* DIFFER_SRC_TREE_VERIFICATION_VERIFICATION_H */