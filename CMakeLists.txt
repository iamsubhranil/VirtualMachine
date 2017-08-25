cmake_minimum_required(VERSION 2.8)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-s -O3")

add_executable(machine main.c
        instruction.c
        machine.c
        function.c
        cycle.c
        utility.c
        loader.c
        parser.c
        writer.c
        print.c)
