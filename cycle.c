#include "cycle.h"
#include "opcodes.h"
#include "function.h"
#include "floader.h"

#include <string.h>
#include <stdio.h>

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

static void dryRun(Machine *m){
    uint16_t i = 0;
    int c = 0;
    loadFunctions(&c);
    Function *enddef = getFunction(strdup("enddef")), *def = getFunction(strdup("def")), *halt = getFunction(strdup("halt")),
        *setl = getFunction(strdup("setl"));
    int cont = 1;
    while(cont){
        m->pc = i;
        Instruction ins = decode(fetch(m));
        if(ins.opcode == def->opcode || ins.opcode == setl->opcode)
            execute(m, ins);
        else if(ins.opcode == enddef->opcode){
            if(strcmp(ins.operands.onea.op1.data.name, "main")==0)
                cont = 0;
        }
        else if(ins.opcode == halt->opcode)
            cont = 0;
        i++;
    }
}

void run(Machine *m) {
    dryRun(m);
    uint16_t l = getAddress(m, strdup("__start__def__main"));
    uint16_t startAddress = readData(m, l);
    //printf("\nStarting from %u!", startAddress);
    m->pc = startAddress;
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

