#include "function.h"
#include "utility.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>


static CallStack *stack = NULL;

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

void incr(Machine *m, Operand * op) {
    int32_t val = getVal(op[0], m);
    putVal(op[0], m, val+1);
}

void decr(Machine *m, Operand * op) {
    int32_t val = getVal(op[0], m);
    putVal(op[0], m, val - 1); 
}

void let(Machine *m, Operand * op) {
    int32_t val = getVal(op[0], m);
    Operand op2 = op[1];
    putVal(op2, m, val);
}

void load(Machine *m, Operand * op) {
    Operand op1 = op[0];
    putVal(op[1], m, getVal(op1, m));
}

void store(Machine *m, Operand * op) {
    Operand op2 = op[1];
    putVal(op2, m, m->registers[op[0].data.rega]);
}

void halt(Machine *m, Operand * op) {
    m->halt = 1;
}

void unlet(Machine *m, Operand * op) {
    Operand op1 = op[0];
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

void add(Machine *m, Operand * op) {
    Operand op1 = op[0];
    Operand op2 = op[1];
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op[2], m, readval1 + readval2);
}

void sub(Machine *m, Operand * op) {
    Operand op1 = op[0];
    Operand op2 = op[1];
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m); 
    putVal(op[2], m, readval1 - readval2);
}

void mul(Machine *m, Operand * op) {
    Operand op1 = op[0];
    Operand op2 = op[1];
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op[2], m, readval1 * readval2);
}

void divd(Machine *m, Operand * op) {
    Operand op1 = op[0];
    Operand op2 = op[1];
    int32_t readval1 = getVal(op1, m), readval2 = getVal(op2, m);
    putVal(op[2], m, readval1 / readval2);
}

void setl(Machine *m, Operand * op) {
    Operand op1 = op[0];
    switch (op1.mode) {
        case DIRECT:
            writeData(m, op1.data.mema, m->pc + 1);
            break;
        case VARIABLE: {
            //printf("\n[Setl] Setting label %s to %u", op1.data.name, m->pc+1);
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

void jne(Machine *m, Operand * op) {
    uint16_t address = jmpAddress(m, op[0]);
    Operand source = op[1];
    Operand dest = op[2];
    if (getVal(source, m) != getVal(dest, m) && address!=0)
        m->pc = address;
}

void jlt(Machine *m, Operand * op) {
    uint16_t address = jmpAddress(m, op[0]);
    Operand source = op[1];
    Operand dest = op[2];
    if (getVal(source, m) < getVal(dest, m) && address!=0)
        m->pc = address;
}

void jgt(Machine *m, Operand * op) {
    uint16_t address = jmpAddress(m, op[0]);
    Operand source = op[1];
    Operand dest = op[2];
    if (getVal(source, m) > getVal(dest, m) && address!=0)
        m->pc = address;
}

void jmp(Machine *m, Operand * op) {
    uint16_t address = jmpAddress(m, op[0]);
    if (address!=0)
        m->pc = address;
}

void jeq(Machine *m, Operand * op) {
    uint16_t address = jmpAddress(m, op[0]);
    Operand source = op[1];
    Operand dest = op[2];
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

void print(Machine *m, Operand * op) {
    Data printData = op[0].data;
    char *printString = formatString(printData.ims);
    char *args = op[1].data.ims;
    char **arguments = NULL;
    size_t noa = 0, i = 0;
    noa = splitIntoArray(args, &arguments, ',');
    Operand *operands = (Operand *)malloc(sizeof(Operand)*noa);
    int check = 1;
    while(i < noa){
        operands = getOperand(operands, i, arguments[i], &check);
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

void inpti(Machine *m, Operand * op){
    char *prompt = formatString(op[0].data.ims);
    int32_t input;
    printf("%s", prompt);
    scanf("%d", &input);
    Operand dest = op[1];
    if(dest.mode == REGISTER)
        m->registers[dest.data.rega] = input;
    else if(dest.mode == DIRECT)
        writeData(m, dest.data.mema, input);
    else if(dest.mode == VARIABLE)
        writeData(m, getAddress(m, dest.data.name), input);
}

void inpts(Machine *m, Operand * op){
    printf("\n[INPTS:ERROR] inpts is not defined yet!");
}

void prntl(Machine *m, Operand * op){
    print(m, op);
    printf("\n");
}

void prompt(Machine *m, Operand * op){
    char *prompt = formatString(op[0].data.ims);
    printf("%s", prompt);
}

void prmptl(Machine *m, Operand * op){
    prompt(m, op);
    printf("\n");
}

void mod(Machine *m, Operand * op){
    int32_t val1 = getVal(op[0], m), val2 = getVal(op[1], m);
    putVal(op[2], m, val1 % val2);
}

char * getstr(char *p, char *q){
    size_t size1 = strlen(p);
    size_t size2 = strlen(q);
   
    char *ret = (char *)malloc(size1 + size2 + 1);
    strcpy(ret, p);
    strcat(ret, q);
    free(p);
    free(q);
    return ret;
}

void def(Machine *m, Operand * op){
    char *fName = op[0].data.name;
    Operand *label = (Operand *)malloc(sizeof(Operand));
    label->mode = VARIABLE;
    label->data.name = getstr(strdup("__start__def__"), strdup(fName));
    //printf("\n[Def] Defined as [%s] Original [%s]!", label->data.name, op[0].data.name);
    setl(m, label);
    //free(label->data.name);
    free(label);
}

void call(Machine *m, Operand * op){
    char *args = op[1].data.ims;
    char *fName = op[0].data.name;
    if(!symtLookup(m, getstr(strdup("__start__def__"), strdup(fName)))){
        printf("\n[ERROR] %s is not defined!", fName);
        m->halt = 1;
        return;
    }
    //printf("\n[Call] Fname is %s!", fName);
    char **arglist = NULL;
    size_t length = splitIntoArray(args, &arglist, ',');
    size_t i = 0;
    int insert = 1;
    Operand * ops = (Operand *)malloc(sizeof(Operand) * 2);
    
    if(stack == NULL)
        stack = cstack_new();
    cstack_push(stack, m->pc + 1);
    //ops[0].mode = VARIABLE;
    //ops[0].data.name = getstr(strdup("__end__def__"), strdup(fName));
    //setl(m, ops);
    //free(ops->data.name);
    while(i < length){
        ops = getOperand(ops, 0, arglist[i], &insert);
        if(!insert){
            printf("[ERROR] Bad argument %s!", arglist[i]);
            i++;
            continue;
        }
        else if(ops->mode == IMMEDIATES){
            printf("[ERROR] Unable to pass immediate string [%s] as argument!", arglist[i]);
            i++;
            continue;
        }
        int32_t val = getVal(*ops, m);
        ops[0].mode = IMMEDIATE;
        ops[0].data.imv = val;
        ops[1].mode = VARIABLE;
        char toString[5];
        sprintf(toString, "%d", (int)i);
        ops[1].data.name = getstr(strdup("arg"), strdup(toString));
        let(m, ops);
        i++;
    }
    ops[0].mode = VARIABLE;
    ops[0].data.name = getstr(strdup("__start__def__"), strdup(fName));
    m->pc = jmpAddress(m, ops[0]);
    free(ops);
}

void enddef(Machine *m, Operand * op){
    if(strcmp(op[0].data.name, "main") == 0)
        halt(m, op);
    else{
        //char *fName = strdup(op[0].data.name);
        //Operand tmp;
        //tmp.mode = VARIABLE;
        //tmp.data.name = getstr(strdup("__end__def__"), fName);
        //printf("\n[Enddef] Jumping to %s..", tmp.data.name);
        m->pc = cstack_pop(stack);
    }
}
