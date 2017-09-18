#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "machine.h"
#include "parser.h"
#include "writer.h"
#include "loader.h"
#include "cycle.h"
#include "analyzer.h"
/* Help */
/*======*/

void help() {
    printf("\n Flags : \n \
            --input  \t-i\tSpecify an input file\n \
            --output \t-o\tSpecify output executable\n \
            --norun  \t-n\tCompile and save, but do not run the source file (Must be used with --output)\n \
            --run    \t-r\tRun a compiled executable\n \
            --analyze\t-a\tAnalyze a precompiled executable\n \
            --help   \t-h\tShow help\n \
            Usage :\n \
            \t1. machine [--input|-i] sourcefile\n \
            \t2. machine [--input|-i] sourcefile [--output|-o] executable\n \
            \t3. machine [--input|-i] sourcefile [--output|-o] executable [--norun|-n]\n \
            \t4. machine [--run|-r] executable\n \
            \t5. machine [--output|-o] executable\n \
            \t6. machine [--output|-o] executable [--norun|-n]\n \
            \t7. machine [--analyze|-a] executable\n \
            \n");
}

/* Driver */
/*========*/

int main(int argc, char **argv) {
    Machine *m = getMachine();
    Instructions *ins = NULL, *libIns = NULL;
    char *inputFilename = NULL, *outputFilename = NULL, *executableName = NULL, *libraries = NULL;
    int r = 1, h = 0, a = 0, check = 1, module = 0, link = 0;
    if (argc > 1) {
        int i = 1;
        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0)
                inputFilename = argv[++i];
            else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-output") == 0)
                outputFilename = argv[++i];
            else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--run") == 0)
                executableName = argv[++i];
            else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--norun") == 0)
                r = 0;
            else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
                h = 1;
            else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--help") == 0){
                a = 1;
                executableName = argv[++i];
            }
            else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--module") == 0){
                module = 1;
                r = 0;
            }
            else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--link") == 0){
                link = 1;
                libraries = argv[++i];
            }
        }
        if (h) {
            if (inputFilename || executableName || !r || outputFilename)
                printf("\n[HELP] Ignoring additional commands!");
            help();
            return 0;
        } else if (((argc - 1) - !r) % 2 != 0) {
            printf("\n[ERROR] Wrong arguments.\n");
            help();
            return 0;
        } else if (a && !executableName){
            printf("\n[ERROR] Provide a binary to analyze!\n");
            return 1;
        } else if (inputFilename && executableName) {
            printf("\n[ERROR] Wrong use of '--run'. Use '--output' instead.\n");
            return 1;
        } else if ((executableName || (inputFilename && !outputFilename)) && !r) {
            printf("\n[ERROR] Wrong use of '--norun'. See '--help'.\n");
            return 1;
        } else if (!inputFilename && !executableName && !outputFilename) {
            printf("\n[ERROR] Wrong arguments.\n");
            help();
            return 0;
        }
        else if(module && r){
            printf("\n[ERROR] A library cannot be run directly!\n");
            return 1;
        }
        else if(link && executableName){
            printf("\n[ERROR] Wrong use of '--link'! See '--help'.\n");
            return 1;
        }

        if (executableName) {
            ins = loadBinary(executableName, &check);
            if (check){
                if(a)
                    analyze(ins);
                else{
                    if(check){
                        writeInstructions(m, ins);
                        run(m);
                    }
                }
            }
        } else if (inputFilename || outputFilename) {
            ins = parseInput(inputFilename, &check, !module);
            if(link){
                libIns = loadLibraries(libraries, &check);
            }
            if(check){
                writeInstructions(m, libIns);
                writeInstructions(m, ins);
                if ((ins->noi > 0) & r){
                    run(m);
                }
                if ((ins->noi > 0) & (outputFilename != NULL)) {
                    //finalizeInstructions(m, ins);
                    writeBinary(ins, outputFilename, !module, libraries);
                }
            }
        }
    } else {
        ins = parseInput(NULL, &check, !module);
        if(link){
            libIns = loadLibraries(libraries, &check);
        }
        if (check){
            writeInstructions(m, libIns);
            writeInstructions(m, ins);
            run(m);
        }
    }

    destroyMachine(m);
    freeInstructions(ins);
    freeInstructions(libIns);

    return 0;
}
