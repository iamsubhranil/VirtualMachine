#include<stdint.h>
#include<stdlib.h>
#include<string.h>

#include"machine.h"

void writeData(Machine *m, uint16_t add, uint32_t val) {
	m->memory[add].acquired = 1;
	m->memory[add].type = DATA;
	m->memory[add].data.value = val;
	m->occupiedAddress++;
}

static void writeInstruction(Machine *m, uint16_t add, Instruction ins) {
	m->memory[add].acquired = 1;
	m->memory[add].type = INSTRUCTION;
	m->memory[add].data.instruction = ins;
	m->occupiedAddress++;
}

void writeInstructions(Machine *m, Instructions *ins){
	uint16_t length = ins->noi;
	uint16_t i = 0;
	while(i<length){
		writeInstruction(m, i, ins->instructions[i]);
		i++;
	}
}

uint32_t readData(Machine *m, uint16_t add) {
	return m->memory[add].data.value;
}

Instruction readInstruction(Machine *m, uint16_t add) {
	return m->memory[add].data.instruction;
}

static uint16_t getFirstFree(Machine m) {
	uint16_t add = 0;
	while (m.memory[add].acquired)
		add++;
	return add;
}

static uint16_t memallocate(Machine *m, char *symbol) {
	uint16_t allocationAddress = getFirstFree(*m);
	//printf("\n[MEMALLOCATE] Allocating memory for %s at address %u\n", symbol, allocationAddress);
	SymbolTable *newSymbol = (SymbolTable *) malloc(sizeof(SymbolTable));
	newSymbol->symbolName = symbol;
	newSymbol->next = NULL;
	newSymbol->mema = allocationAddress;
	if (m->symbolTable == NULL) {
		m->symbolTable = newSymbol;
	} else {
		SymbolTable *temp = m->symbolTable;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = newSymbol;
	}
	m->memory[allocationAddress].acquired = 1;
	m->memory[allocationAddress].data.value = 0;
	m->occupiedAddress++;
	return allocationAddress;
}

uint16_t getAddress(Machine *m, char *symbol) {
	SymbolTable *table = m->symbolTable;
	while (table != NULL) {
		if (strcmp(symbol, table->symbolName) == 0) {
			//printf("\n[GETADDR] Address of %s is %u\n", symbol, table->mema);
			return table->mema;
		}
		table = table->next;
	}
	return memallocate(m, symbol);
}

void deallocate(Machine *m, char *symbol) {
	SymbolTable *temp = m->symbolTable;
	SymbolTable *backup = NULL;
	while (temp != NULL) {
		if (strcmp(symbol, temp->symbolName) == 0) {
			if (backup == NULL)
				m->symbolTable = m->symbolTable->next;
			else
				backup->next = temp->next;
			m->memory[temp->mema].acquired = 0;
			free(temp);
			m->occupiedAddress--;
			break;
		}
		backup = temp;
		temp = temp->next;
	}
}

Machine *getMachine(){
	Machine *m = (Machine *)malloc(sizeof(Machine));
	m->pc = 0;
	m->occupiedAddress = 0;
	m->symbolTable = NULL;
	m->halt = 0;
	int i;
	for(i = 0;i < NUM_REG;i++){
		m->registers[i] = 0;
	}
	for(i = 0;i < NUM_MEM;i++){
		m->memory[i].acquired = 0;
	}
	return m;
}

static void convertVariableToDirect(Machine *m, Operand *a) {
	if (a->mode == VARIABLE) {
		a->mode = DIRECT;
		a->data.mema = getAddress(m, a->data.name);
	}
}

void finalizeInstructions(Machine *m, Instructions *ins){
	uint16_t i = 0;
	while(i<ins->noi){
		Instruction *in = &ins->instructions[i];
        int l = in->format - 0x30, j = 0;
        while(j < l){
            convertVariableToDirect(m, &in->operands[j]);
            j++;
        }
	}
}

void destroyMachine(Machine *m){
	SymbolTable *top = m->symbolTable;
	while(top!=NULL){
		SymbolTable *back = top;
		top = top->next;
		//free(back->symbolName);
		free(back);
	}
	free(m);	
}
