/*
 * util.c
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 * 
 * by: Adiel Bin Nun
 * ID: 319028288
 */



#include "util.h"

/* Gets binary number (as a string), converts it into special octal string */
char *toSpecialOctal(char *binaryCod) {
	char *result = (char*) malloc(sizeof(char) * LONG_WORD_LENGTH);
	char oneLetter[4];
	int i = 5;
	int j;
	if (!result) {
		fprintf(stderr, "Error: no memory room\n");
		exit(1);
	}
	while (i > 0) {
		memcpy(oneLetter, binaryCod, 3);
		oneLetter[3] = '\0';
		binaryCod += 3;
		i--;
		for (j = 0; specialOctal[j].binary != NULL; j++) {
			if (strcmp(specialOctal[j].binary, oneLetter) == 0) {
				strcat(result, specialOctal[j].octal);
			}
		}

	}
	return result;
}

/* Reverse the string str, return it as ptr */
char *strrev(char *str) {
	int i, j;
	char temp;
	char *ptr = malloc(sizeof(char) * strlen(str));
	strcpy(ptr, str);
	if (!str || !*str)
		return str;
	/*swapping the letters*/
	for (i = 0, j = strlen(str) - 1; i < j; ++i, --j) {
		temp = ptr[i];
		ptr[i] = ptr[j];
		ptr[j] = temp;
	}
	return ptr;
}

/* Gets decimal number, converts it into a binary string */
char *toBinary(int num, int length) {
	int negative = num < 0 ? 1 : 0;
	double remainder, i = 1, binaryNumber = 0, counter = 0;
	char *cur;
	char *result = (char*) malloc(sizeof(char) * length);
	if (!result) {
		fprintf(stderr, "Error: no memory room\n");
		exit(1);
	}
	/* convert the number */
	while (num != 0 && counter < length) {
		remainder = num % 2;
		num /= 2;
		binaryNumber += remainder * i;
		i *= 10;
		counter++;
	}
	if (negative)
		binaryNumber *= -1;
	/*convert it to string*/
	sprintf(result, "%.0f", binaryNumber);
	if (counter == length) {
		return result;
	}
	/*if the number is negative - make two complementary*/
	cur = result;
	strcpy(result, strrev(result));
	while (counter++ < length)
		strcat(result, "0");
	if (negative) {
		while (*cur == '0')
			cur++;
		if (*cur == '1')
			cur++;
		while (*cur != 0) {
			if (*cur == '0') {
				*cur = '1';
				cur++;
			} else {
				*cur = '0';
				cur++;
			}
		}

	}
	strcpy(result, strrev(result));
	return result;
}

/* Split sentence to it words seperated with spaces, newline sign or comma */
/* Return an array holds all words */
char **splitSentenceToWords(char *sentence) {
	int n_spaces = 0;
	char ** res = NULL;
	char *p = NULL;
	char *str = (char *) malloc(strlen(sentence)); /* holds the original string the function got */
	if (!str) {
		fprintf(stderr, "Error:no memory room.\n");
		exit(1);
	}
	strcpy(str, sentence);
	p = strtok(str, " ,\t\n");

	while (p) {
		res = realloc(res, sizeof(char*) * ++n_spaces);

		if (res == NULL) {
			fprintf(stderr, "Error: no memory room.\n");
			exit(1);
		}
		res[n_spaces - 1] = p;
		p = strtok(NULL, " ,\t\n");
	}

	/* realloc one extra element for the last NULL */
	res = realloc(res, sizeof(char*) * (n_spaces + 1));
	if (res == NULL) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	res[n_spaces] = 0;
	return res;
}

/* Counts words in sentence */
/* Return number of words separated in spaces or comma */
int countWords(char *sentence) {
	int count = 0;
	char *cur;
	cur = sentence;

	FOREVER {
		while (*cur == ' ' || *cur == '	' || *cur == ',')
			cur++;

		if (*cur == 0)
			break;

		count++;

		while (*cur != 0 && *cur != ' ' && *cur != '	' && *cur != ',') {
			cur++;
		}
		while (*cur == ' ' || *cur == '	' || *cur == ',')
			cur++;
		if (*cur == 0 || *cur == '\n')
			break;
	}
	return count;
}

/* return the label name if the label is legal, NULL otherwise */
char * isLegalLabel(char *label, int lineNumber) {
	int c = ':';
	char *temp = label;
	char *labelEnd = strchr(temp, c); /*search for : of the end of legal label*/
	if (!labelEnd) {
		return NULL;
	}
	/* cut the ':' from the end of the label to store it */
	labelEnd[strlen(labelEnd) - 1] = '\0';

	if (isCommand(label)) {
		fprintf(stderr,
				"Error: a label with a Command name. label name: %s. (line number: %d)\n",
				label, lineNumber);
		return NULL;
	}
	if (isRegister(label)) {
		fprintf(stderr,
				"Error: a label with a Register name. label name: %s. (line number: %d)\n",
				label, lineNumber);
		return NULL;
	}
	return labelEnd;
}

/* Return if name is leading data line */
boolean isDataLine(char* name) {
	if (strcmp(name, DATA) == 0 || strcmp(name, STRING) == 0)
		return TRUE;
	else
		return FALSE;
}


/* Return if operand is type of dynamic operand. if true: store the label and the desire range */
boolean isDynamic(char *operand, char *res, int *start, int * end) {

	int firstNum = 0;
	int secondNum = 0;
	char *cur;
	int i = 0;
	char *label = (char*) malloc(sizeof(char) * MAX_LABLE_LENGTH);
	if (!label) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	cur = operand;
	/*escape spaces*/
	while (*cur == ' ' || *cur == '	')
		cur++;
	/*store the leader label*/
	while (isalpha(*cur)) {
		label[i++] = *cur;
		cur++;
	}
	if (*cur == '[') {
		cur++;
	} else {
		return FALSE;
	}
	if (!isdigit(*cur)) {
		return FALSE;
	}

	/*store the first range number*/
	while (isdigit(*cur)) {
		firstNum = firstNum * 10 + *cur - '0';
		cur++;
	}
	if (*cur == '-')
		cur++;
	else {
		return FALSE;
	}

	if (!isdigit(*cur))
		return FALSE;
	/*store the second range number*/
	while (isdigit(*cur)) {
		secondNum = secondNum * 10 + *cur - '0';
		cur++;
	}
	if (*cur == ']')
		cur++;
	else
		return FALSE;
	/*escape spaces*/
	while (*cur == ' ' || *cur == ' ')
		cur++;

	if (*cur != '\0')
		return FALSE;
	/*escape spaces*/
	while (*cur == ' ' || *cur == ' ')
		cur++;
	strcpy(res, label);
	*start = firstNum;
	*end = secondNum;

	return TRUE;
}


/* Return a decimal number as a string if it spelled correctly, NULL otherwise*/
char* isLegalNumber(char *number, int lineNumber) {

	char *cur;
	int i = 0, binaryNumber;
	char *result = (char*) malloc(sizeof(char*) * LONG_WORD_LENGTH);
	if (!result) {
		fprintf(stderr, "Error: no memory room.\n");
		exit(1);
	}
	cur = number;
	/*escape spaces*/
	while (*cur == ' ' || *cur == '	')
		cur++;
	if (*cur == '#')
		cur++;
	else
		return NULL;
	/*collect the sign*/
	if ((*cur == '+') || (*cur == '-')) {
		result[i++] = *cur;
		cur++;
	}
	if (!isdigit(*cur)) {
		return NULL;
	}
	/*collect the number*/
	while (isdigit(*cur)) {
		result[i++] = *cur;
		cur++;
	}
	/*escape spaces*/
	while (*cur == ' ' || *cur == '	')
		cur++;
	if (*cur != '\0') {
		fprintf(stderr,
				"Error: detected a number operand with syntax errors. operand name is: %s. (line number: %d)\n",
				number, lineNumber);
		return NULL;
	}
	/*escape spaces*/
	while (*cur == ' ' || *cur == '	')
		cur++;
	binaryNumber = atoi(result);
	strcpy(result, toBinary(binaryNumber, SHORT_WORD_LENGTH));
	strcat(result, "00"); /* concat the ERA */

	return result;
}

/* Return if given label is already exist in the labels table */
boolean isLabelExist(char *label) {
	int i;
	for (i = 0; i <= curLabel; i++) {
		if (strcmp(label, labelsTable[i].name) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

/* Return if label is stored as an entry label */
boolean isEntryLabel(char *label) {
	int i;
	for (i = 0; i <= curLabel; i++) {
		if ((strcmp(label, labelsTable[i].name) == 0) && labelsTable[i].isEntry)
			return TRUE;
	}
	return FALSE;
}

/* Return if if name is leader of external or entry line */
boolean isExternLine(char *name) {
	if (strcmp(name, ENTRY) == 0 || strcmp(name, EXTERN) == 0)
		return TRUE;
	else
		return FALSE;
}

/* check if the given name is a register, if true, return the register name, NULL otherwise */
char * isRegister(char *name) {
	int i;
	for (i = 0; reg[i].code != NULL; i++) {
		if (strcmp(name, reg[i].name) == 0)
			break;
	}
	return reg[i].code;
}

/* check if the given name is a command, if true, return the command name, NULL otherwise */
char* isCommand(char* name) {
	int i;
	for (i = 0; commands[i].code != NULL; i++) {
		if (strcmp(name, commands[i].name) == 0) {
			break;
		}
	}
	return commands[i].name;
}
