/*
 * secondProcess.h
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */

#ifndef SECONDPROCESS_H_
#define SECONDPROCESS_H_

/********* Includes ***********/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/********** Macros ************/

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

/********** New types ************/

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

/********** Global variables ************/

lineInfo linesTabel[MAX_DATA];

/********** External variables ************/

extern struct {
	char *name; /* The name */
	char *code; /* The code of operand */
	short numOfOp; /* The number of operands that the command can handle */
} commands[];

extern int IC;
extern int DC;
extern int curLine;
extern int curDataLine;
extern int curLabel;
extern int curEntryLabel;
extern int curExternLabel;
extern lineInfo linesTable[];
extern lineInfo CommandWordsTable[];
extern lineInfo DataWordsTable[];
extern labelInfo labelsTable[];
extern labelInfo externLabel[];

/********** Function declaration  ************/

void secondProcess(lineInfo*);
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
void getCommandLine(lineInfo*, char*, boolean);
int countWords(char*);
char *strrev(char*);
char *saveDynamic(char*, int, int, int);
char *saveDirectly(char*, int, int);


#endif /* SECONDPROCESS_H_ */
