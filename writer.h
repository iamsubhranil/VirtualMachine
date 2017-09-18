#ifndef WRITER_H
#define WRITER_H

#include "instruction.h"
#include <stdint.h>

void writeBinary(Instructions *ins, char *filename, uint8_t isExecutable, char *libraries);

#endif
