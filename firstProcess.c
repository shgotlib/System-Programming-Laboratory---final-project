/*
 * firstProcess.c
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 *
 * by: Adiel Bin Nun
 * ID: 319028288
 */

#include "firstProcess.h"

/* Get a label with name, type and address, save it in labels structure */
void addLabel(labelInfo *label) {
	labelInfo *newLabel = label;
	labelsTable[curLabel++] = *newLabel;
}

/* Gets a .data or .string line. check the syntax*/
/* If the syntax is OK - save the data in the data table with the appropriate address */
void getDataLine(lineInfo *dataLine, boolean isLabel) {
	boolean invalidString = FALSE;
	lineInfo newWord;
	int number, ch, cnt = countWords(dataLine->lineStr);
	char *binaryNumber, *binaryChar;
	char ** words = (char**) malloc(sizeof(char*) * strlen(dataLine->lineStr));
	if (!words) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	words = splitSentenceToWords(dataLine->lineStr);
	if (isLabel) {
		cnt--;
		words++;
	}
	/*the line is .data*/
	if (strcmp(DATA, *words) == 0) {
		words++;
		cnt--;
		while (cnt > 0) {
			if ((number = atoi(*words))) {
				binaryNumber = toBinary(number, LONG_WORD_LENGTH);

				newWord.address = DC++;
				newWord.savedWord = binaryNumber;
				DataWordsTable[curDataLine++] = newWord;
				words++;
			} else {
				fprintf(stderr,
						"Error: a data line with NaN. (line number: %d)\n",
						dataLine->lineNumber);
				newWord.isError = TRUE;
				DataWordsTable[curDataLine++] = newWord;
				words++;
			}
			cnt--;
		}
		/*the line is .string*/
	} else if (strcmp(STRING, *words) == 0) {
		char *cur;
		cur = (*(++words));
		if (*cur == '"')
			cur++;
		else
			invalidString = TRUE;

		while (*cur != '"') {
			ch = *cur;
			if (ch == '\n' || ch == 0) {
				invalidString = TRUE;
				break;
			}
			binaryChar = toBinary(ch, LONG_WORD_LENGTH);

			newWord.address = DC++;
			newWord.savedWord = binaryChar;
			DataWordsTable[curDataLine++] = newWord;
			cur++;
		}
		/*add the end of string sign*/
		newWord.address = DC++;
		newWord.savedWord = "000000000000000";
		DataWordsTable[curDataLine++] = newWord;
		if (*cur == '"')
			cur++;
		else
			invalidString = TRUE;
	}
	if (invalidString) {
		fprintf(stderr, "Error: nut a legal string. (line number: %d)\n",
				dataLine->lineNumber);
		newWord.isError = TRUE;
		DataWordsTable[curDataLine++] = newWord;
	}
}

/* Gets a line from the file, check syntax, save the data lines and command lines (without operands yet)*/
void firstProcess(lineInfo *line) {
	char op1[MAX_INPUT], op2[MAX_INPUT];
	char * dynamicLabel = (char*) malloc(sizeof(char) * MAX_INPUT);
	char **words = (char**) malloc(sizeof(char*) * MAX_INPUT);
	char *completeWord = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	char *typeLine;
	int c, i, firstRange, endRange, lineNum = line->lineNumber;
	lineInfo newWord;
	labelInfo newLabel;
	boolean isLabel = FALSE;

	if (!completeWord || !dynamicLabel || !words) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	/* the line has only "enter" */
	if (line->lineStr[0] == '\n') {
		return;
	}
	words = splitSentenceToWords(line->lineStr);
	/* The first char in the line */
	c = words[0][0];
	if (isspace(c) || c == ';') {
		return;
	}
	/* The line start with illegal sign */
	if (!isalpha(c) && c != '.') {
		fprintf(stderr,
				"Error: a line have to start with a char and start with %c. (line number: %d)\n",
				c, lineNum);
		newWord.isError = TRUE;
		CommandWordsTable[curCommandLine++] = newWord;
		return;
	}
	/* The line has label */
	if (isLegalLabel(*words, lineNum)) { /* If the line has a label */
		if (isLabelExist(words[0]) && !isEntryLabel(*words)) {
			fprintf(stderr,
					"Error: label %s declared recurrent. (line number: %d)\n",
					*words, line->lineNumber);
		}

		isLabel = TRUE;
		newLabel.isCommand = FALSE;
		newLabel.isExtern = FALSE;
		newLabel.isEntry = FALSE;
		newLabel.isData = FALSE;
		strcpy(newLabel.name, *words);
		words++;
	}
	typeLine = *words;
	/* the line holds numbers or string */
	if (isDataLine(*words)) {
		if (isLabel) {
			newLabel.address = DC;
			newLabel.isData = TRUE;
			addLabel(&newLabel);
		}
		getDataLine(line, isLabel);
		return;
		/* the line is external or entry */
	} else if (isExternLine(*words)) {
		if (isLabel)
			fprintf(stdin,
					"Info: external and entry line with unnecessary leader label. (line number: %d)\n",
					line->lineNumber);
		if ((isLabel && countWords(line->lineStr) > TREE_WORDS)
				|| (!isLabel && countWords(line->lineStr) > TWO_WORDS)) {
			fprintf(stderr,
					"Error: external or entry line with too many words. (line number: %d)\n",
					line->lineNumber);
			newWord.isError = TRUE;
			CommandWordsTable[curCommandLine++] = newWord;
		}

		words++;
		strcpy(newLabel.name, *words);
		if (strcmp(typeLine, EXTERN) == 0) { /* the line is external*/
			newLabel.isExtern = TRUE;
			newLabel.isEntry = FALSE;
			newLabel.isData = FALSE;
			newLabel.isCommand = FALSE;
			newLabel.address = -1;
			addLabel(&newLabel);
		} else { /* the line is entry - don't save the label */
			newLabel.isEntry = TRUE;
			newLabel.address = -1;
		}

		return;
		/* the line holds command with 0, 1 or 2 operands*/
	} else if (isCommand(typeLine)) {
		if ((isLabel && countWords(line->lineStr) > FOUR_WORDS)
				|| (!isLabel && countWords(line->lineStr) > TREE_WORDS)) {
			fprintf(stderr,
					"Error: command line with too many words. (line number: %d)\n",
					line->lineNumber);
			newWord.isError = TRUE;
			CommandWordsTable[curCommandLine++] = newWord;
		}
		if (isLabel) {
			newLabel.address = IC;
			newLabel.isCommand = TRUE;
			addLabel(&newLabel);
		}
		for (i = 0; commands[i].code != NULL; i++) {
			if (strcmp(*words, commands[i].name) == 0)
				break;
		}
		words++;
		strcpy(completeWord, DUMMY);
		switch (commands[i].numOfOp) {
		case 0:
			if ((isLabel && countWords(line->lineStr) > TWO_WORDS)
					|| (!isLabel && countWords(line->lineStr) > ONE_WORD) || (isLabel && countWords(line->lineStr) < TWO_WORDS)
					|| (!isLabel && countWords(line->lineStr) < ONE_WORD)) {
				fprintf(stderr,
						"Error: command line with illegal number of operands. (line number: %d)\n",
						line->lineNumber);
				newWord.isError = TRUE;
				return;
			}
			strcat(completeWord, "00"); /* concatenate the group */
			strcat(completeWord, commands[i].code); /* concatenate the opcode */
			strcat(completeWord, "000000"); /* concatenate the ops and ERA*/
			newWord.address = IC;
			newWord.savedWord = completeWord;
			CommandWordsTable[curCommandLine++] = newWord;
			IC += 1;
			break;
		case 1:
			if ((isLabel && countWords(line->lineStr) > TREE_WORDS)
					|| (!isLabel && countWords(line->lineStr) > TWO_WORDS) || (isLabel && countWords(line->lineStr) < TREE_WORDS)
					|| (!isLabel && countWords(line->lineStr) < TWO_WORDS)) {
				fprintf(stderr,
						"Error: command line with illegal number of operands. (line number: %d)\n",
						line->lineNumber);
				newWord.isError = TRUE;
				return;
			}
			strcpy(op1, *words);

			strcat(completeWord, "01"); /* concatenate the group */
			strcat(completeWord, commands[i].code); /* concatenate the opcode */
			strcat(completeWord, "00"); /* concatenate the 1st op to zero */
			if (isRegister(op1))
				strcat(completeWord, "1100"); /* concatenate the 2nd op and ERA */
			else if (isLegalNumber(op1, line->lineNumber))
				strcat(completeWord, "0000"); /* concatenate the 2nd op and ERA */
			else if (isDynamic(op1, dynamicLabel, &firstRange, &endRange))
				strcat(completeWord, "1000"); /* concatenate the 2nd op and ERA */
			else
				strcat(completeWord, "0100"); /* concatenate the 2nd op and ERA */
			newWord.savedWord = completeWord;
			newWord.address = IC;
			CommandWordsTable[curCommandLine++] = newWord;
			IC += 2;
			break;
		case 2:
			if ((isLabel && countWords(line->lineStr) > FOUR_WORDS)
					|| (!isLabel && countWords(line->lineStr) > TREE_WORDS) || (isLabel && countWords(line->lineStr) < FOUR_WORDS) || (!isLabel && countWords(line->lineStr) < TREE_WORDS)) {
				fprintf(stderr,
						"Error: command line with illegal number of operands. (line number: %d)\n",
						line->lineNumber);
				newWord.isError = TRUE;
				return;
			}
			strcpy(op1, *words);
			words++;
			strcpy(op2, *words);
			/* the commands mov, add and sub can't get a number as a target operand */
			if ((strcmp(commands[i].name, "mov") == 0)
					|| (strcmp(commands[i].name, "add") == 0)
					|| (strcmp(commands[i].name, "sub") == 0)) {
				if (isLegalNumber(*words, line->lineNumber)
						|| isDynamic(*words, dynamicLabel, &firstRange,
								&endRange)) {
					fprintf(stderr,
							"Error: illegal destination operand. command %s can not accept a number as a target operand. (line number: %d)\n",
							commands[i].name, line->lineNumber);
					newWord.isError = TRUE;
				}
			}
			strcat(completeWord, "10"); /* concatenate the group */
			strcat(completeWord, commands[i].code); /* concatenate the opcode */

			if (isRegister(op1))
				strcat(completeWord, "11"); /* concatenate the 1st op and ERA */
			else if (isLegalNumber(op1, line->lineNumber))
				strcat(completeWord, "00"); /* concatenate the 1st op */
			else if (isDynamic(op1, dynamicLabel, &firstRange, &endRange))
				strcat(completeWord, "10"); /* concatenate the 1st op */
			else
				strcat(completeWord, "01"); /* concatenate the 1st op */

			if (isRegister(op2))
				strcat(completeWord, "1100"); /* concatenate the 2nd op and ERA */
			else if (isLegalNumber(op2, line->lineNumber))
				strcat(completeWord, "0000"); /* concatenate the 2nd op and ERA */
			else if (isDynamic(op2, dynamicLabel, &firstRange, &endRange))
				strcat(completeWord, "1000"); /* concatenate the 2nd op and ERA */
			else
				strcat(completeWord, "0100"); /* concatenate the 2nd op and ERA */
			newWord.savedWord = completeWord;
			newWord.address = IC;
			CommandWordsTable[curCommandLine++] = newWord;
			IC += 3;
			break;
		default:
			break;
		}
		return;
	} else {
		/* Line contains garbage content */
		fprintf(stderr,
				"Error: line have to start with a label, register name, legal command name or data type. (line number: %d)\n",
				line->lineNumber);
		newWord.isError = TRUE;
		CommandWordsTable[curCommandLine++] = newWord;
		return;
	}
}

