#include "opcodes.h"
#include "operands.h"
#include "floader.h"
#include "instruction.h"
#include "utility.h"
#include "names.h"
#include "print.h"
#include "parser.h"

#include <stdlib.h>

static Instruction *newInstruction() {
    return (Instruction *) malloc(sizeof(Instruction));
}

Instructions * parseInput(char *filename, int *check, int isExecutable) {
    FILE *fp = stdin;
    if (filename) {
        fp = fopen(filename, "rb");
        if (!fp) {
            printf("\n[ERROR] Unable to read file %s!", filename);
            *check = 0;
            return 0;
        }
    }
    loadFunctions(check);
    if(!(*check)){
        if(fp!=stdin)
            fclose(fp);
        return NULL;
    }
    int insert = 1;
    char *buff = NULL;
    size_t size;
    char *token;
    uint16_t add = 0;
    uint8_t lastins = 0;
    Instructions *newIns = (Instructions *)malloc(sizeof(Instructions));
    newIns->instructions = NULL;
    newIns->noi = 0;
    char *defName = NULL; // Present definition name
    int maind = 0, mainf = 0; // Check if main is defined
    Function *def = getFunction("def"), *enddef = getFunction("enddef");
    while (insert) {
        if (fp == stdin)
            printf("\n > ");
        size = readline(&buff, fp);
        if(size > 0 && (buff[0] == '#' || buff[0] == ' '))
            continue;
        token = strtok(buff, " ");
        if(token==NULL){
            if(!isExecutable)
                break;
            else
                token = strdup("halt");
        }
        newIns->instructions = (Instruction *)realloc(newIns->instructions, ++add*sizeof(Instruction));
        Instruction *is = &(newIns->instructions[add-1]);
        is->operands = NULL;
        Operand *operands = NULL;
        //printf("\n[INPUT] [%s]",token);
        Function *function = getFunction(token);
        if(function == NULL){
            printf("\n[PARSER:ERROR] Unknown function %s!", token);
            add--;
            continue;
        } 
        is->opcode = function->opcode;
        //printf("\n[INFO] Function : %s Opcode : 0x%x Format : 0x%x", function->invokation, function->opcode, function->format);
        is->format = function->format;
        operands = (Operand *)malloc(sizeof(Operand) * (is->format - 0x30));
        //printf("\nAllocated %u bytes for %d operands!\n", sizeof(Operand) * (is->format - 0x30),(is->format - 0x30));
        int i = 0;
        switch (is->format) {
            case ONE_ADDRESS: {
                                  token = strtok(NULL, " ");
                                  operands = getOperand(operands, 0, token, &insert);
                                  checkOperand(function, &(operands[0]), 1, &insert);

                                  if(insert && is->opcode == def->opcode){
                                      if(defName != NULL){
                                          printf("\n[PARSER:ERROR] Nested functions not supported (function %s inside %s)!", stripFirst(token), stripFirst(defName));
                                          insert = 0;
                                          break;
                                      }
                                      else{
                                          defName = strdup(token);
                                          if(strcmp(defName, "_main") == 0)
                                              maind = 1;
                                      }
                                  }
                                  else if(insert && is->opcode == enddef->opcode){
                                      if(defName == NULL || strcmp(defName, token)!=0){
                                          printf("\n[PARSER:ERROR] Bad `enddef`!");
                                          insert = 0;
                                          break;
                                      }
                                      else{
                                          if(strcmp(defName, "_main") == 0){
                                              mainf = 1;
                                              insert = 2;
                                          }
                                          free(defName);
                                          defName = NULL;
                                      }

                                  }

                                  *check = insert > 0;
                                  if(insert == 2)
                                      insert = 0;
                                  break;
                              }
            case TWO_ADDRESS: {
                                  while(i < 2){
                                      token = strtok(NULL, " ");
                                      operands = getOperand(operands, i, token, &insert);
                                      checkOperand(function, &(operands[i]), (i+1), &insert);
                                      i++;
                                  }
                                  *check = insert;
                                  break;
                              }
            case THREE_ADDRESS: {
                                    while(i < 3){
                                        token = strtok(NULL, " ");
                                        operands = getOperand(operands, i, token, &insert);
                                        checkOperand(function, &(operands[i]), (i+1), &insert);
                                        i++;
                                    }
                                    *check = insert;
                                    break;
                                }
            case ZERO_ADDRESS: {
                                   break;
                               }
        }
        lastins = is->opcode;
        is->operands = operands;
        free(buff);
        //printIns(*is);
        //printMem(m, add);
        if (is->opcode == HALT)
            insert = 0;
    }

    if(isExecutable){
        if(!maind){
            printf("\n[PARSER:ERROR] main is not defined!");
            *check = 0;
        }
        else if(!mainf){
            printf("\n[PARSER:ERROR] main is not enclosed!");
            *check = 0;
        }
    }
    else if(!isExecutable && maind){
        printf("\n[PARSER:ERROR] `main` shouldn't be defined on a library!");
        *check = 0;
    }

    if(fp!=stdin)
        fclose(fp);
    newIns->noi = add;
    return newIns;
}
