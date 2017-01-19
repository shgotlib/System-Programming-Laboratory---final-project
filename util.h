/*
 * util.h
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */



#ifndef UTIL_H_

#define UTIL_H_

/************ Includes **************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/************ Macros **************/

#define MAX_LABLE_LENGTH 30 /*length of label can be max 30 characters*/
#define FALSE 0
#define TRUE 1
#define SHORT_WORD_LENGTH 13
#define LONG_WORD_LENGTH 15
#define FOREVER	for(;;)
#define EXTERN ".extern"
#define ENTRY ".entry"
#define DATA ".data"
#define STRING ".string"

/************ New types **************/

/* Values of special octal signs */
struct {
	char *binary; /* the binary code */
	char *octal; /* the special octal equivalent */
} specialOctal[] = { { "000", "!" }, { "001", "@" }, { "010", "#" }, { "011",
		"$" }, { "100", "%" }, { "101", "^" }, { "110", "&" }, { "111", "*" }, {
		NULL, NULL } };

typedef unsigned int boolean;

/* Lines Management */
typedef struct some_line {
	int lineNumber; /* The line number */
	int address; /* The address of the line - get the IC/DC*/
	char *lineStr; /* The string in the line */
	boolean isError; /* The line contains error */
	char *savedWord; /* the binary word of the line */
} lineInfo;

/* Labels Management */
typedef struct some_label {
	char name[MAX_LABLE_LENGTH];
	int address;
	boolean isExtern;
	boolean isEntry;
	boolean isCommand;
	boolean isData;
} labelInfo;

/********** External variables ************/

extern struct {
	char *name;
	char *code;
} reg[];

/* Values of commands */
extern struct {
	char *name; /* The name */
	char *code; /* The code of operand */
	short numOfOp; /* The number of operands that the command can handle */
} commands[];

extern int curLabel;
extern labelInfo labelsTable[];

/************ Function declaration **************/

char * isCommand(char*);
char * isRegister(char *);

#endif /* UTIL_H_ */

