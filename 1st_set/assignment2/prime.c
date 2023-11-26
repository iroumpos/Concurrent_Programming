#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


/* Functions' Prototypes. */
void *work(void *args);
int prime(int number);

/* Structs. */
typedef struct thread_info {
    int number;
    int signal; /* 0: worker not available, 1: worker available, 2: termination signal from main, 3: termination signal from worker. */
} THREAD_INFO;

int main(int argc, char **argv) {
    int num_of_workers, i, num;
    THREAD_INFO *threads_info;
    
    printf("[*] Starting program.\n");
    sleep(2);
    
    if (argc != 2) {
        fprintf(stderr, "[!] ERROR: usage: ./exercise2 <num_of_workers>\n");
        exit(EXIT_FAILURE);
    }
    
    num_of_workers = atoi(argv[1]);
    threads_info = (THREAD_INFO *) malloc(sizeof(THREAD_INFO) * num_of_workers);
    pthread_t thread_IDs[num_of_workers];
    for(i = 0; i < num_of_workers; i++) {
        threads_info[i].signal = 1;
        pthread_create(&thread_IDs[i], NULL, work, (void *) &threads_info[i]);
    }

    printf("[*] Ready to read from stdin.\n\n");
    sleep(2);
    while (1) {
        if (scanf("%d", &num) == EOF) break;
        do {
            for (i = 0; i < num_of_workers && threads_info[i].signal == 0; i++);
        } while (i == num_of_workers);
        
        threads_info[i].number = num;
        threads_info[i].signal = 0;
    }
    
    /* Waiting for all workers to become available. */
    i = 0;
    while (i != num_of_workers) 
        for (i = 0; i < num_of_workers && threads_info[i].signal == 1; i++);
    /* Notify workers to temrinate. */
    for (i = 0; i < num_of_workers; i++) threads_info[i].signal = 2;
    /* Wait for all workers to temrinate. */
    i = 0;
    while (i != num_of_workers) for (i = 0; i < num_of_workers && threads_info[i].signal == 3; i++);
    
    free(threads_info);

    printf("\n[*] Done.\n");
    sleep(1);
    return 0;
}

void *work(void *args) {
    THREAD_INFO *arguments = (THREAD_INFO *) args;
    while(1) {
        if (arguments->signal == 0) {
            if (prime(arguments->number)) 
                printf("%d -> prime\n", arguments->number);
            else 
                printf("%d -> not prime\n", arguments->number);
            arguments->signal = 1;
        }
        else if (arguments->signal == 2) break;
    }
    arguments->signal = 3; /* termination signal. */
    return NULL;
}

int prime(int number)
{
	int i;
    for (i = 2; i*i <= number; i++) {
        if (number % i == 0) return 0;
    }
    if (number <= 1) return 0;
    return 1;
}