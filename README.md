# VirtualMachine
A tiny virtual machine implementation in python and c


This is a virtual machine inbuilt with a tiny compiler for the virtual architechture. So you can program using ASCII text in its assembly-like language, compile and save the program, and run the compiled binary directly later. Bytecode of this machine is architechture independent, as it uses standardized `uint*_t` types everywhere while encoding and decoding instructions. This has a serious pitfall though: the machine cannot handle floating point computations. Since it is a very limited simulation, I think we can ignore the fact completely and still live happily together :).

#### Architechture
The machine is register based, with 16 GPRs. It also simulates a RAM-like form of storage, and because of that maintains a symbol table to provide the feature of variables. The machine offers four addressing modes :
1. Register addressing mode : To perform an operation directly on a register, `RN` is the notation of Nth register.
2. Direct addressing mode : To perform an operation directly on a memory address, which is used internally for all variable based operations. This addressing mode is currently inaccessible to a programmer.
3. Variable addressing mode : To perform an operation on memory using a variable name. Any operand starting with a letter is considered as a variable. However, in a compiled program, all variable addressings are converted to direct addressings.
4. Immediate addressing mode : This mode is used to interact with the environment, i.e. to load a value in memory, to perform arithmetic operations on a variable, etc.
##### Instruction set
There are presently 12 operations available to the machine, and the list will increase in near future.
1. let : Stores a value to a variable, i.e. in a location in memory.

            Example : let 500 a
2. unlet : Removes a variable from the symbol table, and resets the its address in memory to zero.

            Example : unlet a
3. load : Loads a value from a memory location to a register.

            Example : load a R1
4. store : Stores a value from a register to a memory location.

            Example : store R1 a
5. incr : Increments the value stored in a memory address or register by 1.

            Example : incr a
6. decr : Decrements the value stored in a memory address or register by 1.

            Example : decr a
7. add : Adds a variable, register or value to another variable or register.

            Example : add 50 a
8. sub : Subtracts a variable, register or value from another variable or register.

            Example : sub 50 a
9. mul : Multiplies a variable, register or value to another variable or register.

            Example : mul 50 a
10. div : Divides a variable, register or value by another variable or register.

            Example : div 50 a
11. print : Prints the value of a variable or register to the output.

            Example : print a
12. halt : Stops the machine. Every program must call halt at the end.

            Example : halt
            
#### System software and operating system
The machine uses absolute linking while loading binaries for a few reasons, but that restriction will hopefully someday dispose. Whenever a `let` instruction is issued, first the symbol table is searched for the address of the variable. If found, just the new value is set at the address. Otherwise, a new entry in symbol table is created with the name of the variable, the memory is searched for the first free cell, and that address is assigned to the variable in the table. Whenever an `unlet` call is issued, if direct addressing is used, just the address is reset to 0. Otherwise, the entry of the variable in the symbol table is also removed, if found. The address inspector module is very forgivable in nature. If you forget to `let` a variable before its use in the program, it will create a new variable in memory, set its value to 0, and supply it as the argument to your call in the program. The compiler is very basic, and only checks for right instructions in the program. It does not check the validity of the arguments, its your job :D .

#### Bytecode
The structure of an executable file is pretty straight forward : a header in front containing the magic, binary version and number of instructions, followed by N instructions, and finally a footer (which is just-for-the-sake-of right now). Each opcode is directly converted to an `uint8_t`, all variables are converted to direct addresses and written as `uint8_t`, and based on the addressing mode, either `uint8_t`, `uint16_t` or `uint32_t` is used to store the argument. In the source, you can easily view the hex values of the opcodes used. Furthermore, all members are explicitly "hand-written" to provide cross-platform support, and reduce the binary size over 70%. The loader just reads and checks the header, and puts all instructions to the machine while reading them from the file.


### Usage
1. Compile it using `clang`, `gcc`, or your favourite compiler :
 
        clang machine.c -o machine

2. Run 

        ./machine -h
