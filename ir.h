#ifndef IR_def
#define IR_def

#include <stddef.h>
#include <stdint.h>
#include "asm_data.h"

typedef enum CALC_OP_KIND
{
    PUSH,
    ADD,
    SUB,
    DIV,
    MOD,
    MUL,
    RET,
    SWAP,
    DUP,
    PUSH_ARG,
    IF,
    SQRT,
    END,
} CALC_OP_KIND;

__int64_t STACKCHANGES[END + 1] = {1, -1, -1, -1, -1, -1, -1, 0, 1, 1, -2,0, 0};
__int64_t STACKREQUIREMENTS[END + 1] = {0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3,1, 0};
__int64_t COMPSIZES[END + 1] = {5, 6, 6, 9, 9, 6, 2, 4, 3, 1, 13, 19,0};

typedef struct
{
    CALC_OP_KIND kind;          // operation
    __int64_t op;               // operand
    __int64_t stackchange;      // by how much does the stack change
    __int64_t stackrequirement; // how many elements need to be in the stack
    __uint64_t compiled_size;   // number of bytes in the machine code implementation
    __int64_t reduce;           // is this a reduce operation
} CALC_OP;

#endif