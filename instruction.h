#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include "operands.h"
/* Instruction Format */

#define ZERO_ADDRESS 0x30
#define ONE_ADDRESS 0x31
#define TWO_ADDRESS 0x32
#define THREE_ADDRESS 0x33

/* Instruction definition */

typedef struct {
    uint8_t format;
    uint8_t opcode;
    Operand *operands;
} Instruction;

typedef struct{
	Instruction *instructions;
	uint16_t noi;
} Instructions;

void freeInstructions(Instructions *ins);
#endif
