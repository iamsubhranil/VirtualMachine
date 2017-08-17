#ifndef OPERANDS_H
#define OPERANDS_H

#include<stdint.h>

/* Addressing modes */

#define IMMEDIATE 0x20
#define REGISTER 0x21
#define DIRECT 0x22
#define VARIABLE 0x23


/* Data types */

typedef union{
	uint32_t imv; // Immediate addressing
	uint8_t rega; // Register addressing
	uint16_t mema; // Direct addressing
	char *name; // Variable addressing
} Data;

/* Operand types */

typedef struct{
	uint8_t mode;
	Data data;
} Operand;

typedef struct{
	char dummy;
} ZeroAddress;

typedef struct{
	Operand op1;
} OneAddress;

typedef struct{
	Operand op1;
	Operand op2;
} TwoAddress;

typedef union{
	ZeroAddress zeroa;
	OneAddress onea;
	TwoAddress twoa;
} Operands;

#endif
