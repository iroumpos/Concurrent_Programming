all: bridge
bridge: bridge.o sem_lib.o
	gcc -Wall -g bridge.o sem_lib.o -o bridge -lpthread
bridge.o: bridge.c sem_lib.h
	gcc -Wall -g -c bridge.c -lpthread 
sem_lib.o: sem_lib.c sem_lib.h
	gcc -Wall -g -c sem_lib.c -lpthread
clean:
	rm *.o