all: exercise1
exercise1: exercise1.o fifo_pipes_lib.o files.o
	gcc -Wall -g exercise1.o fifo_pipes_lib.o files.o -o exercise1 -lpthread
exercise1.o: exercise1.c fifo_pipes_lib.h files.h
	gcc -Wall -g -c exercise1.c -lpthread
fifo_pipes_lib.o: fifo_pipes_lib.c fifo_pipes_lib.h
	gcc -Wall -g -c fifo_pipes_lib.c -lpthread
files.o: files.c files.h
	gcc -Wall -g -c files.c