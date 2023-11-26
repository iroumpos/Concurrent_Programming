#include "sem_lib.h"
#include <pthread.h>
#include <math.h>

/* Functions' Prototypes. */
void *work(void *args);
int   prime(int number);

/* Structs. */
typedef struct thread_info {
    int     number;
    bool    main_done;
    mysem_t getNextNumber;
    mysem_t isAvailable;
    mysem_t process;
    mysem_t cont; /* continue. */
    mysem_t worker_done;
} THREAD_INFO;

int main(int argc, char **argv) {
    int num_of_workers, i;
    THREAD_INFO threads_info;
    
    printf("[*] Starting program.\n");
    sleep(2);
    if (argc != 2) {
        fprintf(stderr, "[!] ERROR: usage: ./exercise2 <num_of_workers>\n");
        exit(EXIT_FAILURE);
    }
    
    printf("[*] Initializing semaphores and variables.\n");
    sleep(2);
    num_of_workers = atoi(argv[1]);
    pthread_t thread_IDs[num_of_workers];
    mysem_init(&threads_info.getNextNumber, 0);
    mysem_init(&threads_info.isAvailable, 0);
    mysem_init(&threads_info.process, 0);
    mysem_init(&threads_info.cont, 0);
    mysem_init(&threads_info.worker_done, 0);
    threads_info.main_done = false;
    for(i = 0; i < num_of_workers; i++) {
        pthread_create(&thread_IDs[i], NULL, &work, (void *) &threads_info);
    }

    printf("[*] Ready to read from stdin.\n\n");
    while (1) {
        if (scanf("%d", &threads_info.number) == EOF) break;
        mysem_up(&threads_info.cont);
        mysem_down(&threads_info.isAvailable);   /* Waiting for a worker to become available. */
        mysem_up(&threads_info.process);
        mysem_down(&threads_info.getNextNumber);
    }
    threads_info.main_done = true;
    
    /* Notify workers to terminate. */
    for (i = 0; i < num_of_workers; i++)
        while(!mysem_up(&threads_info.cont));
    
    /* Wait for all workers to terminate. */
    for (i = 0; i < num_of_workers; i++) 
        mysem_down(&threads_info.worker_done);

    sleep(1);
    printf("\n[*] Done.\n");
    sleep(2);
    return 0;
}

void *work(void *args) {
    THREAD_INFO *argument = (THREAD_INFO *) args;
    while (mysem_down(&argument->cont) && !argument->main_done) {
        mysem_up(&argument->isAvailable);
        mysem_down(&argument->process);
        int temp_number = argument->number;
        mysem_up(&argument->getNextNumber);
        if (prime(temp_number)) 
            printf("%d: prime number\n", temp_number);
        else
            printf("%d: not prime number\n", temp_number);
    }
    while(!mysem_up(&argument->worker_done)); /* Send a termination signal. */
    return NULL;
}

int prime(int number)
{
	int i;
    for (i = 2; i <= sqrt(number); i++) {
        if (number % i == 0) return 0;
    }
    if (number <= 1) return 0;
    return 1;
}
