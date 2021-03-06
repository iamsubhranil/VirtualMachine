#ifndef NAMES_H
#define NAMES_H

static char *insNames[] = {"incr", "decr", "unlet", "print", "load", "store", "let", "halt", "add", "sub", "mul", "div",
                           "setl", "jne", "jlt", "jgt", "prntl", "inpti", "inpts", "prompt", "prmptl", "mod", "def", 
                           "call", "enddef", "jeq", "jmp"};
static char *formatNames[] = {"zeroa", "onea", "twoa", "threa"};
static char *modeNames[] = {"imm", "reg", "dir", "var", "ims"};
static char *modeNamesVerbose[] = {"immediate", "register", "direct", "variable", "immediate string"};
static char *binaryFormat[] = {"FLEXIBLE", "OPTIMISED"};
static char *cellNames[] = {"INSTRUCTION", "DATA"};

#endif
