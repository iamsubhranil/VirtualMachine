#ifndef CALLSTACK_H
#define CALLSTACK_H

#include <stdint.h>

typedef struct CStack{
    uint16_t address;
    struct CStack *next;
} CStack;

typedef struct CallStack{
    CStack *top;
} CallStack;

CallStack * cstack_new();
void cstack_push(CallStack *stack, uint16_t val);
uint16_t cstack_pop(CallStack *stack);
uint16_t cstack_peek(CallStack *stack);

#endif
