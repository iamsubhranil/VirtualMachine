#include <stdlib.h>
#include "instruction.h"
#include <stdio.h>

static void freeOperand(Operand *op, int length){
    int i = 0;
    while(i < length){
        if(op[i].mode==VARIABLE)
            free(op[i].data.name);
        i++;
    }
    free(op);
}

static void freeInstruction(Instruction ins){
    freeOperand(ins.operands, ins.format - 0x30);
}

void freeInstructions(Instructions *ins){
    if(ins==NULL)
        return;
    uint16_t i = 0;
    while(i<ins->noi){
        freeInstruction(ins->instructions[i]);
        i++;
    }
    free(ins->instructions);
    free(ins);
}

void concatInstructions(Instructions *dest, Instructions *source){
    uint16_t totalIns = dest->noi + source->noi, i = 0;
    //printf("\nPrevious : %u New : %u Total : %u Calc size : %lu\n", dest->noi, source->noi, totalIns, sizeof(dest->instructions)/sizeof(dest->instructions[0]));
    dest->instructions = (Instruction *)realloc(dest->instructions, totalIns * sizeof(Instruction));
    while(dest->noi < totalIns){
        dest->instructions[dest->noi] = source->instructions[i];
        dest->noi++;
        i++;
    }
}
