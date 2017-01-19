/*
 * main.c
 *
 * by: Shlomi Gottlieb
 * ID: 200505717
 *
 * by: Adiel Bin Nun
 * ID: 319028288
 */
#include "main.h"

/* Collects all labels that declared as entry in the second process. save them in special array */
void updateEntryLabels() {
	int i;

	for (i = 0; i < curLabel; i++) {
		if (labelsTable[i].isEntry) {
			labelInfo newLabel;
			newLabel.address = labelsTable[i].address;
			newLabel.isEntry = TRUE;
			strcpy(newLabel.name, labelsTable[i].name);
			entryLabels[curEntryLabel++] = newLabel;
		}
	}

}

/* Gets all lines from the data structure. prints all of them to the target files, in special octal encoding */
void printAllLines() {
	int i;
	FILE *fp, *ent, *ext;
	IC -= FIRST_ADDRESS; /*numbers of command lines*/
	/* DC -= 1;  ignore the last increment */
	strcat(extFileName, obFileName);
	strcat(entFileName, obFileName);
	strcat(entFileName, ".ent");
	strcat(extFileName, ".ext");
	strcat(obFileName, ".ob");
	fp = fopen(obFileName, "a");
	if (!fp) {
		fprintf(stderr, "Error: can't write to target files.\n");
		exit(1);
	}
	fprintf(fp, "%s\t%s\n", toSpecialOctal(toBinary(IC, LONG_WORD_LENGTH)),
			toSpecialOctal(toBinary(DC, LONG_WORD_LENGTH)));
	fprintf(fp, "\n\n");

	for (i = 0; i < curLine; i++) {
		fprintf(fp, "%s\t%s\n",
				(toSpecialOctal(
						toBinary(linesTabel[i].address, LONG_WORD_LENGTH))),
				(toSpecialOctal(linesTabel[i].savedWord)));
	}

	for (i = 0; i < curDataLine; i++) {
		fprintf(fp, "%s\t%s\n",
				(toSpecialOctal(
						toBinary(DataWordsTable[i].address, LONG_WORD_LENGTH))),
				(toSpecialOctal(DataWordsTable[i].savedWord)));
	}
	fclose(fp);
	printf("Info: object file created.\n");
	if (curEntryLabel > 0) {
		ent = fopen(entFileName, "a");
		if (!ent) {
			fprintf(stderr, "Error: can't write to target files.\n");
			exit(1);
		}
		/* Prints all entry labels and their address to .ent file  */
		for (i = 0; i < curEntryLabel; i++) {
			fprintf(ent, "%s\t%s\n", entryLabels[i].name,
					toSpecialOctal(
							toBinary(entryLabels[i].address, LONG_WORD_LENGTH)));
		}
		fclose(ent);
		printf("Info: entry file created.\n");
	}
	if (curExternLabel > 0) {
		ext = fopen(extFileName, "a");
		if (!ext) {
			fprintf(stderr, "Error: can't write to target files.\n");
			exit(1);
		}
		/* Prints all external labels and their address to .ext file */
		for (i = 0; i < curExternLabel; i++) {

			fprintf(ext, "%s\t", externLabel[i].name);
			fprintf(ext, "%s\n",
					toSpecialOctal(
							toBinary(externLabel[i].address, LONG_WORD_LENGTH)));

		}
		fclose(ext);
		printf("Info: external file created.\nAll files created successfully.\n\n");
	}
}

/* This method read a whole line from the input stream "file" */
/* Initials the line, read it and then make it the first and second process' */
void readFiles(FILE *file) {
	int i;
	static int lineNumber = 1;
	char buffer[MAX_INPUT];
	while (fgets(buffer, MAX_INPUT, file)) { /*if there is another line */
		lineInfo line;
		line.lineNumber = lineNumber++;
		line.isError = FALSE;
		line.lineStr = buffer;
		/* make the first process */
		firstProcess(&line);
	}/* If there was an errors in the first process than stop the program */
	for (i = 0; i < curCommandLine; i++) {
		if (CommandWordsTable[i].isError == TRUE) {
			fprintf(stderr, "You have syntax errors .files did not create.\n");
			return;
		}
	}
	for (i = 0; i < curDataLine; i++) {
		if (DataWordsTable[i].isError == TRUE) {
			fprintf(stderr, "You have syntax errors .files did not create.\n");
			return;
		}
	}


	IC -= 1;
	/* update the address of all data lines */
	for (i = 0; i < curDataLine; i++) {
		DataWordsTable[i].address = DataWordsTable[i].address + IC;
	}
	/* update the address of all labels of data lines */
	for (i = 0; i < curLabel; i++) {
		if (labelsTable[i].isData)
			labelsTable[i].address = labelsTable[i].address + IC;
	}
	/* Back to beginning of file and make second process */
	rewind(file);
	lineNumber = 1;
	IC = FIRST_ADDRESS;
	while (fgets(buffer, MAX_INPUT, file)) { /*if there is another line */
		lineInfo line;
		line.lineNumber = lineNumber++;
		line.isError = FALSE;
		line.lineStr = buffer;

		secondProcess(&line);
	}
	updateEntryLabels();
	printAllLines();
	lineNumber = 1;
}

/* Parse the file for read input */
void parseFiles(char *fileName) {
	FILE *fp;
	/* Reset all variables and data structures */
	IC = FIRST_ADDRESS;
	DC = 0;
	curLine = 0;
	curCommandLine = 0;
	curDataLine = 0;
	curLabel = 0;
	curEntryLabel = 0;
	curExternLabel = 0;
	memset(&CommandWordsTable, 0, sizeof(CommandWordsTable));
	memset(&DataWordsTable, 0, sizeof(DataWordsTable));
	memset(&linesTabel, 0, sizeof(linesTabel));
	memset(&labelsTable, 0, sizeof(labelsTable));
	memset(&entryLabels, 0, sizeof(entryLabels));
	memset(&externLabel, 0, sizeof(externLabel));
	memset(obFileName, 0, sizeof(255));
	memset(extFileName, 0, sizeof(char) * 255);
	memset(entFileName, 0, sizeof(char) * 255);

	fp = (fopen(fileName, "r"));
	if (fp) {
		strcpy(obFileName, fileName);
		readFiles(fp);
		fclose(fp);
	} else {
		strcat(fileName, ".as");
		fp = (fopen(fileName, "r"));
		if (fp) {
			strcpy(obFileName, fileName);
			readFiles(fp);
			fclose(fp);
		} else {
			fprintf(stderr, "Error: %s is invalid file name or it protected\n",
					fileName);
		}
	}
}

int main(int argc, char * argv[]) {
	int c;
	if (argc < 2) {
		fprintf(stdin, "Info: no file names were observed.\n");
		return 1;
	}
	for (c = 1; c < argc; c++) {
		parseFiles(argv[c]);
	}
	return 0;
}

