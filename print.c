#include<stdio.h>
#include"print.h"
#include"names.h"

void printOperand(Operand o) {
    printf("\n\t\t\tAddressing Mode : %s", modeNames[o.mode - 0x20]);
    switch (o.mode) {
        case IMMEDIATE:
            printf("\n\t\t\tValue : %u", o.data.imv);
            break;
        case REGISTER:
            printf("\n\t\t\tValue : %u", o.data.rega);
            break;
        case DIRECT:
            printf("\n\t\t\tValue : %u", o.data.mema);
            break;
        case VARIABLE:
            printf("\n\t\t\tValue : %s", o.data.name);
    }
}

void printIns(Instruction ins) {
    printf("\n\tInstruction : %s", insNames[ins.opcode - 0xA0]);
    printf("\n\t\tFormat : %s", formatNames[ins.format - 0x30]);
    int l = ins.format - 0x30, i = 0;
    while(i < l){
        printf("\n\tOperand %d : ", (i + 1));
        printOperand(ins.operands[i]);
        i++;
    }
}

void printMem(Machine m, uint16_t add) {
    Cell c = m.memory[add];
    printf("\nCell Address : %u", add);
    printf("\nCell Type : %s", cellNames[c.type]);
    printf("\nAcquired : %u", c.acquired);
    if (c.acquired) {
        switch (c.type) {
            case INSTRUCTION:
                printIns(c.data.instruction);
                break;
            case DATA:
                printf("\n\tValue : %u", c.data.value);
                break;
        }
    }
    printf("\n");
}
