#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
/* 
 *
 *
 *
 *
 */

#define REG_BITS 4
#define NUM_REG 16 // 2^4

#define NUM_MEM 16383 // 2^16


/* Operation codes */

typedef enum{
	/* One address */
	INCR,
	DECR,
	UNLET,
	/* Two address */
	LOAD,
	STORE,
	LET,
	/* Zero address */
	HALT
} OpCode;

static char* insNames[] = {"INCR", "DECR", "UNLET", "LOAD", "STORE", "LET", "HALT"};

/* Addressing modes */

typedef enum{
	IMMEDIATE,
	REGISTER,
	DIRECT,
	VARIABLE
} AddressingMode;

/* Data types */

typedef union{
	uint32_t imv; // Immediate addressing
	uint8_t rega : REG_BITS; // Register addressing
	uint16_t mema; // Direct addressing
	char *name; // Variable addressing
} Data;

/* Operand types */

typedef struct{
	AddressingMode mode;
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

/* Instruction Format */

typedef enum{
	ZERO_ADDRESS,
	ONE_ADDRESS,
	TWO_ADDRESS
} InstructionFormat;

/* Instruction definition */

typedef struct{
	InstructionFormat format;
	OpCode opcode;
	Operands operands;
} Instruction;

/* Machine primitives */

// To keep correspondance between symbol name and memory location
typedef struct SymT{
	char *symbolName;
	uint16_t mema;
	struct SymT *next;
} SymbolTable;

typedef enum{
	INSTRUCTION,
	DATA
} CellType;

typedef union{
	Instruction instruction;
	uint32_t value;
} CellData;

typedef struct{
	unsigned short free : 1;
	CellType type;
	CellData data;
} Cell;

typedef struct{
	uint32_t registers[NUM_REG]; // GPRs
	uint16_t pc; // Program counter, initially will point to the first instruction location
	Cell memory[NUM_MEM]; // Memory will store both data and instruction
	unsigned short halt : 1;
	uint16_t occupiedAddress;
	SymbolTable *symbolTable;
} Machine;

void printIns(Instruction ins){
	printf("\n\tInstruction : %s", insNames[ins.opcode]);
}

void printMem(Machine m, uint16_t add){
	Cell c = m.memory[add];
	switch(c.type){
		case INSTRUCTION:
				printf("\nCell Type : Instruction");
				printIns(c.data.instruction);
				break;
		case DATA: printf("\nCell Type : Data");
			   break;
	}
	printf(" Free : %u", c.free);
}

void writeData(Machine *m, uint16_t add, uint32_t val){
	m->memory[add].free = 0;
	m->memory[add].type = DATA;
	m->memory[add].data.value = val;
	m->occupiedAddress++;
}

void writeInstruction(Machine *m, uint16_t add, Instruction ins){
	m->memory[add].free = 0;
	m->memory[add].type = INSTRUCTION;
	m->memory[add].data.instruction = ins;
	m->occupiedAddress++;
}

uint32_t readData(Machine *m, uint16_t add){
	return m->memory[add].data.value;
}

Instruction readInstruction(Machine *m, uint16_t add){
	return m->memory[add].data.instruction;
}

uint16_t getFirstFree(Machine m){
	uint16_t add = 0;
	while(!m.memory[add].free)
		add++;
	return add;
}

uint16_t memallocate(Machine *m, char *symbol){
	uint16_t allocationAddress = getFirstFree(*m);
	SymbolTable *newSymbol = (SymbolTable *)malloc(sizeof(SymbolTable));
	newSymbol->symbolName = strdup(symbol);
	newSymbol->next = NULL;
	newSymbol->mema = allocationAddress;
	if(m->symbolTable==NULL){
		m->symbolTable = newSymbol;
	}
	else{
		SymbolTable *temp = m->symbolTable;
		while(temp->next!=NULL)
			temp = temp->next;
		temp->next = newSymbol;
	}
	m->memory[allocationAddress].data.value = 0;
	m->occupiedAddress++;
	return allocationAddress;
}

uint16_t getAddress(Machine *m, char *symbol){
	SymbolTable *table = m->symbolTable;
	while(table!=NULL){
		if(strcmp(symbol, table->symbolName)==0)
			return table->mema;
	}
	return memallocate(m, symbol);
}

void deallocate(Machine *m, char *symbol){
	SymbolTable *temp = m->symbolTable;
	SymbolTable *backup = NULL;
	while(temp!=NULL){
		if(strcmp(symbol, temp->symbolName)==0){
			if(backup==NULL)
				m->symbolTable = m->symbolTable->next;
			else
				backup->next = temp->next;
			m->memory[temp->mema].free = 1;
			free(temp);
			m->occupiedAddress--;
			break;
		}
		backup = temp;
		temp = temp->next;
	}
}


/* Machine cycle primitives */

void execute(Machine *m, Instruction ins){
	Data d1, d2;
	Operand op1, op2;
	printf("\n[INSTRUCTION] OpCode : %d Format : %d", (int)ins.opcode, (int)ins.format);
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
								 uint16_t add = getAddress(m, d1.name);
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
		case HALT: printf("\nHALTING REQUIRED!");
			    m->halt = 1;
			   break;
		case UNLET: printf("\n[ERROR] UNLET not defined!");
			    break;
	}

}

Instruction decode(Cell cell){
	switch(cell.type){
		case DATA:{
				  Instruction halt = {ZERO_ADDRESS, HALT, {.zeroa = {0}}};
				  return halt;
			  }
		case INSTRUCTION:
			  return cell.data.instruction;
	}
}

Cell fetch(Machine *m){
	uint16_t pc = m->pc;
	printf("\nPC : %u", pc);
	return m->memory[pc];
}

void run(Machine *m){
	while(!m->halt){
		Cell cell = fetch(m);
		Instruction ins = decode(cell);
		execute(m, ins);
		m->pc++;
	}
}


/*
 * Adds the given character to the buffer. Since it modifies
 * the buffer itself, it returns the pointer in any case.
 * However, the pointer may not be reassigned to a new address
 * after calling realloc(). Also, there is no typical NULL
 * check after realloc, because it will practically never happen.
 * 
 * Arguments => buffer : The buffer to extend
 * 		bufferSize : A pointer to the present size of the buffer
 * 		add : The character to add
 * Returns   => The newly relocated buffer
 */
char * addToBuffer(char *buffer, size_t *bufferSize, char add){
	//char *backup = buffer;
	buffer = (char *)realloc(buffer, ++(*bufferSize)); //Call realloc to extend the buffer to bufferSize+1
	//if(backup!=buffer && *bufferSize>1)
	//	free(backup);
	(*(buffer+(*bufferSize)-1)) = add; //Add the character to the newly available position
	return buffer;
}

/*
 * Limited getline() implementation for non POSIX(read Windows)
 * systems. This method reads a line of arbitrary length from stdin, 
 * stores that in buffer, returning the number of characters read.
 * 
 * Arguments => buffer : The buffer to store the line, terminated with
 * 			EOF or '\n' as applicable
 * Returns => The number of characters read from stdin
 */
size_t readline(char **buffer){
	size_t read_size = 0; // The read counter
	(*buffer) = (char *)malloc(sizeof(char)); // Allocate atleast one char of memory
	char c = 1; // Temporary character to store stdin read

	while(c!=EOF && c!='\n'){ // Continue until the end of line
		c = getc(stdin); // Read a character from stdin
		(*buffer) = addToBuffer((*buffer), &read_size, c); // Add it to the buffer
	}
	return read_size; // Return the amount of characters read
}

void getConstantOperand(Operand *op, char *val){
	Data d = { .imv = (uint32_t)atoi(val)};
	op->mode = IMMEDIATE;
	op->data = d;
}

char *stripFirst(char *val){
	char *buffer = NULL;
	size_t len = strlen(val);
	size_t i = 1;
	size_t dummy = 0;
	while(i>len){
		buffer = addToBuffer(buffer, &dummy, val[i]);
		i++;
	}
	return buffer;
}

void getRegisterOperand(Operand *op, char *val){
	char *buffer = stripFirst(val);

	uint8_t regNo = (uint8_t)atoi(buffer);
	Data d = {.rega = regNo};
	op->mode = REGISTER;
	op->data = d;
	free(buffer);
}

void getVariableOperand(Operand *op, char *val){
	Data d = {.name = strdup(val)};
	op->mode = VARIABLE;
	op->data = d;
}

void getDirectOperand(Operand *op, char *val){
	char *buffer = stripFirst(val);
	uint16_t mem = (uint16_t)atoi(buffer);
	Data d = {.mema = mem};
	op->mode = DIRECT;
	op->data = d;
	free(buffer);
}

void getRegisterOrVariableOperand(Operand *op, char *val){
	if(val[0]=='R')
		getRegisterOperand(op, val);
	else
		getVariableOperand(op, val);
}

Instruction * newInstruction(){
	return (Instruction *)malloc(sizeof(Instruction));
}

int main(){
	Machine m;
	m.symbolTable = NULL;
	m.halt = 0;
	m.pc = 0;

	int insert = 1;
	char *buff = NULL;
	size_t size;
	char *token;
	uint16_t add = 0;
	//Instruction *instructions = NULL, *backup = NULL;
	while(insert){
		printf("\n > ");
		size = readline(&buff);
		token = strtok(buff, " ");
		Instruction* is = newInstruction();
		OpCode *op = &(is->opcode);
		InstructionFormat *format = &(is->format);
		Operands *os = &(is->operands);
		printf("\n[INPUT] [%s]",token);
		if(strcmp(token, "let")==0){
			*op = LET;
			*format = TWO_ADDRESS;
		}
		else if(strcmp(token, "incr")==0){
			*op = INCR;
			*format = ONE_ADDRESS;
		}
		else if(strcmp(token, "load")==0){
			*op = LOAD;
			*format = TWO_ADDRESS;
		}
		else if(strcmp(token, "store")==0){
			*op = STORE;
			*format = TWO_ADDRESS;
		}
		else if(strcmp(token, "decr")==0){
			*op = DECR;
			*format = ONE_ADDRESS;
		}
		else if(strcmp(token, "halt")>=0){
			*op = HALT;
			*format = ZERO_ADDRESS;
			//printf("\n[HALT] Entered!");
		}
		switch(*format){
			case ONE_ADDRESS:{ 
						 token = strtok(NULL, " ");
						 switch(*op){
							 case UNLET: getVariableOperand(&(os->onea.op1), token);
								     break;
							 default: getRegisterOrVariableOperand(&(os->onea.op1), token);
								  break;
						 }
						 break;
					 }
			case TWO_ADDRESS:{
						 Operand *op1 = &(os->twoa.op1);
						 Operand *op2 = &(os->twoa.op2);
						 token = strtok(NULL, " ");
						 switch(*op){
							 case LET: getConstantOperand(op1, token);
								   token = strtok(NULL, " ");
								   getVariableOperand(op2, token);
								   break;
							 case LOAD: getRegisterOrVariableOperand(op1, token);
								    token = strtok(NULL, " ");
								    getRegisterOperand(op2, token);
								    break;
							 case STORE: getRegisterOperand(op1, token);
								     token = strtok(NULL, " ");
								     getRegisterOrVariableOperand(op2, token);
								     break;
							 default: printf("[ERROR] No such two address operations!");
						 }
						 break;
					 }
			case ZERO_ADDRESS:{
						  os->zeroa.dummy = '0';
						  break;
					  }

		}
		writeInstruction(&m, add, *is);
		//printMem(m, add);
		printIns(m.memory[add].data.instruction);
		//printf("\n[INP] OpCode : %d Format : %d Address : %u", (*is).opcode, (*is).format, add);
		add++;
		if(*op==HALT)
			insert = 0;
	}

	run(&m);

	return 0;
}
