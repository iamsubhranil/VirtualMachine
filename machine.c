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

void writeInstruction(Machine *m, uint16_t add, Instruction ins) {
    m->memory[add].acquired = 1;
    m->memory[add].type = INSTRUCTION;
    m->memory[add].data.instruction = ins;
    m->occupiedAddress++;
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
    newSymbol->symbolName = strdup(symbol);
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
