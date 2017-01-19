/*
 * secondProcess.c
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */

#include "secondProcess.h"

/* Get a label, recognize its address and save it in the lines table */
char *saveDirectly(char *label, int lineNumber, int opNum) {
	int i, labelAddress = -1;
	char addressString[SHORT_WORD_LENGTH];
	char *wordToSave = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	if (!wordToSave) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	if (!isLabelExist(label)) {
		fprintf(stderr,
				"Error: 1st operand is a label that dosn't exist. label name is: %s. (line number: %d)\n",
				label, lineNumber);
		exit(1);
	}
	/*search for the address*/
	for (i = 0; i < curLabel + 1; i++) {
		if (strcmp(label, labelsTable[i].name) == 0) {
			labelAddress = labelsTable[i].address;
			break;
		}
	}
	/*the external label stored in external lines table*/
	if (labelsTable[i].isExtern) {
		labelInfo extLabel;
		extLabel.address = IC + opNum;
		strcpy(extLabel.name, labelsTable[i].name);
		externLabel[curExternLabel++] = extLabel;
		strcat(wordToSave, "000000000000001"); /*the line's word is empty and ERA is external*/
	} else if (labelsTable[i].isEntry) {
		fprintf(stderr,
				"Error: operand can't be an entry label. (line number: %d)\n",
				lineNumber);
		exit(1);
	} else {
		strcpy(addressString, toBinary(labelAddress, SHORT_WORD_LENGTH));
		strcpy(wordToSave, addressString); /* concat the address of the label */
		strcat(wordToSave, "10"); /* concat the ERA */
	}
	return wordToSave;
}

/* Get operand, and range to cut its word. cut it in the desire range.*/
/* Complete the rest with two complementary method and save it in the lines table */
char *saveDynamic(char* label, int startRange, int endRange, int lineNumber) {
	int adderss, i, rest;
	char wordToCut[LONG_WORD_LENGTH] = "", binAddress[MAX_LABLE_LENGTH] = "",
			cutedAdd[SHORT_WORD_LENGTH] = "",
			labelTofind[MAX_LABLE_LENGTH] = "";
	char *wordToSave = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	if (startRange < 0 || endRange > 15) {
		fprintf(stderr,
				"Error: detected dynamic operand with illegal range. (line number: %d)\n",
				lineNumber);
		exit(1);
	}
	if (!wordToSave) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	/*search the label in the labels structure*/
	for (i = 0; i < curLabel; i++) {
		if (strcmp(label, labelsTable[i].name) == 0) {
			strcpy(labelTofind, labelsTable[i].name);
			break;
		}
	}
	if (labelsTable[i].isExtern) {
		fprintf(stderr,
				"Error: detect dynamic operand with external label. label name: %s. (line number: %d)\n",
				label, lineNumber);
		exit(1);
	}
	if (strcmp(labelTofind, "") == 0) {
		fprintf(stderr,
				"Error: detected dynamic operand with unknown label. the label %s not found. (line number: %d)\n",
				label, lineNumber);
		exit(1);
	}
	adderss = labelsTable[i].address;
	/*get the word stored in the specific address*/
	if (labelsTable[i].isCommand) {
		for (i = 0; i < curLine; i++) {
			if (adderss == CommandWordsTable[i].address) {
				strcpy(wordToCut, CommandWordsTable[i].savedWord);
				break;
			}
		}
	} else if (labelsTable[i].isData) {
		for (i = 0; i < curDataLine; i++) {
			if (adderss == DataWordsTable[i].address) {
				strcpy(wordToCut, DataWordsTable[i].savedWord);
				break;
			}
		}
	}
	/*cut the located word*/
	strcpy(binAddress, wordToCut);
	strcpy(binAddress, strrev(binAddress));
	strncpy(cutedAdd, binAddress + startRange, endRange - startRange + 1);
	strcpy(cutedAdd, strrev(cutedAdd));
	rest = SHORT_WORD_LENGTH - strlen(cutedAdd);
	/*complete the rest numbers with two complementary method*/
	if (cutedAdd[0] == '0') {
		while (rest--)
			strcat(wordToSave, "0");

		strcpy(wordToSave, cutedAdd);
		strcat(wordToSave, "00");
	} else {
		while (rest--)
			strcat(wordToSave, "1");
		strcat(wordToSave, cutedAdd);
		strcat(wordToSave, "00");
	}
	return wordToSave;
}

/* Get a line from the file. check more syntax. update the address of the entry labels */
void secondProcess(lineInfo *line) {
	boolean isLabel = FALSE;
	int i, c;
	char command[4];
	char **words = (char**) malloc(sizeof(char*) * strlen(line->lineStr));

	if (!words) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	if (line->lineStr[0] == '\n') {
		return;
	}

	words = splitSentenceToWords(line->lineStr);
	c = words[0][0];
	if (c == ';') {
		return;
	}
	if (isLegalLabel(words[0], line->lineNumber)) {
		isLabel = TRUE;
	}
	if (isLabel)
		words++;
	if (isDataLine(*words)) {
		return;
	} else if (isExternLine(*words)) {
		if (strcmp(*words, ENTRY) == 0) { /* the line is entry */
			words++;
			for (i = 0; i < curLabel; i++) {
				if (strcmp(labelsTable[i].name, *words) == 0) {
					labelsTable[i].isEntry = TRUE;
				}
			}
		}
		return;
	} else if (isCommand(*words)) {
		strcpy(command, *words);
		getCommandLine(line, command, isLabel);
	}
}

/* Get a command line, analyze its operands and save the relevant words */
void getCommandLine(lineInfo *commandLine, char* commandName, boolean isLabel) {
	lineInfo firstLine, secondLine, thirdLine;
	boolean isOneWord = FALSE;
	boolean firstIsRegister = FALSE;
	char *firstWord = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	char *SecondWord = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	char *thirdWord = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	char *dynamicLabel = (char*) malloc(sizeof(char) * MAX_INPUT);
	char *binaryNumber = (char*) malloc(sizeof(char) * SHORT_WORD_LENGTH);
	char **words = (char**) malloc(sizeof(char*) * MAX_INPUT);
	int i, firstRange, endRange;
	char op1[MAX_INPUT], op2[MAX_INPUT], command[4];
	if (!dynamicLabel || !words || !dynamicLabel || !binaryNumber) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}

	strcpy(command, commandName);
	words = splitSentenceToWords(commandLine->lineStr);
	for (i = 0; commands[i].code != NULL; i++) {
		if (strcmp(command, commands[i].name) == 0)
			break;
	}
	strcpy(firstWord, DUMMY);
	strcpy(SecondWord, "");
	strcpy(thirdWord, "");
	/*the nest decisions based on number of operands the command can get*/
	switch (commands[i].numOfOp) {
	case 0:
		strcat(firstWord, "00"); /*concat the group*/
		strcat(firstWord, commands[i].code); /*concat the opcode*/
		strcat(firstWord, "000000"); /*concat the rest word*/
		firstLine.savedWord = firstWord; /*save the word*/
		firstLine.address = IC++;
		linesTabel[curLine++] = firstLine;
		break;
	case 1:
		strcat(firstWord, "01"); /* concat the group */
		strcat(firstWord, commands[i].code); /* concat the opcode */
		strcat(firstWord, "00"); /* zero the 1st operand */

		strcpy(op1, isLabel ? words[2] : words[1]);

		if ((isRegister(op1))) {
			strcpy(op1, isRegister(op1));
			strcat(firstWord, "1100"); /* concat the 2nd operand and ERA */
			strcat(SecondWord, "0000000"); /* concat the 1st place and 6th places to zero */
			strcat(SecondWord, op1); /* concat the 2nd operand - 6 places */
			strcat(SecondWord, "00"); /* concat the ERA */
		} else if ((isLegalNumber(op1, commandLine->lineNumber))) {
			strcat(firstWord, "0000");
			SecondWord = isLegalNumber(op1, commandLine->lineNumber);
		} else if (isDynamic(op1, dynamicLabel, &firstRange, &endRange)) {
			SecondWord = saveDynamic(dynamicLabel, firstRange, endRange,
					commandLine->lineNumber);
			strcat(firstWord, "1000"); /* concat the 2nd operand and ERA */
			break;

		} else {
			SecondWord = saveDirectly(op1, commandLine->lineNumber, 1);
			strcat(firstWord, "0100"); /* concat the 2nd op (destination operand) and ERA */
		}
		/* add the 1st line */
		firstLine.savedWord = firstWord;
		firstLine.address = IC++;
		linesTabel[curLine++] = firstLine;
		/* add the 2nd line*/
		secondLine.savedWord = SecondWord;
		secondLine.address = IC++;
		linesTabel[curLine++] = secondLine;
		break;

	case 2:
		strcat(firstWord, "10"); /* concat the group */
		strcat(firstWord, commands[i].code); /* concat the opcode */

		strcpy(op1, isLabel ? words[2] : words[1]);
		strcpy(op2, isLabel ? words[3] : words[2]);
		/* 1st operand*/
		if (isRegister(op1)) {
			strcat(firstWord, "11"); /* concat the 1st operand */
			strcpy(op1, isRegister(op1));
			strcat(SecondWord, "0"); /* concat the leader zero */
			strcat(SecondWord, op1); /* concat the 6 digit of the 1st register */
			firstIsRegister = TRUE;
		} else if (isLegalNumber(op1, commandLine->lineNumber)) {
			strcat(firstWord, "00");
			SecondWord = isLegalNumber(op1, commandLine->lineNumber);
		} else if (isDynamic(op1, dynamicLabel, &firstRange, &endRange)) {
			SecondWord = saveDynamic(dynamicLabel, firstRange, endRange,
					commandLine->lineNumber);
			strcat(firstWord, "10"); /* concat the 1st operand */
		} else {
			SecondWord = saveDirectly(op1, commandLine->lineNumber, 1);
			strcat(firstWord, "01"); /* concat the 1st operand */
		}

		/* 2nd operand */
		if (isRegister(op2)) {
			strcpy(op1, isRegister(op2));
			strcat(firstWord, "1100"); /* concat the 2nd operand and ERA */
			if (firstIsRegister) {
				strcat(SecondWord, op1); /* concat the 6 digit of the 2nd register */
				strcat(SecondWord, "00");
				isOneWord = TRUE;
			} else {
				strcat(thirdWord, "0000000");/* concat the prefix */
				strcat(thirdWord, op1); /* concat the register */
				strcat(thirdWord, "00"); /* concat the ERA */
			}
		} else if (isLegalNumber(op2, commandLine->lineNumber)) {
			strcat(firstWord, "0000"); /* concat the 2nd operand and ERA */
			thirdWord = isLegalNumber(op1, commandLine->lineNumber);
			if (firstIsRegister)
				strcat(SecondWord, "00000000");
		} else if (isDynamic(op2, dynamicLabel, &firstRange, &endRange)) {
			strcat(firstWord, "1000"); /* concat the 2nd operand and ERA */
			thirdWord = saveDynamic(dynamicLabel, firstRange, endRange,
					commandLine->lineNumber);
			if (firstIsRegister)
				strcat(SecondWord, "00000000");
		} else {
			strcat(firstWord, "0100"); /* concat the 2nd operand and ERA */
			thirdWord = saveDirectly(op2, commandLine->lineNumber, 2);
			if (firstIsRegister)
				strcat(SecondWord, "00000000");
		}
		/* add the first line */
		firstLine.savedWord = firstWord;
		firstLine.address = IC++;
		linesTabel[curLine++] = firstLine;
		/* add the 2nd line */
		secondLine.savedWord = SecondWord;
		secondLine.address = IC++;
		linesTabel[curLine++] = secondLine;
		/* add the third line */
		if (!isOneWord) {
			thirdLine.savedWord = thirdWord;
			thirdLine.address = IC++;
			linesTabel[curLine++] = thirdLine;
		}
		break;
	} /* switch */
} /* getCommandLine */

