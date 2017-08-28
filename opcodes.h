#ifndef OPCODES_H
#define OPCODES_H

/* Operation codes */

#define INCR 0xA0
#define DECR 0xA1
#define UNLET 0xA2
//print is defined in I/O functions
#define LOAD 0xA4
#define STORE 0xA5
#define LET 0xA6
#define HALT 0xA7
#define ADD 0xA8
#define SUB 0xA9
#define MUL 0xAA
#define DIV 0xAB
#define SETL 0xAC
#define JNE 0xAD
#define JLT 0xAE
#define JGT 0xAF

// I/O Functions //
/* 1. ^ denotes immediate string operand
 * 2. \ denotes escape character - \s is space and \e denotes end of a line, to print a \, \\ should be used
 * 3. % denotes formatter, only two types are allowed - string as %s and integer as %d
 *
 * 4. Print will take the whole string as operand, and do the formatting later while execution
 *      Format :
 *              print ^printstring ^arg1,arg2,arg3
 *              arguments must be a NO SPACE comma separated list of one or more variables, direct addresses, 
 *              register numbers or all type of immediate addressing operands, which should contain .
 *      Example :
 *              print ^my\sname\sis\s%s ^_name
 *              print ^I\swas\sborn\son\s%d\s%d,\s%d ^_day,_month,_year
 *      prntl has the same format and effect as print, additionally it'll print a \n to switch to a new line
 *
 * 5. Input will ask user for inputs. Two separate input functions are available to input two different data types:
 *      5.1 inpts should be used to input a string in the following format:
 *              inpts ^prompt variable_to_store
 *          Since, there is not much use of strings right now except for print or
 *          prompt, inpts is not allowed to store the input in register. And since
 *          the string will be stored in memory, there will be a small performance
 *          hit, but if you use io operations frequently, that will be the ultimate
 *          case anyway.
 *      5.2 inpti should be used to input an integer in the following format:
 *              inpti ^prompt variable_to_store
 *      To display no prompt, one can use (noprompt) as the prompt string, i.e.
 *              inpts ^(noprompt) _var
 *              inpti ^(noprompt) _var
 *       This will read a string from stdin and store it to variable 'var', displaying no prompt to the user.
 *
 *       The input given by the user will be later converted to appropiate type and stored into memory. However,
 *       it'll be failed with an error message if the user gives a string as an integer input, hence, showing 
 *       clear prompts hinting the input type to the user is highly recommended.
 *
*/
#define PRINT 0xA3 // print ^roll\snumber\s%d ^_a ==> (roll number %d, _a)
                    // print ^my\sname\sis\s%s ^_name
                    // print ^%d ^_a
                    // print ^%d\s%s ^_a,_name
#define PRNTL 0xB0 // print ....\n
#define INPTS 0xB1 // inpts ^Enter\syour\sname\s:\s  _name
#define INPTI 0xB2  // inpti ^Enter\syour\snumber\s:\s  _a
                    // inpti ^(noprompt) _a

#endif
