#include "analyzer.h"
#include <stdint.h>
#include <stdio.h>
#include "names.h"
#include <stdlib.h>
#include <string.h>

static char **strings = NULL;
static int count = 0;

static int getCount(char *token){
    int i = 0;
    while(i < count){
        if(strcmp(strings[i], token)==0)
            return i;
        i++;
    }
    strings = realloc(strings, sizeof(char *)*++count);
    strings[count-1] = token;
    return count - 1;
}

static void printOperand(Operand op){
    printf("  0x%2x  |  %4s  |", op.mode, modeNames[op.mode - 0x20]);
    switch(op.mode){
        case VARIABLE: 
            printf(" (str%3d) |", getCount(op.data.name));
            break;
        case DIRECT:
            printf(" %8x |", op.data.mema);
            break;
        case REGISTER:
            printf(" %8x |", op.data.rega);
            break;
        case IMMEDIATES:
            printf(" (str%3d) |", getCount(op.data.ims));
            break;
        default: 
            printf(" %8x |", op.data.imv);
            break;
    }
}

static void printNull(){
    printf("   --   |   --   |    --    |");
}

static void printOperands(Operand *op, uint8_t insMode){
    int l = insMode - 0x30, i = 0;
    while(i < l){
        printOperand(op[i]);
        i++;
    }
    while(i < 3){
        printNull();
        i++;
    }
}

void analyze(Instructions *ins){
    uint16_t length = ins->noi, i = 0;
    Instruction *codes = ins->instructions;
    printf("\n-----------------------------------------------------------------------------------------------------------------------------------");
    printf("\n|  Count  |                                                      Instruction                                                      |");
    printf("\n|         |-----------------------------------------------------------------------------------------------------------------------|");
    printf("\n|         |     Operation   |    Format    |                                      Operand(s)                                      |");
    printf("\n|         |-----------------|--------------|--------------------------------------------------------------------------------------|");
    printf("\n|         |  Code  |  Name  | Code |  Type |         Operand 1          |         Operand 2          |         Operand 3          |");
    printf("\n|         |--------|--------|------|-------|----------------------------|----------------------------|----------------------------|");
    printf("\n|         |        |        |      |       |    Add. Mode    | HexValue |    Add. Mode    | HexValue |    Add. Mode    | HexValue |");
    printf("\n|         |        |        |      |       |-----------------|----------|-----------------|----------|-----------------|----------|");
    printf("\n|         |        |        |      |       |  Code  |  Name  |          |  Code  |  Name  |          |  Code  |  Name  |          |");
    while(i<length){
        Instruction in = codes[i];
        printf("\n|---------|--------|--------|------|-------|--------|--------|----------|--------|--------|----------|--------|--------|----------|");
        printf("\n|  %5u  |  0x%2x  | %6s | 0x%2x | %5s |"
                , (i+1), in.opcode, insNames[in.opcode - 0xA0], in.format, formatNames[in.format - 0x30]);
        Operand *op = in.operands;
        printOperands(op, in.format);
        i++;
    };
    int ic = 0;
    while(ic<count){
        printf("\n|---------------------------------------------------------------------------------------------------------------------------------|");
        printf("\n|  str%3d : %117s |", ic, strings[ic]);
        ic++;
    }
    printf("\n-----------------------------------------------------------------------------------------------------------------------------------");
}
