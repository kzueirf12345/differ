#ifndef DIFFER_SRC_TREE_VERIFICATION_DUMB_H
#define DIFFER_SRC_TREE_VERIFICATION_DUMB_H

#include <assert.h>

#include "tree/structs.h"
#include "utils/utils.h"

enum TreeDumbError
{
    TREE_DUMB_ERROR_SUCCESS = 0,
    TREE_DUMB_ERROR_FAILURE = 1
};
static_assert(TREE_DUMB_ERROR_SUCCESS == 0);

const char* tree_dumb_strerror(const enum TreeDumbError error);

#define TREE_DUMB_ERROR_HANDLE(call_func, ...)                                                      \
    do {                                                                                            \
        enum TreeDumbError error_handler = call_func;                                               \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            tree_dumb_strerror(error_handler));                                     \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

enum TreeDumbError tree_dumb_ctor(void);
enum TreeDumbError tree_dumb_dtor(void);

enum TreeDumbError tree_dumb_set_out_file(char* const filename);

void tree_dumb (const tree_t* const tree);

#endif /*DIFFER_SRC_TREE_VERIFICATION_DUMB_H*/