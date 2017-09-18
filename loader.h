#ifndef LOADER_H
#define LOADER_H

#include"instruction.h"

Instructions * loadBinary(char *fileName, int *check);

Instructions * loadLibraries(char *libraries, int *check);

#endif
