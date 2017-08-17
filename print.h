#ifndef PRINT_H
#define PRINT_H

#include"instruction.h"
#include"operands.h"
#include"machine.h"

#include<stdint.h>

void printIns(Instruction i);
void printMem(Machine m, uint16_t add);
void printOperand(Operand o);

#endif
