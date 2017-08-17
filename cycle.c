#include"cycle.h"
#include"opcodes.h"
#include"function.h"

#include<stdio.h>

/* Machine cycle primitives */

static uint32_t getVal(Operand o, Machine *m){
	Data d1 = o.data;
	switch(o.mode){
		case IMMEDIATE: return d1.imv;
				break;
		case REGISTER: return m->registers[d1.rega];
			       break;
		case DIRECT: return readData(m, d1.mema);
			     break;
		case VARIABLE: return readData(m, getAddress(m, d1.name));
			       break;
	}
	return 0;
}

static void execute(Machine *m, Instruction ins){
	if((ins.opcode-0x10)>NUMFUNCS)
		printf("\n[ERROR] Undefined opcode %u!", ins.opcode);
	else
		func[ins.opcode-0x10](m, ins.operands);
}

static Instruction decode(Cell cell){
	switch(cell.type){
		case DATA:{
				  Instruction halt = {ZERO_ADDRESS, HALT, {.zeroa = {0}}};
				  return halt;
			  }
		case INSTRUCTION:
			  return cell.data.instruction;
	}
}

static Cell fetch(Machine *m){
	uint16_t pc = m->pc;
	//printf("\nProgram Counter : %u", pc);
	//printMem(*m, pc);
	return m->memory[pc];
}

void run(Machine *m){
	while(!m->halt){
		//printf("\n[MACHINE] Running!");
		Cell cell = fetch(m);
		uint16_t backup = m->pc;
		Instruction ins = decode(cell);
		execute(m, ins);
		if(m->pc==backup)
			m->pc++;
	}
}

