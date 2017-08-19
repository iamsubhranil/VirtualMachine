#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>
#include"instruction.h"

#define NUM_REG 16 // 2^4

#define NUM_MEM 16383 // 2^16

// To keep correspondance between symbol name and memory location
typedef struct SymT {
    char *symbolName;
    uint16_t mema;
    struct SymT *next;
} SymbolTable;

typedef enum {
    INSTRUCTION,
    DATA
} CellType;

typedef union {
    Instruction instruction;
    uint32_t value;
} CellData;

typedef struct {
    uint8_t acquired : 1;
    CellType type;
    CellData data;
} Cell;

typedef struct {
    uint32_t registers[NUM_REG]; // GPRs
    uint16_t pc; // Program counter, initially will point to the first instruction location
    Cell memory[NUM_MEM]; // Memory will store both data and instruction
    uint8_t halt : 1;
    uint16_t occupiedAddress;
    SymbolTable *symbolTable;
} Machine;

void writeData(Machine *m, uint16_t add, uint32_t val);

void writeInstruction(Machine *m, uint16_t add, Instruction ins);

uint32_t readData(Machine *m, uint16_t add);

Instruction readInstruction(Machine *m, uint16_t add);

uint16_t getAddress(Machine *m, char *symbol);

void deallocate(Machine *m, char *symbol);

Machine *getMachine();
#endif
