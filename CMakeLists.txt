cmake_minimum_required(VERSION 2.8)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")

add_library(machine SHARED machine.c)

add_library(cycle SHARED cycle.c
	function.c)
target_link_libraries(cycle machine)

add_library(parser SHARED parser.c
	utility.c)
target_link_libraries(parser machine)

add_library(loader SHARED loader.c)
target_link_libraries(loader machine)

add_library(writer SHARED writer.c)
target_link_libraries(writer machine)

add_library(printer SHARED print.c)
target_link_libraries(printer machine)

add_executable(main main.c)
target_link_libraries(main machine
	cycle
	parser
	loader
	writer
	printer)
set_target_properties(main PROPERTIES OUTPUT_NAME "machine")
