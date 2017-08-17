machine: binfmt.h cycle.c cycle.h instruction.h loader.c loader.h machine.c machine.h main.c opcodes.h operands.h parser.c parser.h print.c print.h utility.c utility.h writer.c writer.h
	clang -g cycle.c loader.c machine.c main.c parser.c print.c utility.c writer.c -o machine
