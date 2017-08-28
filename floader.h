#ifndef FLOADER_H
#define FLOADER_H

#include "stdint.h"
#include "operands.h"

typedef struct func{
    char *invokation;
    uint8_t opcode;
    uint8_t format;
    uint8_t **expectedArguments;
    struct func *next;
} Function;

void loadFunctions(int *check);

Function *getFunction(char *invokation);

void checkOperand(Function *function, Operand op, int opnum, int *check);

#endif
