#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"machine.h"
#include"parser.h"
#include"writer.h"
#include"loader.h"
#include"cycle.h"
/* Help */
/*======*/

void help(){
	printf("\n Flags : \n \
			--input \t-i\tSpecify an input file\n \
			--output\t-o\tSpecify output executable\n \
			--norun \t-n\tCompile and save, but do not run the source file (Must be used with --output)\n \
			--run   \t-r\tRun a compiled executable\n \
			--help  \t-h\tShow help\n \
			Usage :\n \
			\t1. machine [--input|-i] sourcefile\n \
			\t2. machine [--input|-i] sourcefile [--output|-o] executable\n \
			\t3. machine [--input|-i] sourcefile [--output|-o] executable [--norun|-n]\n \
			\t4. machine [--run|-r] executable\n \
			\t5. machine [--output|-o] executable\n \
			\n");
}

/* Driver */
/*========*/

int main(int argc, char **argv){
	Machine m;
	m.symbolTable = NULL;
	m.halt = 0;
	m.pc = 0;
	char *inputFilename = NULL, *outputFilename = NULL, *executableName = NULL;
	int r = 1;
	int h = 0;
	int check = 1;
	if(argc>1){
		int i = 1;
		for(i = 1;i<argc;i++){
			if(strcmp(argv[i], "-i")==0 || strcmp(argv[i], "--input")==0)
				inputFilename = argv[++i];
			else if(strcmp(argv[i], "-o")==0 || strcmp(argv[i], "-output")==0)
				outputFilename = argv[++i];
			else if(strcmp(argv[i], "-r")==0 || strcmp(argv[i], "--run")==0)
				executableName = argv[++i];
			else if(strcmp(argv[i], "-n")==0 || strcmp(argv[i], "--norun")==0)
				r = 0;
			else if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0)
				h = 1;
		}
		if(h){
			if(inputFilename || executableName || !r || outputFilename)
				printf("\n[HELP] Ignoring additional commands!");
			help();
			return 0;
		}
		else if(((argc-1)-!r)%2!=0){
			printf("\n[ERROR] Wrong arguments.\n");
			help();
			return 0;
		}
		else if(inputFilename && executableName){
			printf("\n[ERROR] Wrong use of '--run'. Use '--output' instead.\n");
			return 1;
		}
		else if((executableName || (inputFilename && !outputFilename)) && !r){
			printf("\n[ERROR] Wrong use of '--norun'. See '--help'.\n");
			return 1;
		}
		else if(!inputFilename && !executableName && !outputFilename){
			printf("\n[ERROR] Wrong arguments.\n");
			help();
			return 0;
		}

		if(executableName){
			loadBinary(&m, executableName, &check);
			if(check)
				run(&m);
		}
		else if(inputFilename || outputFilename){
			uint16_t num = parseInput(&m, inputFilename, &check);
			if((num > 0) & r & check)
				run(&m);
			if((num > 0) & (outputFilename!=NULL) & check){
				Instruction ins[num];
				uint16_t j = 0;
				while(j<num){
					ins[j] = readInstruction(&m, j);
					j++;
				}
				writeBinary(ins, num, &m, outputFilename);
			}	
		}
	}
	else{	
		parseInput(&m, NULL, &check);
		if(check)
			run(&m);
	}

	printf("\n");

	return 0;
}
