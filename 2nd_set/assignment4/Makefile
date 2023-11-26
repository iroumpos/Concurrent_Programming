all: train
train: train.o sem_lib.o 
	gcc -Wall -g train.o sem_lib.o -o train -lpthread
train.o: train.c sem_lib.h 
	gcc -Wall -g -c train.c 
sem_lib.o: sem_lib.c sem_lib.h
	gcc -Wall -g -c sem_lib.c
clean: 
	rm  *.o
