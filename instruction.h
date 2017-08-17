#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include<stdint.h>
#include"operands.h"
/* Instruction Format */

#define ZERO_ADDRESS 0x30
#define ONE_ADDRESS 0x31
#define TWO_ADDRESS 0x32

/* Instruction definition */

typedef struct{
	uint8_t format;
	uint8_t opcode;
	Operands operands;
} Instruction;

#endif
