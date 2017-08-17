#include"cycle.h"
#include"opcodes.h"

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
	Data d1, d2;
	Operand op1, op2;
	//printf("\n[EXECUTE] Instruction details : ");
	//printIns(ins);
	//printf("\n");
	switch(ins.format){
		case ONE_ADDRESS: op1 = ins.operands.onea.op1;
				  d1 = op1.data;
				  break;
		case TWO_ADDRESS: op1 = ins.operands.twoa.op1;
				  op2 = ins.operands.twoa.op2;
				  d1 = op1.data;
				  d2 = op2.data;
				  break;
		case ZERO_ADDRESS:
				  break;
	}
	switch(ins.opcode){
		case INCR: {
				   uint32_t val;
				   switch(op1.mode){
					   case REGISTER: m->registers[d1.rega] += 1;
							  break;
					   case DIRECT: val = readData(m, d1.mema) + 1;
							writeData(m, d1.mema, val);
							break;
					   case VARIABLE:{ 
								 uint16_t add = getAddress(m, d1.name);
								 val = readData(m, add) + 1;
								 writeData(m, add, val);
								 break;
							 }
					   case IMMEDIATE: break; // TODO: Handle error
				   }
				   break;
			   }
		case DECR: {
				   uint32_t val;
				   switch(op1.mode){
					   case REGISTER: m->registers[d1.rega] -= 1;
							  break;
					   case DIRECT: val = readData(m, d1.mema) - 1;
							writeData(m, d1.mema, val);
							break;
					   case VARIABLE: {
								  uint16_t add = getAddress(m, d1.name);
								  val = readData(m, add) - 1;
								  writeData(m, add, val);
								  break;

							  }
					   case IMMEDIATE: break; // TODO: Handle error
				   }
				   break;
			   }
		case LET: { 
				  uint32_t val = d1.imv;
				  switch(op2.mode){
					  case REGISTER: m->registers[d2.rega] = val;
							 break;
					  case DIRECT: writeData(m, d2.mema, val);
						       break;
					  case VARIABLE: {
								 uint16_t add = getAddress(m, d2.name);
								 writeData(m, add, val);
								 break;
							 }
					  case IMMEDIATE: break; // TODO: Handle error
				  }
				  break;
			  }
		case LOAD: switch(op1.mode){
				   case REGISTER: m->registers[d2.rega] = m->registers[d1.rega];
						  break;
				   case DIRECT: m->registers[d2.rega] = readData(m, d1.mema);
						break;
				   case VARIABLE: m->registers[d2.rega] = readData(m, getAddress(m, d1.name));
						   //printf("\n[LOAD] Load complete to reg%u of val %u!\n", d2.rega, m->registers[d2.rega]);
						   //printf("\n[LOAD] Expected : %u from address : %u\n", readData(m, getAddress(m, d1.name)), getAddress(m, d1.name));
						  break;
				   case IMMEDIATE: break; // TODO: Handle error
			   }
			   break;
		case STORE: switch(op2.mode){
				    case REGISTER: m->registers[d2.rega] = m->registers[d1.rega];
						   break;
				    case DIRECT: writeData(m, d2.mema, m->registers[d1.rega]);
						 break;
				    case VARIABLE: writeData(m, getAddress(m, d2.name), m->registers[d1.rega]);
						   break;
				    case IMMEDIATE: break; // TODO: Handle error
			    }
			    break;
		case HALT: //printf("\nHALTING REQUIRED!");
			    m->halt = 1;
			    break;
		case UNLET:{ 
				   switch(op1.mode){
					   case IMMEDIATE: break;
					   case REGISTER: break;
					   case VARIABLE: deallocate(m, d1.name);
							  break;
					   case DIRECT: writeData(m, d1.mema, 0);
							break;
				   }
				   break;
			   }
		case PRINT:{ 
				   switch(op1.mode){
					   case IMMEDIATE: printf("%u", d1.imv);
							   break;
					   case REGISTER: //printf("\n[REG] %u", d1.rega);
							   printf("%u", m->registers[d1.rega]);
							   // printf("\n[PRINT] Printed from reg%u", d1.rega);
							   break;
					   case DIRECT: printf("%u", readData(m, d1.mema));
							break;
					   case VARIABLE: printf("%u", readData(m, getAddress(m, d1.name)));
							  break;
				   }
				   break;
			   }
		case ADD: { uint32_t readval = getVal(op1, m), temp = 0;
				  switch(op2.mode){
					  case REGISTER: m->registers[d2.rega] += readval;
							 break;
					  case DIRECT: temp = readData(m, d2.mema);
						       writeData(m, d2.mema, temp + readval);
						       break;
					  case VARIABLE: temp = readData(m, getAddress(m, d2.name));
							 writeData(m, getAddress(m, d2.name), temp + readval);
							 break;
				  }
				  break;
			  }
		case SUB: { uint32_t readval = getVal(op1, m), temp = 0;
				  switch(op2.mode){
					  case REGISTER: m->registers[d2.rega] -= readval;
							 break;
					  case DIRECT: temp = readData(m, d2.mema);
						       writeData(m, d2.mema, temp - readval);
						       break;
					  case VARIABLE: temp = readData(m, getAddress(m, d2.name));
							 writeData(m, getAddress(m, d2.name), temp - readval);
							 break;
				  }
				  break;
			  }
		case MUL: { uint32_t readval = getVal(op1, m), temp = 0;
				  switch(op2.mode){
					  case REGISTER: m->registers[d2.rega] *= readval;
							 break;
					  case DIRECT: temp = readData(m, d2.mema);
						       writeData(m, d2.mema, temp * readval);
						       break;
					  case VARIABLE: temp = readData(m, getAddress(m, d2.name));
							 writeData(m, getAddress(m, d2.name), temp * readval);
							 break;
				  }
				  break;
			  }
		case DIV: { uint32_t readval = getVal(op1, m), temp = 0;
				  switch(op2.mode){
					  case REGISTER: m->registers[d2.rega] /= readval;
							 break;
					  case DIRECT: temp = readData(m, d2.mema);
						       writeData(m, d2.mema, temp / readval);
						       break;
					  case VARIABLE: temp = readData(m, getAddress(m, d2.name));
							 writeData(m, getAddress(m, d2.name), temp / readval);
							 break;
				  }
				  break;
			  }
	}

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
		Instruction ins = decode(cell);
		execute(m, ins);
		m->pc++;
	}
}

