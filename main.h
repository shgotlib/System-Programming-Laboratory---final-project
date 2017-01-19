/*
 * main.h
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */

#ifndef MAIN_H_
#define MAIN_H_

/************ Includes **************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/************ Macros **************/

#define MAX_DATA 1000
#define MAX_INPUT 81 /* last char is for end string sign */
#define MAX_LABLE_LENGTH 30 /*length of label can be max 30 characters*/
#define FIRST_ADDRESS 100 /* words counter start with as default */
#define FALSE 0
#define TRUE 1
#define SHORT_WORD_LENGTH 13
#define LONG_WORD_LENGTH 15

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

/************ Global variables **************/

char obFileName[30];
char entFileName[30];
char extFileName[30];
int IC = FIRST_ADDRESS;
int DC = 0;
int curLine = 0;
int curDataLine = 0;
int curCommandLine = 0;
int curLabel = 0;
int curEntryLabel = 0;
int curExternLabel = 0;
lineInfo CommandWordsTable[MAX_DATA]; /* An array that holds all correct commands lines */
lineInfo DataWordsTable[MAX_DATA]; /* An array that holds all correct data lines */
lineInfo linesTabel[MAX_DATA]; /* An array that holds all lines */
labelInfo labelsTable[MAX_DATA]; /* An array that hold all labels */
labelInfo entryLabels[MAX_DATA]; /* An array that holds all entry labels */
labelInfo externLabel[MAX_DATA]; /* An array that hold all external labels */


/************ Function declaration **************/

char *toSpecialOctal(char*);
char *toBinary(int, int);
void firstProcess(lineInfo*);
void secondProcess(lineInfo*);
void parseFiles(char*);
void readFiles(FILE*);

#endif /* MAIN_H_ */
