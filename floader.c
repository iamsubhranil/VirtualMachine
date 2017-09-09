#include "utility.h"
#include <stdlib.h>
#include "floader.h"
#include "names.h"
#include "operands.h"

#define sizeof_arr(x)  (sizeof(x) / sizeof((x)[0]))
#define base_func 0xA0
#define base_mode 0x20
#define base_format 0x30


static Function *functions = NULL;
extern char functionDataStart[] asm("_binary_definition_parser_start");
extern char functionDataEnd[] asm("_binary_definition_parser_end");


void checkOperand(Function *func, Operand *o, int opnum, int *check){
    if(*check == 0)
        return;
    if(o->mode == 0){
        printf("\n[ERROR] Missing operand %d for function %s!", opnum, func->invokation);
        *check = 0;
        return;
    }
    
    if(opnum > (func->format - base_format)){
        printf("\n[VALIDATOR:ERROR] Number of operands for %s must not exceed %u!", func->invokation, func->format - base_format);
        *check = 0;
        return;
    }
    uint8_t *allowedModes = func->expectedArguments[opnum - 1];
    int i = 0;
    while(allowedModes[i] != TERMINATOR){
        if(o->mode == allowedModes[i])
            return;
        i++;
    }

    printf("\n[VALIDATOR:ERROR] Bad addressing mode `%s` for operand %d of instruction %s!", modeNamesVerbose[o->mode-base_mode], opnum, func->invokation);
    *check = 0;
}

Function *getFunction(char *in){
    if(functions == NULL){
        printf("\n[BOOTLOADER:ERROR] No functions loaded!");
        return NULL;
    }
    Function *temp = functions;
    while(temp != NULL){
        if(strcmp(temp->invokation, in) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void loadFunctions(int *check){
    if(strlen(functionDataStart) == 0){
        printf("\n[ERROR] Unable to load function definitions!");
        *check = 0;
        return;
    }
    uint8_t count = 0;
    Function *prev = NULL;
    size_t size = 1, lineCount = 0;
    while(size > 0){
        char *definition = NULL, **temp = NULL;
        size_t dummy;
        definition = getNextLine(functionDataStart, &lineCount); // func 3 1:2,3,4 2:3,4
        if(definition[0]=='#')
            continue;
        //printf("\n%s\n", definition);
        Function *now = (Function *)malloc(sizeof(Function));
        now->opcode = base_func + count;
        dummy = splitIntoArray(definition, &temp, ' ');
        if(temp==NULL)
            break;
        now->invokation = strdup(temp[0]); // func
        int numop = atoi(temp[1]); // 3
        //printf("\nExpected arguments for %s is %d", temp[0], numop);
        now->format = base_format + numop;
        if(numop > 0){
            now->expectedArguments = (uint8_t **)malloc(sizeof(uint8_t *) * numop);
            int i = 0;
            while(i < numop){
                //printf("\nOperand %d : ", (i+1));
                char **basic = NULL, **typeList = NULL;
                splitIntoArray(temp[2+i], &basic, ':'); // 1:2,3,4 => 1 2,3,4
                size_t numList = splitIntoArray(basic[1], &typeList, ','), loop = 0; // 2,3,4 => 2 3 4
                now->expectedArguments[i] = (uint8_t *)malloc(sizeof(uint8_t)*numList+1);
                while(numList > loop){
                    now->expectedArguments[i][loop] = base_mode + atoi(typeList[loop]) - 1;
                    loop++;
                }
                now->expectedArguments[i][loop] = base_mode + 0xf; // Terminator
                //printf("\n");
                i++;
            }
        }
        if(prev == NULL)
            functions = now;
        else
            prev->next = now;
        prev = now;
        count++;
    }
}

/*
int main(){
    int check = 1;
    //char *line = "mytest of a strong line", **ck = NULL;
    //splitIntoArray(line, &ck, ' ');
    loadFunctions(&check);
    while(functions != NULL){
        printf("\nFunction : %s", functions->invokation);
        printf("\n\tFormat : %s", formatNames[functions->format-0x30]);
        int numop = functions->format - 0x30 ;
        int i = 0;
        while(i < numop){
            printf("\n\t\tExpected modes for operand %d is : ", (i+1));
            size_t j = 0;
            while(functions->expectedArguments[i][j] != 0x2f){
                printf("%s ", modeNames[functions->expectedArguments[i][j] - 0x20]);
                j++;
            }
            i++;
        }
        functions = functions->next;
    }
    printf("\n");
}
*/
