#include "tree/verification/verification.h"
#include "utils/utils.h"
#include "logger/liblogger.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* tree_strerror(const enum TreeError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(TREE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(TREE_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(TREE_ERROR_TREE_IS_NULL);
        CASE_ENUM_TO_STRING_(TREE_ERROR_TREE_IS_INVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_DATA_LESS_ZERO);
        CASE_ENUM_TO_STRING_(TREE_ERROR_UNKNOWN);
        CASE_ENUM_TO_STRING_(TREE_ERROR_PT_IS_NULL);
        CASE_ENUM_TO_STRING_(TREE_ERROR_PT_IS_INVALID);
        CASE_ENUM_TO_STRING_(TREE_ERROR_LT_PT_INCORRECT);
        CASE_ENUM_TO_STRING_(TREE_ERROR_RT_PT_INCORRECT);
        CASE_ENUM_TO_STRING_(TREE_ERROR_SIZE_GREATER);
        CASE_ENUM_TO_STRING_(TREE_ERROR_SIZE_LESSER);

        
        default:
            return "UNKNOWN_TREE_ERROR";
    }
    return "UNKNOWN_TREE_ERROR";
}
#undef CASE_ENUM_TO_STRING_

#ifndef NDEBUG

static size_t        size_counter_ = 0;
static const tree_t* start_node_   = NULL;

enum TreeError tree_verify_NOT_USE(const tree_t* const tree)
{
    if (start_node_ == NULL)
        start_node_ = tree;

    switch (is_invalid_ptr(tree))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_TREE_IS_NULL;
        case PTR_STATES_INVALID:     return TREE_ERROR_TREE_IS_INVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum tree, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    if (tree->type != NODE_TYPE_NUM && tree->data.num < 0)
        return TREE_ERROR_DATA_LESS_ZERO;

    switch (is_invalid_ptr(tree->pt))
    {
        case PTR_STATES_VALID:       break;
        case PTR_STATES_NULL:        return TREE_ERROR_PT_IS_NULL;
        case PTR_STATES_INVALID:     return TREE_ERROR_PT_IS_INVALID;
        case PTR_STATES_ERROR:       return TREE_ERROR_STANDARD_ERRNO;
        
        default:
            fprintf(stderr, "Unknown PtrState enum tree, it's soooo bad\n");
            return TREE_ERROR_UNKNOWN;
    }

    size_t cur_size = 0;
    size_counter_   = 0;

    if (tree->lt)
    {
        if (tree->lt->pt != tree)
            return TREE_ERROR_LT_PT_INCORRECT;
        
        TREE_ERROR_HANDLE(tree_verify_NOT_USE(tree->lt));
    }

    cur_size += size_counter_;
               size_counter_ = 0;

    if (tree->rt)
    {
        if (tree->rt->pt != tree)
            return TREE_ERROR_RT_PT_INCORRECT;
            
        TREE_ERROR_HANDLE(tree_verify_NOT_USE(tree->rt));
    }
    
    cur_size += size_counter_ + 1;
                size_counter_ = cur_size;

    if (tree->size > cur_size) return TREE_ERROR_SIZE_GREATER;
    if (tree->size < cur_size) return TREE_ERROR_SIZE_LESSER;

    if (start_node_ == tree)
    {
        size_counter_ = 0;
        start_node_   = NULL;
    }   

    return TREE_ERROR_SUCCESS;
}

#endif /* NDEBUG */