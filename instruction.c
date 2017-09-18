#include<stdlib.h>
#include"instruction.h"

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
