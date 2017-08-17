#ifndef FUNCTION_H
#define FUNCTION_H

#include"machine.h"
#include"operands.h"

typedef void (*function)(Machine *m, Operands op);

#define NUMFUNCS 12 // Update this counter whenever a new function is a added

void incr(Machine *m, Operands op);
void decr(Machine *m, Operands op);
void unlet(Machine *m, Operands op);
void let(Machine *m, Operands op);
void store(Machine *m, Operands op);
void load(Machine *m, Operands op);
void print(Machine *m, Operands op);
void halt(Machine *m, Operands op);
void add(Machine *m, Operands op);
void sub(Machine *m, Operands op);
void mul(Machine *m, Operands op);
void div(Machine *m, Operands op);
void setl(Machine *m, Operands op);

static function func[] = {incr, decr, unlet, print, load, store, let, halt, add, sub, mul, div, setl};

#endif
