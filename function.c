#include"function.h"
#include"utility.h"
#include<stdio.h>
#include<stdlib.h>

static int32_t getVal(Operand o, Machine *m) {
    Data d1 = o.data;
    switch (o.mode) {
        case IMMEDIATE:
            return d1.imv;
            break;
        case REGISTER:
            return m->registers[d1.rega];
            break;
        case DIRECT:
            return readData(m, d1.mema);
            break;
        case VARIABLE:
            return readData(m, getAddress(m, d1.name));
            break;
    }
    return 0;
}

static void putVal(Operand o, Machine *m, int32_t val){
    Data d1 = o.data;
    switch(o.mode){
        case REGISTER:
            m->registers[d1.rega] = val;
            break;
        case DIRECT:
            writeData(m, d1.mema, val);
            break;
        case VARIABLE:
            writeData(m, getAddress(m, d1.name), val);
            break;
    }
}

void incr(Machine *m, Operands op) {
    int32_t val = getVal(op.onea.op1, m);
    putVal(op.onea.op1, m, val+1);
}

void decr(Machine *m, Operands op) {
    int32_t val = getVal(op.onea.op1, m);
    putVal(op.onea.op1, m, val - 1); 
}

void let(Machine *m, Operands op) {
    int32_t val = getVal(op.twoa.op1, m);
    Operand op2 = op.twoa.op2;
    putVal(op2, m, val);
}

void load(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    putVal(op.twoa.op2, m, getVal(op1, m));
}

void store(Machine *m, Operands op) {
    Operand op2 = op.twoa.op2;
    putVal(op2, m, m->registers[op.twoa.op1.data.rega]);
}

void halt(Machine *m, Operands op) {
    op.zeroa.dummy = 'a'; // Stop the warning
    m->halt = 1;
}

void unlet(Machine *m, Operands op) {
    Operand op1 = op.onea.op1;
    Data d1 = op1.data;
    switch (op1.mode) {
        case IMMEDIATE:
            break;
        case REGISTER:
            break;
        case VARIABLE:
            deallocate(m, d1.name);
            break;
        case DIRECT:
            writeData(m, d1.mema, 0);
            break;
    }
}

void add(Machine *m, Operands op) {
    Operand op1 = op.threa.op1;
    Operand op2 = op.threa.op2;
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op.threa.op3, m, readval1 + readval2);
}

void sub(Machine *m, Operands op) {
    Operand op1 = op.threa.op1;
    Operand op2 = op.threa.op2;
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m); 
    putVal(op.threa.op3, m, readval1 - readval2);
}

void mul(Machine *m, Operands op) {
    Operand op1 = op.threa.op1;
    Operand op2 = op.threa.op2;
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op.threa.op3, m, readval1 * readval2);
}

void divd(Machine *m, Operands op) {
    Operand op1 = op.threa.op1;
    Operand op2 = op.threa.op2;
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op.threa.op3, m, readval1 / readval2);
}

void setl(Machine *m, Operands op) {
    Operand op1 = op.onea.op1;
    switch (op1.mode) {
        case DIRECT:
            writeData(m, op1.data.mema, m->pc + 1);
            break;
        case VARIABLE: {
            //               printf("\nSetting label %s to %u", op1.data.name, m->pc+1);
            uint16_t ad = getAddress(m, op1.data.name);
            writeData(m, ad, m->pc + 1);
        }
            break;
    }
}

static uint16_t jmpAddress(Machine *m, Operand label) {
    uint16_t address;
    switch (label.mode) {
        case VARIABLE:
            address = getAddress(m, label.data.name);
            address = readData(m, address);
            break;
        case DIRECT:
            address = readData(m, label.data.mema);
            break;
    }
    if(address==0)
	    printf("\n[JUMP:WARNING] Trying to jump to undefined label!");
    return address;
}

void jne(Machine *m, Operands op) {
    uint16_t address = jmpAddress(m, op.threa.op1);
    Operand source = op.threa.op2;
    Operand dest = op.threa.op3;
    if (getVal(source, m) != getVal(dest, m) && address!=0)
        m->pc = address;
}

void jlt(Machine *m, Operands op) {
    uint16_t address = jmpAddress(m, op.threa.op1);
    Operand source = op.threa.op2;
    Operand dest = op.threa.op3;
    if (getVal(source, m) < getVal(dest, m) && address!=0)
        m->pc = address;
}

void jgt(Machine *m, Operands op) {
    uint16_t address = jmpAddress(m, op.threa.op1);
    Operand source = op.threa.op2;
    Operand dest = op.threa.op3;
    if (getVal(source, m) > getVal(dest, m) && address!=0)
        m->pc = address;
}

void jmp(Machine *m, Operands op) {
    uint16_t address = jmpAddress(m, op.threa.op1);
    if (address!=0)
        m->pc = address;
}

void jeq(Machine *m, Operands op) {
    uint16_t address = jmpAddress(m, op.threa.op1);
    Operand source = op.threa.op2;
    Operand dest = op.threa.op3;
    if (getVal(source, m) == getVal(dest, m) && address!=0)
        m->pc = address;
}

static char *formatString(char *input){
    char *buffer = NULL;
    size_t dummy = 0, len = strlen(input), i = 0;
    if(strcmp(input, "(noprompt)") == 0){
        buffer = addToBuffer(buffer, &dummy, ' ');
        buffer = addToBuffer(buffer, &dummy, '\0');
        return buffer;
    }
    while(i < len){
        char p = input[i];
        if(p=='\\' && i < (len-1)){
            char n = input[++i];
            if(n=='s')
                buffer = addToBuffer(buffer, &dummy, ' ');
            else if(n=='n')
                buffer = addToBuffer(buffer, &dummy, '\n');
            else{
                buffer = addToBuffer(buffer, &dummy, p);
                buffer = addToBuffer(buffer, &dummy, n);
            }
        }
        else
            buffer = addToBuffer(buffer, &dummy, p);
        i++;
    }
    buffer = addToBuffer(buffer, &dummy, '\0');
    return buffer;
}

void print(Machine *m, Operands op) {
    Data printData = op.twoa.op1.data;
    char *printString = formatString(printData.ims);
    char *args = op.twoa.op2.data.ims;
    char **arguments = NULL;
    size_t noa = 0, i = 0;
    noa = splitIntoArray(args, &arguments, ',');
    Operand *operands = (Operand *)malloc(sizeof(Operand)*noa);
    int check = 1;
    while(i < noa){
        getOperand(&operands[i], arguments[i], &check);
        if(!check){
            free(operands);
            printf("\n[ERROR] Print failed!");
            break;
        }
        i++;
    }
    i = 0;
    size_t count = 0;
    size_t size = strlen(printString);
    while(i < size){
        char toPrint = printString[i];
        if(toPrint == '%' && (i < size-1)){
            char next = printString[++i];
            if(next == 'd'){
                printf("%d", getVal(operands[count], m));
                count++;
                if(count > noa){
                    printf("\n[ERROR] Less arguments in print!");
                    break;
                }
            }
            else{
                printf("%c%c", toPrint, next);
            }
        }
        else
            printf("%c", toPrint);
        i++;
    }
    free(operands);
}

void inpti(Machine *m, Operands op){
    char *prompt = formatString(op.twoa.op1.data.ims);
    int32_t input;
    printf("%s", prompt);
    scanf("%d", &input);
    Operand dest = op.twoa.op2;
    if(dest.mode == REGISTER)
        m->registers[dest.data.rega] = input;
    else if(dest.mode == DIRECT)
        writeData(m, dest.data.mema, input);
    else if(dest.mode == VARIABLE)
        writeData(m, getAddress(m, dest.data.name), input);
}

void inpts(Machine *m, Operands op){
    printf("\n[INPTS:ERROR] inpts is not defined yet!");
}

void prntl(Machine *m, Operands op){
    print(m, op);
    printf("\n");
}

void prompt(Machine *m, Operands op){
    char *prompt = formatString(op.onea.op1.data.ims);
    printf("%s", prompt);
}

void prmptl(Machine *m, Operands op){
    prompt(m, op);
    printf("\n");
}

void mod(Machine *m, Operands op){
    int32_t val1 = getVal(op.threa.op1, m), val2 = getVal(op.threa.op2, m);
    putVal(op.threa.op3, m, val1 % val2);
}

void def(Machine *m, Operands op){
    char *fName = op.onea.op1.data.name;
    Operands *label = (Operands *)malloc(sizeof(Operands));
    label->onea.op1.mode = VARIABLE;
    label->onea.op1.data.name = strcat(strdup("__start__def__"), strdup(fName));
    setl(m, *label);
    free(label);
}

void call(Machine *m, Operands op){
    char *args = op.threa.op2.data.ims;
    char **arglist = NULL;
    size_t length = splitIntoArray(args, &arglist, ',');
    size_t i = 0;
    int insert = 1;
    Operands ops;
    ops.onea.op1.mode = VARIABLE;
    ops.onea.op1.data.name = strcat(strdup("__end__def__"),strdup(op.onea.op1.data.name));
    setl(m, ops);
    while(i < length){
        Operand op;
        getOperand(&op, arglist[i], &insert);
        if(!insert){
            printf("[ERROR] Bad argument %s!", arglist[i]);
            i++;
            continue;
        }
        else if(op.mode == IMMEDIATES){
            printf("[ERROR] Unable to pass immediate string [%s] as argument!", arglist[i]);
            i++;
            continue;
        }
        int32_t val = getVal(op, m);
        ops.twoa.op1.mode = IMMEDIATE;
        ops.twoa.op1.data.imv = val;
        ops.twoa.op2.mode = VARIABLE;
        char toString[5];
        sprintf(toString, "%d", (int)i);
        ops.twoa.op2.data.name = strcat(strdup("arg"), toString);
        let(m, ops);
        i++;
    }
    op.twoa.op1.data.name = strcat(strdup("__start__def__"), op.twoa.op1.data.name);
    m->pc = jmpAddress(m, op.twoa.op1);
}

void enddef(Machine *m, Operands op){
    if(strcmp(op.onea.op1.data.name, "main") == 0)
        halt(m, op);
    else{
        char *fName = strdup(op.onea.op1.data.name);
        op.onea.op1.mode = VARIABLE;
        op.onea.op1.data.name = strcat(strdup("__end__def__"), fName);
        m->pc = jmpAddress(m, op.onea.op1);
    }
}
