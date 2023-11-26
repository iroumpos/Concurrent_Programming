all: ex1
ex1: ex1.o sem_lib.o 
	gcc -Wall -g ex1.o sem_lib.o -o ex1 -lpthread
ex1.o: ex1.c sem_lib.h 
	gcc -Wall -g -c ex1.c 
sem_lib.o: sem_lib.c sem_lib.h
	gcc -Wall -g -c sem_lib.c
clean: 
	rm  *.o
