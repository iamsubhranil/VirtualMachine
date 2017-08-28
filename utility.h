#ifndef UTILITY_H
#define UTILITY_H

#include<string.h>
#include<stdio.h>

char *addToBuffer(char *buffer, size_t *bufferSize, char toAdd);

size_t readline(char **buffer, FILE *fp);

char *stripFirst(char *val);

int digit(char c);

int alpha(char c);

int aldigit(char c);

size_t splitIntoArray(const char *input, char ***output, const char toSplit);
#endif
