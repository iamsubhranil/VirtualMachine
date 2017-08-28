#include"function.h"
#include"utility.h"
#include<stdio.h>

void incr(Machine *m, Operands op) {
    uint32_t val;
    Operand op1 = op.onea.op1;
    Data d1 = op1.data;
    switch (op1.mode) {
        case REGISTER:
            m->registers[d1.rega] += 1;
            break;
        case DIRECT:
            val = readData(m, d1.mema) + 1;
            writeData(m, d1.mema, val);
            break;
        case VARIABLE: {
            uint16_t add = getAddress(m, d1.name);
            val = readData(m, add) + 1;
            writeData(m, add, val);
            break;
        }
        case IMMEDIATE:
            break; // TODO: Handle error
    }
}

void decr(Machine *m, Operands op) {
    uint32_t val;
    Operand op1 = op.onea.op1;
    Data d1 = op1.data;
    switch (op1.mode) {
        case REGISTER:
            m->registers[d1.rega] -= 1;
            break;
        case DIRECT:
            val = readData(m, d1.mema) - 1;
            writeData(m, d1.mema, val);
            break;
        case VARIABLE: {
            uint16_t add = getAddress(m, d1.name);
            val = readData(m, add) - 1;
            writeData(m, add, val);
            break;

        }
        case IMMEDIATE:
            break; // TODO: Handle error
    }
}

static uint32_t getVal(Operand o, Machine *m) {
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

void let(Machine *m, Operands op) {
    uint32_t val = getVal(op.twoa.op1, m);
    Operand op2 = op.twoa.op2;
    Data d2 = op2.data;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] = val;
            break;
        case DIRECT:
            writeData(m, d2.mema, val);
            break;
        case VARIABLE: {
            uint16_t add = getAddress(m, d2.name);
            writeData(m, add, val);
            break;
        }
        case IMMEDIATE:
            break; // TODO: Handle error
    }
}

void load(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    Data d1 = op1.data;
    Data d2 = op.twoa.op2.data;
    switch (op1.mode) {
        case REGISTER:
            m->registers[d2.rega] = m->registers[d1.rega];
            break;
        case DIRECT:
            m->registers[d2.rega] = readData(m, d1.mema);
            break;
        case VARIABLE:
            m->registers[d2.rega] = readData(m, getAddress(m, d1.name));
            //printf("\n[LOAD] Load complete to reg%u of val %u!\n", d2.rega, m->registers[d2.rega]);
            //printf("\n[LOAD] Expected : %u from address : %u\n", readData(m, getAddress(m, d1.name)), getAddress(m, d1.name));
            break;
        case IMMEDIATE:
            break; // TODO: Handle error
    }
}

void store(Machine *m, Operands op) {
    Data d1 = op.twoa.op1.data;
    Data d2 = op.twoa.op2.data;
    Operand op2 = op.twoa.op2;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] = m->registers[d1.rega];
            break;
        case DIRECT:
            writeData(m, d2.mema, m->registers[d1.rega]);
            break;
        case VARIABLE:
            writeData(m, getAddress(m, d2.name), m->registers[d1.rega]);
            break;
        case IMMEDIATE:
            break; // TODO: Handle error
    }
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

void print(Machine *m, Operands op) {
    Operand op1 = op.onea.op1;
    Data d1 = op1.data;
    switch (op1.mode) {
        case IMMEDIATE:
            printf("%u", d1.imv);
            break;
        case REGISTER: //printf("\n[REG] %u", d1.rega);
            printf("%u", m->registers[d1.rega]);
            // printf("\n[PRINT] Printed from reg%u", d1.rega);
            break;
        case DIRECT:
            printf("%u", readData(m, d1.mema));
            break;
        case VARIABLE:
            printf("%u", readData(m, getAddress(m, d1.name)));
            break;
    }
    printf("\n");
}

void add(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    Operand op2 = op.twoa.op2;
    Data d2 = op2.data;
    uint32_t readval = getVal(op1, m), temp = 0;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] += readval;
            break;
        case DIRECT:
            temp = readData(m, d2.mema);
            writeData(m, d2.mema, temp + readval);
            break;
        case VARIABLE:
            temp = readData(m, getAddress(m, d2.name));
            writeData(m, getAddress(m, d2.name), temp + readval);
            break;
    }
}

void sub(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    Operand op2 = op.twoa.op2;
    Data d2 = op2.data;
    uint32_t readval = getVal(op1, m), temp = 0;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] -= readval;
            break;
        case DIRECT:
            temp = readData(m, d2.mema);
            writeData(m, d2.mema, temp - readval);
            break;
        case VARIABLE:
            temp = readData(m, getAddress(m, d2.name));
            writeData(m, getAddress(m, d2.name), temp - readval);
            break;
    }
}

void mul(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    Operand op2 = op.twoa.op2;
    Data d2 = op2.data;
    uint32_t readval = getVal(op1, m), temp = 0;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] *= readval;
            break;
        case DIRECT:
            temp = readData(m, d2.mema);
            writeData(m, d2.mema, temp * readval);
            break;
        case VARIABLE:
            temp = readData(m, getAddress(m, d2.name));
            writeData(m, getAddress(m, d2.name), temp * readval);
            break;
    }
}

void div(Machine *m, Operands op) {
    Operand op1 = op.twoa.op1;
    Operand op2 = op.twoa.op2;
    Data d2 = op2.data;
    uint32_t readval = getVal(op1, m), temp = 0;
    switch (op2.mode) {
        case REGISTER:
            m->registers[d2.rega] /= readval;
            break;
        case DIRECT:
            temp = readData(m, d2.mema);
            writeData(m, d2.mema, temp / readval);
            break;
        case VARIABLE:
            temp = readData(m, getAddress(m, d2.name));
            writeData(m, getAddress(m, d2.name), temp / readval);
            break;
    }
}

void setl(Machine *m, Operands op) {
    Operand op1 = op.onea.op1;
    switch (op1.mode) {
        case DIRECT:
            writeData(m, op1.data.mema, m->pc + 1);
            break;
        case VARIABLE: {
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

static char *formatString(char *input){
    char *buffer = NULL;
    size_t dummy = 0, len = strlen(input), i = 0;
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

void inpti(Machine *m, Operands op){
    char *prompt = formatString(op.twoa.op1.data.ims);
    uint32_t input;
    printf("%s", prompt);
    scanf("%u", &input);
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
