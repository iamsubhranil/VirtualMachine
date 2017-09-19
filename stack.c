#include <stdint.h>
#include <stdlib.h>

#include "stack.h"

CallStack * cstack_new(){
    CallStack *ret = (CallStack *)malloc(sizeof(CallStack));
    if(!ret)
        return NULL;
    ret->top = NULL;
    return ret;
}

static CStack * val_new(uint16_t value){
    CStack *top = (CStack *)malloc(sizeof(CStack));
    top->address = value;
    top->next = NULL;
    return top;
}

void cstack_push(CallStack *stack, uint16_t value){
    CStack *ntop = val_new(value);
    ntop->next = stack->top;
    stack->top = ntop;
}

uint16_t cstack_pop(CallStack *stack){
    if(stack->top == NULL)
        return 0;
    uint16_t bak = stack->top->address;
    CStack *old = stack->top;
    stack->top = stack->top->next;
    free(old);
    return bak;
}

uint16_t cstack_peek(CallStack *stack){
    if(stack->top == NULL)
        return 0;
    return stack->top->address;
}
