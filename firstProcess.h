/*
 * firstProcess.h
 * 
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */

#ifndef FIRSTPROCESS_H_
#define FIRSTPROCESS_H_

/************ Includes **************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/************ Macros **************/

#define MAX_DATA 1000
#define MAX_INPUT 81 /* last char is for end string sign */
#define MAX_LABLE_LENGTH 30 /*length of label can be max 30 characters*/
#define FIRST_ADDRESS 100 /* words counter start with as default */
#define FALSE 0
#define TRUE 1
#define SHORT_WORD_LENGTH 13
#define LONG_WORD_LENGTH 15
#define FOREVER	for(;;)
#define EXTERN ".extern"
#define ENTRY ".entry"
#define DATA ".data"
#define STRING ".string"
#define ONE_WORD 1
#define TWO_WORDS 2
#define TREE_WORDS 3
#define FOUR_WORDS 4
#define DUMMY "101"

/************ New types **************/

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


struct {
	char *name;
	char *code;
} reg[] = { { "r0", "000000" }, { "r1", "000001" }, { "r2", "000010" }, { "r3",
		"000011" }, { "r4", "000100" }, { "r5", "000101" }, { "r6", "000110" },
		{ "r7", "000111" }, { "invalid", NULL } };

/* Values of commands */
struct {
	char *name; /* The name */
	char *code; /* The code of operand */
	short numOfOp; /* The number of operands that the command can handle */
} commands[] = { { "mov", "0000", 2 }, { "cmp", "0001", 2 },
		{ "add", "0010", 2 }, { "sub", "0011", 2 }, { "not", "0100", 1 }, {
				"clr", "0101", 1 }, { "lea", "0110", 2 }, { "inc", "0111", 1 },
		{ "dec", "1000", 1 }, { "jmp", "1001", 1 }, { "bne", "1010", 1 }, {
				"red", "1011", 1 }, { "prn", "1100", 1 }, { "jsr", "1101", 1 },
		{ "rts", "1110", 0 }, { "stop", "1111", 0 }, { NULL, NULL, -1 } };

/********** Global variables ************/

int curCommandLine;

/********** External variables ************/

extern int IC;
extern int DC;
extern int curDataLine;
extern int curLabel;
extern lineInfo linesTable[];
extern lineInfo CommandWordsTable[];
extern lineInfo DataWordsTable[];
extern labelInfo labelsTable[];

void firstProcess(lineInfo*);
void addLabel(labelInfo*);
char * isLegalLabel(char*, int);
char * isRegister(char *);
char * isLegalNumber(char*, int);
char **splitSentenceToWords(char *);
char * isCommand(char*);
char *toBinary(int, int);
boolean isDataLine(char*);
boolean isExternLine(char*);
boolean isDynamic(char*, char*, int*, int*);
boolean isLabelExist(char*);
boolean isEntryLabel(char*);
void getDataLine(lineInfo*, boolean);
int countWords(char*);

#endif /* FIRSTPROCESS_H_ */
