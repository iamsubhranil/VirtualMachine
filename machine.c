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

#define NUM_MEM 65535 // 2^16


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
	Operand op1;
} OneAddress;

typedef struct{
	Operand op1;
	Operand op2;
} TwoAddress;

typedef union{
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
	CellType type;
	CellData data;
} Cell;

typedef struct{
	uint32_t registers[NUM_REG]; // GPRs
	uint16_t pc; // Program counter, initially will point to the first instruction location
	Cell memory[NUM_MEM]; // Memory will store both data and instruction
	unsigned short halt : 1;
	SymbolTable *symbolTable;
	uint16_t occupiedAddress;
} Machine;

void writeData(Machine m, uint16_t add, uint32_t val){
	m.memory[add].type = DATA;
	m.memory[add].data.value = val;
	m.occupiedAddress++;
}

void writeInstruction(Machine m, uint16_t add, Instruction ins){
	m.memory[add].type = INSTRUCTION;
	m.memory[add].data.instruction = ins;
	m.occupiedAddress++;
}

uint32_t readData(Machine m, uint16_t add){
	return m.memory[add].data.value;
}

Instruction readInstruction(Machine m, uint16_t add){
	return m.memory[add].data.instruction;
}


/* Machine cycle primitives */

void execute(Machine m, Instruction ins){
	Data d1, d2;
	Operand op1, op2;
	switch(ins.format){
		case ONE_ADDRESS: op1 = ins.operands.onea.op1;
				  d1 = op1.data;
				  break;
		case TWO_ADDRESS: op1 = ins.operands.twoa.op1;
				  op2 = ins.operands.twoa.op2;
				  d1 = op1.data;
				  d2 = op2.data;
				  break;
		case ZERO_ADDRESS: break;
	}
	switch(ins.opcode){
		case INCR: {
				   uint32_t val;
				   switch(op1.mode){
					   case REGISTER: m.registers[d1.rega] += 1;
							  break;
					   case DIRECT: val = readData(m, d1.mema) + 1;
							writeData(m, d1.mema, val);
							break;
					   case IMMEDIATE: break; // TODO: Handle error
				   }
				   break;
			   }
		case DECR: {
				   uint32_t val;
				   switch(op1.mode){
					   case REGISTER: m.registers[d1.rega] -= 1;
							  break;
					   case DIRECT: val = readData(m, d1.mema) - 1;
							writeData(m, d1.mema, val);
							break;
					   case IMMEDIATE: break; // TODO: Handle error
				   }
				   break;
			   }
		case LET: { 
				  uint32_t val = d1.imv;
				  switch(op2.mode){
					  case REGISTER: m.registers[d2.rega] = val;
							 break;
					  case DIRECT: writeData(m, d2.mema, val);
						       break;
					  case IMMEDIATE: break; // TODO: Handle error
				  }
				  break;
			  }
		case LOAD: switch(op1.mode){
				   case REGISTER: m.registers[d2.rega] = m.registers[d1.rega];
						  break;
				   case DIRECT: m.registers[d2.rega] = readData(m, d1.mema);
						break;
				   case IMMEDIATE: break; // TODO: Handle error
			   }
			   break;
		case STORE: switch(op2.mode){
				    case REGISTER: m.registers[d2.rega] = m.registers[d1.rega];
						   break;
				    case DIRECT: writeData(m, d2.mema, m.registers[d1.rega]);
						 break;
				    case IMMEDIATE: break; // TODO: Handle error
			    }
		case HALT: m.halt = 1;
			   break;
		case UNLET: printf("[ERROR] UNLET not defined!");
			    break;
	}

}

Instruction decode(Cell cell){
	switch(cell.type){
		case DATA:{
				  Instruction halt = {ZERO_ADDRESS, HALT, {{{DIRECT, {0}}}}};
				  return halt;
			  }
		case INSTRUCTION:
			  return cell.data.instruction;
	}
}

Cell fetch(Machine m){
	uint16_t pc = m.pc;
	return m.memory[pc];
}

void run(Machine m){
	while(!m.halt){
		Cell cell = fetch(m);
		Instruction ins = decode(cell);
		execute(m, ins);
		m.pc++;
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

int main(){
	Machine m;
	m.symbolTable = NULL;
	m.halt = 0;
	m.pc = 0;

	int insert = 1;
	char *buff = NULL;
	size_t size;
	char *token;
	while(insert){
		printf("\n > ");
		size = readline(&buff);
		token = strtok(buff, " ");
		OpCode op;
		InstructionFormat format;
		Operands os;
		AddressingMode am1, am2;
		Operand o1, o2;
		Data d1, d2;
		if(strcmp(token, "let")==0){
			op = LET;
			format = TWO_ADDRESS;
			am1 = IMMEDIATE;
			am2 = VARIABLE;
		}
		else if(strcmp(token, "incr")==0){
			op = INCR;
			format = ONE_ADDRESS;
		}
		else if(strcmp(token, "load")==0){
			op = LOAD;
			format = TWO_ADDRESS;
			am1 = DIRECT;
			am2 = REGISTER;
		}
		else if(strcmp(token, "store")==0){
			op = STORE;
			format = TWO_ADDRESS;
			am1 = REGISTER;
			am2 = DIRECT;
		}
		else if(strcmp(token, "decr")==0){
			op = DECR;
			format = ONE_ADDRESS;
		}
		else if(strcmp(token, "halt")==0){
			op = HALT;
			format = ZERO_ADDRESS;
		}
		token = strtok(NULL, " ");
		switch(op){
			case LET:
				uint32_t val = atoi(token);
				d1.imv = val;
				o1.mode = IMMEDIATE;
				token = strtok(NULL, " ");
				d2.name = token;
				o2.mode = VARIABLE;
				TwoAddress ta = {o1, o2};
				os.twoa = ta;
				break;
			// Start from INCR

		}
	}

	//printf("LET 5, 100");
	//Operand op1, op2;
	//createOperand(&op1, IMMEDIATE, {.imv = 5});
	//createOperand(&op2, DIRECT, {.mema = 100});
	//Operands op = {.twoa = {op1, op2}};
	//Instruction ins = {TWO_ADDRESS, LET, op};
	//writeInstruction(m, 0, ins);

	return 0;
}
