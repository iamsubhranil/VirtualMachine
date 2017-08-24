# VirtualMachine
A tiny virtual machine implementation in python and c


This is a virtual machine inbuilt with a tiny compiler for the virtual architechture. So you can program using ASCII text in its assembly-like language, compile and save the program, and run the compiled binary directly later. Bytecode of this machine is architechture independent, as it uses standardized `uint*_t` types everywhere while encoding and decoding instructions. This has a serious pitfall though: the machine cannot handle floating point computations. Since it is a very limited simulation, I think we can ignore the fact completely and still live happily together :).

#### Architechture
The machine is register based, with 16 GPRs. It also simulates a RAM-like form of storage, and because of that maintains a symbol table to provide the feature of variables. The machine offers four addressing modes :
1. Register addressing mode : To perform an operation directly on a register, `RN` is the notation of Nth register.
2. Direct addressing mode : To perform an operation directly on a memory address, which is used internally for all variable based operations. To specify a operand using direct addressing mode, precede the address with `@`.
3. Variable addressing mode : To perform an operation on memory using a variable name. Any operand starting with an underscore `_` is considered as a variable. However, in a compiled program, all variable addressings are converted to direct addressings.
4. Immediate addressing mode : This mode is used to interact with the environment, i.e. to load a value in memory, to perform arithmetic operations on a variable, etc. To specify an immediate constant, precede the value with `#`
##### Instruction set
There are presently 16 operations available to the machine.
1. let : Stores a value to a variable, i.e. in a location in memory.

            let #500 _a
2. unlet : Removes a variable from the symbol table, and resets the its address in memory to zero.

            unlet _a
3. load : Loads a value from a memory location to a register.

            load _a R1
4. store : Stores a value from a register to a memory location.

            store R1 _a
5. incr : Increments the value stored in a memory address or register by 1.

            incr _a
6. decr : Decrements the value stored in a memory address or register by 1.

            decr _a
7. add : Adds a variable, register or value to another variable or register.

            add #50 _a
8. sub : Subtracts a variable, register or value from another variable or register.

            sub #50 _a
9. mul : Multiplies a variable, register or value to another variable or register.

            mul #50 _a
10. div : Divides a variable, register or value by another variable or register.

            div #50 _a
11. print : Prints the value of a variable or register to the output.

            print _a
12. halt : Stops the machine. Every program must call halt at the end.

            halt
13. setl : Sets a label to the next instruction, to be used with jump calls.

            setl _test
            incr _a // test points here
14. jne : Jump to a label if arg2 is not equal to arg3. Arg2 should not be a constant. Arg3 can be a constant however.

            jne _test _a #20 // Jump to _test if value stored at _a is not equal to 20
15. jlt : Jump to a label if arg2 is less than arg3. Arg2 should not be a constant. Arg3 can be a constant however.

            jlt _test _a #20 // Jump to label _test if value stored at _a is less than 20
16. jgt : Jump to a label if arg2 is greater arg3. Arg2 should not be a constant. Arg3 can be a constant however.

            jne _test _a #20 // Jump to _test if value stored at _a is greater 20
            
#### System software and operating system
The machine uses absolute linking while loading binaries for a few reasons, but that restriction will hopefully someday dispose. Whenever a `let` instruction is issued, first the symbol table is searched for the address of the variable. If found, just the new value is set at the address. Otherwise, a new entry in symbol table is created with the name of the variable, the memory is searched for the first free cell, and that address is assigned to the variable in the table. Whenever an `unlet` call is issued, if direct addressing is used, just the address is reset to 0. Otherwise, the entry of the variable in the symbol table is also removed, if found. The address inspector module is very forgivable in nature. If you forget to `let` a variable before its use in the program, it will create a new variable in memory, set its value to 0, and supply it as the argument to your call in the program. The compiler is very basic, and only checks for right instructions in the program. It does not check the validity of the arguments, its your job :D .

#### Bytecode
The structure of an executable file is pretty straight forward : a header in front containing the magic, binary version and number of instructions, followed by N instructions, and finally a footer (which is just-for-the-sake-of right now). Each opcode is directly converted to an `uint8_t`, all variables are converted to direct addresses and written as `uint16_t`, and based on the addressing mode, either `uint8_t`, `uint16_t` or `uint32_t` is used to store the argument. In the source, you can easily view the hex values of the opcodes used. Furthermore, all members are explicitly "hand-written" to provide cross-platform support, and reduce the binary size over 70%. The loader just reads and checks the header, and puts all instructions to the machine while reading them from the file.


### Usage
1. Run `make machine` or `clang .` followed by `make all`
2. Run `./machine -h`
