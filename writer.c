#include "writer.h"
#include "binfmt.h"
#include <string.h>
#include <stdio.h>

static void writeHeader(FILE *fp, uint16_t length, uint8_t isExecutable) {
    Header header = {MAGIC, VERSION, length, isExecutable};
    fwrite(&(header.magic), sizeof(uint32_t), 1, fp);
    fwrite(&(header.version), sizeof(uint8_t), 1, fp);
    fwrite(&(header.numIns), sizeof(uint16_t), 1, fp);
    fwrite(&(header.isExecutable), sizeof(uint8_t), 1, fp);
}

static void writeFooter(FILE *fp) {
    Footer footer = {OPTIMISED, sizeof(Instruction) & 0xFF};
    fwrite(&footer, sizeof(Footer), 1, fp);
}

static void writeString(char *string, FILE *fp){
    //printf("\n[Writer] Writing string %s!", string);
    uint8_t ln = strlen(string) + 1;
    fwrite(&ln, sizeof(uint8_t), 1, fp);
    int i = 0;
    while(i < ln){
        char c = string[i];
        fwrite(&c, sizeof(char), 1, fp);
        i++;
    }
}

static void writeOperand(Operand op, FILE *fp) {
    fwrite(&(op.mode), sizeof(uint8_t), 1, fp);
    Data d = op.data;
    //printf("\t%8x", op.mode);
    switch (op.mode) {
        case IMMEDIATE:
            fwrite(&(d.imv), sizeof(int32_t), 1, fp);
            //printf("\t%4x", d.imv);
            break;
        case REGISTER:
            fwrite(&(d.rega), sizeof(uint8_t), 1, fp);
            //printf("\t%4x", d.rega);
            break;
        case DIRECT:
            fwrite(&(d.mema), sizeof(uint16_t), 1, fp);
            //printf("\t%4x", d.mema);
            break;
        case IMMEDIATES:
            writeString(d.ims, fp);
            break;            
        case VARIABLE:
            writeString(d.name, fp);
            break;
    }
}

static void writeOperands(Instruction i, FILE *fp) {
    int l = i.format - 0x30, j = 0;
    while(j < l){
        writeOperand(i.operands[j], fp);
        j++;
    }
}

void writeBinary(Instructions *ins, char *filename, uint8_t isExecutable) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("\n[ERROR] Unable to open file %s!", filename);
        return;
    }
    uint16_t i = 0;
    uint16_t length = ins->noi;
    writeHeader(fp, length, isExecutable);
    //printf("\nOpcode\tFormat\tAddressingMode1\tValue1\tAddressingMode2\tValue2\tAddressingMode3\tValue3");
    //printf("\n======\t======\t===============\t======\t===============\t======\t===============\t======");
    while (i < length) {
        Instruction in = ins->instructions[i];
        //printf("\n%5x", in.opcode);
        fwrite(&(in.opcode), sizeof(uint8_t), 1, fp);
        //printf("\t%5x", in.format);
        fwrite(&(in.format), sizeof(uint8_t), 1, fp);
        writeOperands(in, fp);
        i++;
    }
    writeFooter(fp);
    fclose(fp);
}
