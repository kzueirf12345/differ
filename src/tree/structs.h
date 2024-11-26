#ifndef DIFFER_SRC_TREE_TREE_STRUCTS_H
#define DIFFER_SRC_TREE_TREE_STRUCTS_H

#include <stdio.h>
#include <assert.h>
#include <math.h>



enum NodeType
{
    NODE_TYPE_NUM = 0,
    NODE_TYPE_VAR = 1,
    NODE_TYPE_OP  = 2
};

enum OpType
{
    OP_TYPE_SUM     = '+',
    OP_TYPE_SUB     = '-',
    OP_TYPE_MUL     = '*',
    OP_TYPE_DIV     = '/',
    OP_TYPE_POW     = '^',

    OP_TYPE_SIN     = 's',
    OP_TYPE_COS     = 'c',
    OP_TYPE_TG      = 't',
    OP_TYPE_CTG     = 'c' + 't',

    OP_TYPE_ARCSIN  = 'a' + 's',
    OP_TYPE_ARCCOS  = 'a' + 'c',
    OP_TYPE_ARCTG   = 'a' + 't',
    OP_TYPE_ARCCTG  = 'a' + 'c' + 't', 

    OP_TYPE_SH      = 's' + 'h',
    OP_TYPE_CH      = 'c' + 'h',
    OP_TYPE_TH      = 't' + 'h',
    OP_TYPE_CTH     = 'c' + 't' + 'h',

    OP_TYPE_LOG     = 'l',

    OP_TYPE_UNKNOWN = 0
};
static_assert(OP_TYPE_UNKNOWN == 0);

typedef union TreeData
{
    double num;
    enum OpType op;
    int var;
} tree_data_u;

typedef struct Tree
{ 
    tree_data_u data;
    enum NodeType type;

    struct Tree* lt;
    struct Tree* rt;
    struct Tree* pt;

    size_t size;
} tree_t;

#endif /* DIFFER_SRC_TREE_TREE_STRUCTS_H */