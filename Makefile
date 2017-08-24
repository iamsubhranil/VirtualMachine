machine: binfmt.h cycle.c cycle.h instruction.h loader.c loader.h machine.c machine.h main.c opcodes.h operands.h parser.c parser.h print.c print.h utility.c utility.h writer.c writer.h function.h function.c instruction.c
	clang -O3 cycle.c loader.c machine.c main.c parser.c print.c utility.c writer.c function.c instruction.c -o machine


machine_debug: binfmt.h cycle.c cycle.h instruction.h loader.c loader.h machine.c machine.h main.c opcodes.h operands.h parser.c parser.h print.c print.h utility.c utility.h writer.c writer.h function.h function.c instruction.c
	clang -g cycle.c loader.c machine.c main.c parser.c print.c utility.c writer.c function.c instruction.c -o machine


