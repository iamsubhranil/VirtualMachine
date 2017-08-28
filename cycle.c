#include"cycle.h"
#include"opcodes.h"
#include"function.h"

#include<stdio.h>

/* Machine cycle primitives */

#define sizeof_arr(x) (sizeof(x)/sizeof(x[0]))

static void execute(Machine *m, Instruction ins) {
    if ((ins.opcode - 0xA0) > sizeof_arr(func))
        printf("\n[ERROR] Undefined opcode 0x%x!", ins.opcode);
    else
        func[ins.opcode - 0xA0](m, ins.operands);
}

static Instruction decode(Cell cell) {
    switch (cell.type) {
        case DATA: {
            Instruction halt = {ZERO_ADDRESS, HALT, {.zeroa = {0}}};
            return halt;
        }
        case INSTRUCTION:
            return cell.data.instruction;
    }
}

static Cell fetch(Machine *m) {
    uint16_t pc = m->pc;
    //printf("\nProgram Counter : %u", pc);
    //printMem(*m, pc);
    return m->memory[pc];
}

void run(Machine *m) {
    while (!m->halt) {
        //printf("\n[MACHINE] Running!");
        Cell cell = fetch(m);
        uint16_t backup = m->pc;
        Instruction ins = decode(cell);
        execute(m, ins);
        if (m->pc == backup)
            m->pc++;
    }
}

