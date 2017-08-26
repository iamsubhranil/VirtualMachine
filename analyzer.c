#include "analyzer.h"
#include "stdint.h"
#include "stdio.h"
#include "names.h"
#include "loader.h"

static void printOperand(Operand op){
    printf("  0x%2x  |  %4s  |", op.mode, modeNames[op.mode - 0x20]);
    switch(op.mode){
        case VARIABLE: 
            printf(" %8s |", op.data.name);
            break;
        case DIRECT:
            printf(" %8u |", op.data.mema);
            break;
        case REGISTER:
            printf(" %8u |", op.data.rega);
            break;
        default: 
            printf(" %8x |", op.data.imv);
            break;
    }
}

static void printNull(){
    printf("   --   |   --   |     --   |");
}

static void printOperands(Operands op, uint8_t insMode){
    switch(insMode){
        case THREE_ADDRESS: 
            printOperand(op.threa.op1);
            printOperand(op.threa.op2);
            printOperand(op.threa.op3);
            break;
        case TWO_ADDRESS: 
            printOperand(op.twoa.op1);
            printOperand(op.twoa.op2);
            printNull();
            break;
        case ONE_ADDRESS: 
            printOperand(op.onea.op1);
            printNull();
            printNull();
            break;
        case ZERO_ADDRESS: 
            printNull();
            printNull();
            printNull();
            break;

    }
}

void analyze(Instructions *ins){
    uint16_t length = ins->noi, i = 0;
    Instruction *codes = ins->instructions;
    printf("\n---------------------------------------------------------------------------------------------------------------------------------");
    printf("\n  Count  |       Instruction             |                   Operand(s)                                                         |");
    printf("\n         |-------------------------------|--------------------------------------------------------------------------------------|");
    printf("\n         |    Operation   |   Format     |    Operand 1               |  Operand 2                 |   Operand 3                |");
    printf("\n         |----------------|--------------|----------------------------|----------------------------|----------------------------|");
    printf("\n         |  Code  | Name  | Code |  Type |  Add. Mode      |  Value   |  Add. Mode      |  Value   |  Add. Mode      |  Value   |");
    printf("\n         |--------|-------|------|-------|-----------------|----------|-----------------|----------|-----------------|----------|");
    printf("\n         |        |       |      |       |  Code  |  Name  |          |  Code  |  Name  |          |  Code  |  Name  |          |");
    while(i<length){
        Instruction in = codes[i];
     	printf("\n---------|--------|-------|------|-------|--------|--------|----------|--------|--------|----------|--------|--------|----------|");
        printf("\n  %5u  |  0x%2x  | %5s | 0x%2x | %5s |"
                , i, in.opcode, insNames[in.opcode - 0x10], in.format, formatNames[in.format - 0x30]);
        Operands op = in.operands;
        printOperands(op, in.format);
        i++;
    };
    printf("\n---------------------------------------------------------------------------------------------------------------------------------");
}
