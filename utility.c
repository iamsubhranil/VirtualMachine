#include"utility.h"
#include<stdlib.h>
/*
 * Adds the given character to the buffer. Since it modifies
 * the buffer itself, it returns the pointer in any case.
 * However, the pointer may not be reassigned to a new address
 * after calling realloc(). Also, there is no typical NULL
 * check after realloc, because it will practically never happen.
 * 
 * Arguments => buffer : The buffer to extend
 * 		bufferSize : A pointer to the present size of the buffer
 * 		add : The character to add
 * Returns   => The newly relocated buffer
 */
char * addToBuffer(char *buffer, size_t *bufferSize, char add){
	//char *backup = buffer;
	buffer = (char *)realloc(buffer, ++(*bufferSize)); //Call realloc to extend the buffer to bufferSize+1
	//if(backup!=buffer && *bufferSize>1)
	//	free(backup);
	buffer[*bufferSize-1] = add; //Add the character to the newly available position
	return buffer;
}

/*
 * Limited getline() implementation for non POSIX(read Windows)
 * systems. This method reads a line of arbitrary length from stdin, 
 * stores that in buffer, returning the number of characters read.
 * 
 * Arguments => buffer : The buffer to store the line, terminated with
 * 			EOF or '\n' as applicable
 * Returns => The number of characters read from stdin
 */
size_t readline(char **buffer, FILE *fp){
	size_t read_size = 0; // The read counter
	(*buffer) = (char *)malloc(sizeof(char)); // Allocate atleast one char of memory
	int c = 1; // Temporary character to store stdin read

	while(c!=EOF && c!='\n'){ // Continue until the end of line
		c = getc(fp); // Read a character from stdin
		(*buffer) = addToBuffer((*buffer), &read_size, (c=='\n'||c==EOF||c=='\r')?'\0':c); // Add it to the buffer
		if(c=='\r') // Windows
			c = getc(fp);
	}
	return read_size; // Return the amount of characters read
}

char *stripFirst(char *val){
	char *buffer = NULL;
	size_t len = strlen(val);
	size_t i = 1;
	size_t dummy = 0;
	while(i<len){
		buffer = addToBuffer(buffer, &dummy, val[i]);
		i++;
	}
	if(buffer[dummy-1]!='\0')
		buffer = addToBuffer(buffer, &dummy, '\0');
	return buffer;
}

int alpha(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}

int digit(char c){
	return (c>='0' && c<='9');
}

int aldigit(char c){
	return alpha(c) || digit(c);
}
