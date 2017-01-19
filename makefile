assembler:util.o firstProcess.o secondProcess.o main.o
	gcc -g -Wall -pedantic -ansi util.o firstProcess.o secondProcess.o main.o -o assembler
util.o:util.c
	gcc -c -Wall -pedantic -ansi util.c -o util.o
firstProcess.o:firstProcess.c
	gcc -c -Wall -pedantic -ansi firstProcess.c -o firstProcess.o
secondProcess.o:secondProcess.c
	gcc -c -Wall -pedantic -ansi secondProcess.c -o secondProcess.o
main.o:main.c
	gcc -c -Wall -pedantic -ansi main.c -o main.o
clean:
	rm assembler
