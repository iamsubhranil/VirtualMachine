#ifndef OPERANDS_H
#define OPERANDS_H

#include<stdint.h>

/* Addressing modes */

#define IMMEDIATE 0x20
#define REGISTER 0x21
#define DIRECT 0x22
#define VARIABLE 0x23
#define IMMEDIATES 0x24 // Immediate string
#define TERMINATOR 0x2f // To denote the end of an expectedArguments list
/* Data types */

typedef union {
    uint32_t imv; // Immediate value
    char *ims; // Immediate string
    uint8_t rega; // Register addressing
    uint16_t mema; // Direct addressing
    char *name; // Variable addressing
} Data;

/* Operand types */

typedef struct {
    uint8_t mode;
    Data data;
} Operand;

typedef struct {
    char dummy;
} ZeroAddress;

typedef struct {
    Operand op1;
} OneAddress;

typedef struct {
    Operand op1;
    Operand op2;
} TwoAddress;

typedef struct {
    Operand op1;
    Operand op2;
    Operand op3;
} ThreeAddress;

typedef union {
    ZeroAddress zeroa;
    OneAddress onea;
    TwoAddress twoa;
    ThreeAddress threa;
} Operands;

void getOperand(Operand *op, char *val, int *insert);

//void checkOperand(Operand op, uint8_t function, int opnum, int *insert);

#endif
