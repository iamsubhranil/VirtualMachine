#include"opcodes.h"
#include"operands.h"
#include"floader.h"
#include"instruction.h"
#include"utility.h"
#include"names.h"

#include"parser.h"

#include<stdlib.h>

static Instruction *newInstruction() {
    return (Instruction *) malloc(sizeof(Instruction));
}

Instructions * parseInput(char *filename, int *check) {
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
            token = strdup("halt");
        }
        newIns->instructions = (Instruction *)realloc(newIns->instructions, ++add*sizeof(Instruction));
        Instruction *is = &(newIns->instructions[add-1]);
        Operands *os = &(is->operands);
        //printf("\n[INPUT] [%s]",token);
        Function *function = getFunction(token);
        if(function == NULL){
            printf("\n[PARSER:ERROR] Unknown function %s!", token);
            add--;
            continue;
        } 
        is->opcode = function->opcode;
        //printf("\n[INFO] Function : %s Opcode : 0x%x", function->invokation, function->opcode);
        is->format = function->format;

        switch (is->format) {
            case ONE_ADDRESS: {
                                  token = strtok(NULL, " ");
                                  getOperand(&(os->onea.op1), token, &insert);
                                  checkOperand(function, &(os->onea.op1), 1, &insert);

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
                                  Operand *op1 = &(os->twoa.op1);
                                  Operand *op2 = &(os->twoa.op2);
                                  token = strtok(NULL, " ");
                                  getOperand(op1, token, &insert);
                                  checkOperand(function, op1, 1, &insert);
                                  token = strtok(NULL, " ");
                                  getOperand(op2, token, &insert);
                                  checkOperand(function, op2, 2, &insert);
                                  *check = insert;
                                  break;
                              }
            case THREE_ADDRESS: {
                                    Operand *op1 = &(os->threa.op1);
                                    Operand *op2 = &(os->threa.op2);
                                    Operand *op3 = &(os->threa.op3);

                                    token = strtok(NULL, " ");
                                    getOperand(op1, token, &insert);
                                    checkOperand(function, op1, 1, &insert);

                                    token = strtok(NULL, " ");
                                    getOperand(op2, token, &insert);
                                    checkOperand(function, op2, 2, &insert);

                                    token = strtok(NULL, " ");
                                    getOperand(op3, token, &insert);
                                    checkOperand(function, op3, 3, &insert);

                                    *check = insert;
                                    break;
                                }
            case ZERO_ADDRESS: { 
                                   os->zeroa.dummy = '0';
                                   break;
                               }
        }
        lastins = is->opcode;
        free(buff);
        //printMem(m, add);
        if (is->opcode == HALT)
            insert = 0;
    }

    if(!maind){
        printf("\n[PARSER:ERROR] main is not defined!");
        *check = 0;
    }
    else if(!mainf){
        printf("\n[PARSER:ERROR] main is not enclosed!");
        *check = 0;
    }

    if(fp!=stdin)
        fclose(fp);
    newIns->noi = add;
    return newIns;
}
