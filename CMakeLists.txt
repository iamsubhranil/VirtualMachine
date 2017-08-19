cmake_minimum_required(VERSION 2.8)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -s")

add_library(machine SHARED machine.c)

add_library(function SHARED function.c)

add_library(cycle SHARED cycle.c)
target_link_libraries(cycle function)

add_library(utility SHARED utility.c)

add_library(parser SHARED parser.c)
target_link_libraries(parser utility)

add_library(loader SHARED loader.c)

add_library(writer SHARED writer.c)

add_library(printer SHARED print.c)

add_executable(main main.c)
target_link_libraries(main machine
	cycle
	parser
	loader
	writer)
set_target_properties(main PROPERTIES OUTPUT_NAME "machine")
