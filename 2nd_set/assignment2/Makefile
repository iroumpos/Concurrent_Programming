all: prime
prime: prime.o sem_lib.o
	gcc -Wall -g prime.o sem_lib.o -o prime -lpthread -lm
prime.o: prime.c sem_lib.h
	gcc -Wall -g -c prime.c -lpthread 
sem_lib.o: sem_lib.c sem_lib.h
	gcc -Wall -g -c sem_lib.c -lpthread
clean:
	rm *.o