cmake_minimum_required(VERSION 2.8)

add_executable(machine main.c
        instruction.c
        machine.c
        function.c
        cycle.c
        utility.c
        loader.c
        parser.c
        writer.c
        print.c
        analyzer.c
        floader.c
        operands.c)
