#include"opcodes.h"
#include"operands.h"
#include"instruction.h"
#include"utility.h"
#include"names.h"

#include"parser.h"

#include<stdlib.h>

static int checkVariableName(char *val) {
    if (!alpha(val[0]))
        return 0;
    int i = 1;
    int last = strlen(val);
    while (i < last) {
        if (!(aldigit(val[i]) || val[i] == '_'))
            return 0;
        i++;
    }
    return 1;
}

static void getOperand(Operand *op, char *val, int *insert) {
    if (!*insert)
        return;
    uint8_t addressingMode = 0;
    if (val[0] == '@')
        addressingMode = DIRECT;
    else if (val[0] == '#')
        addressingMode = IMMEDIATE;
    else if (val[0] == 'R' || val[0] == 'r')
        addressingMode = REGISTER;
    else if (val[0] == '_')
        addressingMode = VARIABLE;
    else {
        printf("\n[PARSER:ERROR] Unknown addressing mode for operand %s!", val);
        *insert = 0;
        return;
    }
    val = stripFirst(val);
    char *rem;
    switch (addressingMode) {
        case VARIABLE:
            if (checkVariableName(val))
                op->data.name = strdup(val);
            else {
                printf("\n[PARSER:ERROR] Bad variable name %s!", val);
                *insert = 0;
            }
            break;
        case DIRECT:
            op->data.mema = strtoll(val, &rem, 10);
            if (strlen(rem)) {
                printf("\n[PARSER:ERROR] Direct addressing contains invalid part : %s", rem);
                *insert = 0;
            }
            break;
        case IMMEDIATE:
            op->data.imv = strtoll(val, &rem, 10);
            if (strlen(rem)) {
                printf("\n[PARSER:ERROR] Bad immediate value : %s", val);
                *insert = 0;
            }
            break;
        case REGISTER:
            op->data.rega = strtoll(val, &rem, 10);
            if (strlen(rem)) {
                printf("\n[PARSER:ERROR] Bad register address : %s", val);
                *insert = 0;
            }
            break;
        default:
            printf("\n[PARSER:ERROR] Unknown addressing mode : %s", val);
            *insert = 0;
            break;
    }
    free(val);
    op->mode = addressingMode;
}

static void checkOperand(Operand op, uint8_t operation, int opnum, int *insert) {
    if (*insert == 0)
        return;
    switch (operation) {
        case INCR:
        case DECR:
            *insert = (op.mode == VARIABLE || op.mode == REGISTER || op.mode == DIRECT);
            break;
        case UNLET:
            *insert = (op.mode == VARIABLE || op.mode == DIRECT);
            break;
        case LET:
            *insert = (opnum == 1 && op.mode != REGISTER) || (opnum == 2 && (op.mode == VARIABLE || op.mode == DIRECT));
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            *insert = (opnum == 1) || (opnum == 2 && (op.mode == VARIABLE || op.mode == REGISTER || op.mode == DIRECT));
            break;
        case LOAD:
            *insert = (opnum == 1 && op.mode != IMMEDIATE) || (opnum == 2 && op.mode == REGISTER);
            break;
        case STORE:
            *insert = (opnum == 1 && op.mode == REGISTER) || (opnum == 2 && (op.mode == VARIABLE || op.mode == DIRECT));
            break;
        case PRINT:
            *insert = op.mode == VARIABLE || op.mode == DIRECT || op.mode == REGISTER;
            break;
        case SETL:
            *insert = op.mode == VARIABLE || op.mode == DIRECT;
            break;
        case JNE:
        case JLT:
        case JGT:
            *insert = (opnum == 1 && (op.mode == VARIABLE || op.mode == DIRECT))
                      || (opnum == 2 && (op.mode == VARIABLE || op.mode == DIRECT || op.mode == REGISTER))
                      || (opnum == 3);
            break;
    }
    if (*insert == 0)
        printf("\n[PARSER:ERROR] Bad addressing mode %s for operand %d for instruction %s!", modeNames[op.mode - 0x20],
               opnum, insNames[operation - 0x10]);
}

static Instruction *newInstruction() {
    return (Instruction *) malloc(sizeof(Instruction));
}

Instructions * parseInput(char *filename, int *check) {
    FILE *fp = stdin;
    if (filename) {
        fp = fopen(filename, "rb");
        if (!fp) {
            printf("\n[ERROR] Unable to read file %s!", filename);
	    *check = 0;
	    fclose(fp);
            return 0;
        }
    }
    int insert = 1;
    char *buff = NULL;
    size_t size;
    char *token;
    uint16_t add = 0;
    Instructions *newIns = (Instructions *)malloc(sizeof(Instructions));
    newIns->instructions = NULL;
    newIns->noi = 0;
    while (insert) {
        if (fp == stdin)
            printf("\n > ");
        size = readline(&buff, fp);
        token = strtok(buff, " ");
	newIns->instructions = (Instruction *)realloc(newIns->instructions, ++add*sizeof(Instruction));
        Instruction *is = &(newIns->instructions[add-1]);
        uint8_t *op = &(is->opcode);
        uint8_t *format = &(is->format);
        Operands *os = &(is->operands);
        //printf("\n[INPUT] [%s]",token);
        if (strcmp(token, "let") == 0) {
            *op = LET;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "unlet") == 0) {
            *op = UNLET;
            *format = ONE_ADDRESS;
        } else if (strcmp(token, "incr") == 0) {
            *op = INCR;
            *format = ONE_ADDRESS;
        } else if (strcmp(token, "load") == 0) {
            *op = LOAD;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "store") == 0) {
            *op = STORE;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "decr") == 0) {
            *op = DECR;
            *format = ONE_ADDRESS;
        } else if (strcmp(token, "print") == 0) {
            *op = PRINT;
            *format = ONE_ADDRESS;
        } else if (strcmp(token, "halt") == 0) {
            *op = HALT;
            *format = ZERO_ADDRESS;
            //printf("\n[HALT] Entered!");
        } else if (strcmp(token, "add") == 0) {
            *op = ADD;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "sub") == 0) {
            *op = SUB;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "mul") == 0) {
            *op = MUL;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "div") == 0) {
            *op = DIV;
            *format = TWO_ADDRESS;
        } else if (strcmp(token, "setl") == 0) {
            *op = SETL;
            *format = ONE_ADDRESS;
        } else if (strcmp(token, "jlt") == 0) {
            *op = JLT;
            *format = THREE_ADDRESS;
        } else if (strcmp(token, "jgt") == 0) {
            *op = JGT;
            *format = THREE_ADDRESS;
        } else if (strcmp(token, "jne") == 0) {
            *op = JNE;
            *format = THREE_ADDRESS;
        } else {
            printf("\n[ERROR] Unknown operation %s", token);
            add--;
            continue;
        }
        switch (*format) {
            case ONE_ADDRESS: {
                token = strtok(NULL, " ");
                getOperand(&(os->onea.op1), token, &insert);
                checkOperand(os->onea.op1, *op, 1, &insert);
                *check = insert;
                break;
            }
            case TWO_ADDRESS: {
                Operand *op1 = &(os->twoa.op1);
                Operand *op2 = &(os->twoa.op2);
                token = strtok(NULL, " ");
                getOperand(op1, token, &insert);
                checkOperand(*op1, *op, 1, &insert);
                token = strtok(NULL, " ");
                getOperand(op2, token, &insert);
                checkOperand(*op2, *op, 2, &insert);
                *check = insert;
                break;
            }
            case THREE_ADDRESS: {
                Operand *op1 = &(os->threa.op1);
                Operand *op2 = &(os->threa.op2);
                Operand *op3 = &(os->threa.op3);

                token = strtok(NULL, " ");
                getOperand(op1, token, &insert);
                checkOperand(*op1, *op, 1, &insert);

                token = strtok(NULL, " ");
                getOperand(op2, token, &insert);
                checkOperand(*op2, *op, 2, &insert);

                token = strtok(NULL, " ");
                getOperand(op3, token, &insert);
                checkOperand(*op3, *op, 3, &insert);
		
		*check = insert;
                break;
            }
            case ZERO_ADDRESS: {
                os->zeroa.dummy = '0';
                break;
            }
        }
	free(buff);
        //printMem(m, add);
        if (*op == HALT)
            insert = 0;
    }
    fclose(fp);
    newIns->noi = add;
    return newIns;
}

