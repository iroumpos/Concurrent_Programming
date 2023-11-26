all: program
program: program.o uthreads.o
	gcc -Wall -g program.o uthreads.o -o program 
program.o: program.c uthreads.h
	gcc -Wall -g -c program.c
uthreads.o: uthreads.c uthreads.h
	gcc -Wall -g -c uthreads.c
clean: 
	rm *.o

	