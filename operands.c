#include "operands.h"
#include "utility.h"
#include "opcodes.h"
#include "names.h"
#include <stdlib.h>

static int checkVariableName(char *val) {
    int i = 1;
    int last = strlen(val);
    while (i < last) {
        if (aldigit(val[i])==0 && val[i]!='_')
            return 0;
        i++;
    }
    return 1;
}

void getOperand(Operand *op, char *val, int *insert) {
    if (!*insert)
        return;
    if(val == NULL){
        op->mode = 0;
        return;
    }

    uint8_t addressingMode = 0;
    if (val[0] == '@')
        addressingMode = DIRECT;
    else if (val[0] == '#')
        addressingMode = IMMEDIATE;
    else if(val[0] == '^')
        addressingMode = IMMEDIATES;
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
        case IMMEDIATES:
            op->data.ims = strdup(val);
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
