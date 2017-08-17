#include"loader.h"
#include"binfmt.h"
#include"names.h"
#include<stdio.h>


static void readOperand(Operand *op, FILE *fp){
	fread(&(op->mode), sizeof(uint8_t), 1, fp);
	switch(op->mode){
		case IMMEDIATE: fread(&(op->data.imv), sizeof(uint32_t), 1, fp);
				break;
		case REGISTER: fread(&(op->data.rega), sizeof(uint8_t), 1, fp);
			       break;
		case DIRECT: fread(&(op->data.mema), sizeof(uint16_t), 1, fp);
			     break;
		case VARIABLE: printf("\n[LOADER:WARNING] Bad addressing mode!");
			       break;
	}
}

void loadBinary(Machine *m, char *filename){
	FILE *fp = fopen(filename, "rb");
	if(!fp){
		printf("\n[ERROR] Unable to open file %s!\n", filename);
		return;
	}
	Header h;
	fread(&(h.magic), sizeof(uint32_t), 1, fp);
	fread(&(h.version), sizeof(uint8_t), 1, fp);
	fread(&(h.numIns), sizeof(uint16_t), 1, fp);
	if(h.magic==MAGIC){
		printf("\n[LOADER] Magic matched");
		if(h.version==VERSION){
			printf("\n[LOADER] Version matched\n[LOADER] Instructions : %u", h.numIns);

			uint16_t i = 0;
			Instruction ins[h.numIns];
			while(i<h.numIns){
				fread(&(ins[i].opcode), sizeof(uint8_t), 1, fp);
				fread(&(ins[i].format), sizeof(uint8_t), 1, fp);
				switch(ins[i].format){
					case ZERO_ADDRESS: break;
					case ONE_ADDRESS: readOperand(&(ins[i].operands.onea.op1), fp);
							  break;
					case TWO_ADDRESS: readOperand(&(ins[i].operands.twoa.op1), fp);
							  readOperand(&(ins[i].operands.twoa.op2), fp);
							  break;
				}
				i++;
			}
			Footer f;
			fread(&f, sizeof(Footer), 1, fp);
			//printf("\n[LOADER] Expected file size : %lu", f.expectedSize);
			printf("\n[LOADER] Instruction length : %u bytes", f.instructionLength);
			if(f.instructionLength!=sizeof(Instruction)){
				printf("\n[LOADER:WARNING] Instruction length mismatch! The binary may be corrupted!");
			}
			printf("\n[LOADER] Binary format : %s\n", binaryFormat[f.format-0x40]);
			i = 0;
			while(i<h.numIns){
				//printIns(instructions[i]);
				writeInstruction(m, i, ins[i]);
				i++;
			}
		}
		else
			printf("\n[ERROR] Binary version incompatible!");
	}
	else
		printf("\n[ERROR] Magic not matched! This is not a valid executable file!");
	
	fclose(fp);
}
