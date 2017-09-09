#include"loader.h"
#include"binfmt.h"
#include"names.h"
#include<stdio.h>
#include<malloc.h>

static char * readString(FILE *fp){
    uint8_t len = 0;
    fread(&len, sizeof(uint8_t), 1, fp);
    char *buffer = malloc(sizeof(char)*len);
    int i = 0;
    while(i<len){
        fread(&(buffer[i]), sizeof(char), 1, fp);
        i++;
    }
    return buffer;
}

static void readOperand(Operand *op, FILE *fp) {
    fread(&(op->mode), sizeof(uint8_t), 1, fp);
    switch (op->mode) {
        case IMMEDIATE:
            fread(&(op->data.imv), sizeof(int32_t), 1, fp);
            break;
        case REGISTER:
            fread(&(op->data.rega), sizeof(uint8_t), 1, fp);
            break;
        case DIRECT:
            fread(&(op->data.mema), sizeof(uint16_t), 1, fp);
            break;
        case IMMEDIATES:
            op->data.ims = readString(fp);
            break;
        case VARIABLE:
            op->data.name = readString(fp);
            break;
    }
}

Instructions * loadBinary(char *filename, int *check) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("\n[ERROR] Unable to open file %s!\n", filename);
        *check = 0;
        return NULL;
    }
    Header h;
    Instructions *inss = (Instructions *)malloc(sizeof(Instructions));
    fread(&(h.magic), sizeof(uint32_t), 1, fp);
    fread(&(h.version), sizeof(uint8_t), 1, fp);
    fread(&(h.numIns), sizeof(uint16_t), 1, fp);
    if (h.magic == MAGIC) {
        printf("\n[LOADER] Magic matched");
        if (h.version == VERSION) {
            printf("\n[LOADER] Version matched\n[LOADER] Instructions : %u", h.numIns);

            uint16_t i = 0;
            inss->noi = h.numIns;
            inss->instructions = (Instruction *)malloc(sizeof(Instruction)*h.numIns);
            while (i < h.numIns) {
                fread(&(inss->instructions[i].opcode), sizeof(uint8_t), 1, fp);
                fread(&(inss->instructions[i].format), sizeof(uint8_t), 1, fp);
                switch (inss->instructions[i].format) {
                    case ZERO_ADDRESS:
                        break;
                    case ONE_ADDRESS:
                        readOperand(&(inss->instructions[i].operands.onea.op1), fp);
                        break;
                    case TWO_ADDRESS:
                        readOperand(&(inss->instructions[i].operands.twoa.op1), fp);
                        readOperand(&(inss->instructions[i].operands.twoa.op2), fp);
                        break;
                    case THREE_ADDRESS:
                        readOperand(&(inss->instructions[i].operands.threa.op1), fp);
                        readOperand(&(inss->instructions[i].operands.threa.op2), fp);
                        readOperand(&(inss->instructions[i].operands.threa.op3), fp);
                        break;
                }
                i++;
            }
            Footer f;
            fread(&f, sizeof(Footer), 1, fp);
            //printf("\n[LOADER] Expected file size : %lu", f.expectedSize);
            printf("\n[LOADER] Instruction length : %u bytes", f.instructionLength);
            if (f.instructionLength != sizeof(Instruction)) {
                printf("\n[LOADER:WARNING] Instruction length mismatch! The binary may be corrupted!");
            }
            printf("\n[LOADER] Binary format : %s\n", binaryFormat[f.format - 0x40]);
            fclose(fp);
            return inss;
        } else {
            printf("\n[ERROR] Binary version incompatible!");
            *check = 0;
        }
    } else {
        printf("\n[ERROR] Magic not matched! This is not a valid executable file!");
        *check = 0;
    }
    fclose(fp);
    return NULL;
}
