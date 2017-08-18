#include"writer.h"
#include"binfmt.h"
#include<stdio.h>

static void convertVariableToDirect(Operand *a, Machine *m) {
    if (a->mode == VARIABLE) {
        a->mode = DIRECT;
        a->data.mema = getAddress(m, a->data.name);
    }
}

static void writeHeader(FILE *fp, uint16_t length) {
    Header header = {MAGIC, VERSION, length};
    fwrite(&(header.magic), sizeof(uint32_t), 1, fp);
    fwrite(&(header.version), sizeof(uint8_t), 1, fp);
    fwrite(&(header.numIns), sizeof(uint16_t), 1, fp);
}

static void writeFooter(FILE *fp) {
    Footer footer = {OPTIMISED, sizeof(Instruction) & 0xFF};
    fwrite(&footer, sizeof(Footer), 1, fp);
}

static void writeOperand(Operand op, FILE *fp) {
    fwrite(&(op.mode), sizeof(uint8_t), 1, fp);
    Data d = op.data;
    //printf("\t%8x", op.mode);
    switch (op.mode) {
        case IMMEDIATE:
            fwrite(&(d.imv), sizeof(uint32_t), 1, fp);
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
        case VARIABLE:
            break;
    }
}

static void writeOperands(Instruction i, FILE *fp) {
    switch (i.format) {
        case ZERO_ADDRESS:
            break;
        case ONE_ADDRESS:
            writeOperand(i.operands.onea.op1, fp);
            break;
        case TWO_ADDRESS:
            writeOperand(i.operands.twoa.op1, fp);
            writeOperand(i.operands.twoa.op2, fp);
            break;
        case THREE_ADDRESS:
            writeOperand(i.operands.threa.op1, fp);
            writeOperand(i.operands.threa.op2, fp);
            writeOperand(i.operands.threa.op3, fp);
            break;
    }
}

void writeBinary(Instruction ins[], uint16_t length, Machine *m, char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("\n[ERROR] Unable to open file %s!", filename);
        return;
    }
    uint16_t i = 0;
    writeHeader(fp, length);
    //printf("\nOpcode\tFormat\tAddressingMode1\tValue1\tAddressingMode2\tValue2\tAddressingMode3\tValue3");
    //printf("\n======\t======\t===============\t======\t===============\t======\t===============\t======");
    while (i < length) {
        Instruction in = ins[i];
        switch (in.format) {
            case ZERO_ADDRESS:
                break;
            case ONE_ADDRESS:
                convertVariableToDirect(&in.operands.onea.op1, m);
                break;
            case TWO_ADDRESS:
                convertVariableToDirect(&in.operands.twoa.op1, m);
                convertVariableToDirect(&in.operands.twoa.op2, m);
                break;
            case THREE_ADDRESS:
                convertVariableToDirect(&in.operands.threa.op1, m);
                convertVariableToDirect(&in.operands.threa.op2, m);
                convertVariableToDirect(&in.operands.threa.op3, m);
                break;
        }
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
