#ifndef FUNCTION_H
#define FUNCTION_H

#include "machine.h"
#include "operands.h"

typedef void (*function)(Machine *m, Operand * op);

void incr(Machine *m, Operand * op);

void decr(Machine *m, Operand * op);

void unlet(Machine *m, Operand * op);

void let(Machine *m, Operand * op);

void store(Machine *m, Operand * op);

void load(Machine *m, Operand * op);

void print(Machine *m, Operand * op);

void halt(Machine *m, Operand * op);

void add(Machine *m, Operand * op);

void sub(Machine *m, Operand * op);

void mul(Machine *m, Operand * op);

void divd(Machine *m, Operand * op);

void setl(Machine *m, Operand * op);

void jne(Machine *m, Operand * op);

void jlt(Machine *m, Operand * op);

void jgt(Machine *m, Operand * op);

void prntl(Machine *m, Operand * op);

void inpti(Machine *m, Operand * op);

void inpts(Machine *m, Operand * op);

void prompt(Machine *m, Operand * op);

void prmptl(Machine *m, Operand * op);

void mod(Machine *m, Operand * op);

void def(Machine *m, Operand * op);

void call(Machine *m, Operand * op);

void enddef(Machine *m, Operand * op);

void jeq(Machine *m, Operand * op);

void jmp(Machine *m, Operand * op);

static function func[] = {incr, decr, unlet, print, load, store, let, halt, add, sub, mul, divd, setl, jne, jlt, jgt, prntl, inpti, inpts,
                            prompt, prmptl, mod, def, call, enddef, jeq, jmp};

#endif
