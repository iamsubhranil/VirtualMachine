#include<stdlib.h>
#include"instruction.h"

static void freeOperand(Operand *op){
	//if(op->mode==VARIABLE)
		//free(op->data.name);
}

static void freeInstruction(Instruction ins){
	switch(ins.format){
		case ZERO_ADDRESS:
			break;
		case ONE_ADDRESS:
			freeOperand(&ins.operands.onea.op1);
			break;
		case TWO_ADDRESS:
			freeOperand(&ins.operands.twoa.op1);
			freeOperand(&ins.operands.twoa.op2);
			break;
		case THREE_ADDRESS:
			freeOperand(&ins.operands.threa.op1);
			freeOperand(&ins.operands.threa.op2);
			freeOperand(&ins.operands.threa.op3);
			break;
	}
}

void freeInstructions(Instructions *ins){
	if(ins==NULL)
		return;
	uint16_t i = 0;
	while(i<ins->noi){
		freeInstruction(ins->instructions[i]);
		i++;
	}
	//free(ins->instructions);
	free(ins);
}
