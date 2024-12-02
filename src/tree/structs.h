#ifndef DIFFER_SRC_TREE_STRUCTS_H
#define DIFFER_SRC_TREE_STRUCTS_H

#include <stdio.h>

enum NodeType
{
    NODE_TYPE_NUM = 0,
    NODE_TYPE_VAR = 1,
    NODE_TYPE_OP  = 2
};

#define OPERATION_HANDLE(_type, _name, _tex_name, _count_operands, _notation, _num)                 \
        OP_TYPE_##_type = _num,

enum OpType
{
    #include "tree/operation/codegen.h"
    MAX_OP_TYPE
};

#undef OPERATION_HANDLE

typedef double operand_t;

typedef union TreeData
{
    operand_t num;
    enum OpType op;
    char var;
} tree_data_u;

typedef struct Tree
{
    tree_data_u   data;
    enum NodeType type;

    struct Tree* lt;
    struct Tree* rt;
    struct Tree* pt;

    size_t size;
} tree_t;

typedef struct TexStreams
{
    FILE* file;
    char* filename;
    
} tex_streams_t;

#endif /* DIFFER_SRC_TREE_STRUCTS_H */