#include "fifo_pipes_lib.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char **argv)
{   
    READ_ARGS read_arguments;
    WRITE_ARGS write_arguments;

    /* Start. */
    printf("[*] Starting program.\n");
    sleep(2);

    /* Init struct array. */
    int i;
    for (i = 0; i < NUM_OF_PIPES; i++) {
        pipe_info[i].pipe_root    = NULL; 
        pipe_info[i].write_ptr    = NULL;
        pipe_info[i].read_ptr     = NULL;
        pipe_info[i].isEmpty      = 1;
        pipe_info[i].write_access = 1;
        pipe_info[i].ID           = 0;
    }

    /* Pipes creation. */
    for (i = 0; i < NUM_OF_PIPES; i++) {
        pipe_info[i].ID = pipe_open(SIZE_OF_PIPE);
    }
    write_arguments.array = reading_from_file("/home/dusu/Desktop/Sxoli/3o-etos/tautoxronos/project1/exercise_1/file2.txt");
    write_arguments.array2 = (char *) calloc(strlen(write_arguments.array), sizeof(char));
    if (!write_arguments.array2) {
        fprintf(stderr, "[!] Error: calloc() failed.\n");
        exit(-1);
    }
    
    read_arguments.array = (char *) calloc(strlen(write_arguments.array), sizeof(char));
    if (!read_arguments.array) {
        fprintf(stderr, "[!] Error: calloc() failed.\n");
        exit(-1);
    }

    /* Now we create our two threads. */
    printf("[*] Creating threads.\n");
    sleep(2);
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, &write_func, &write_arguments);
    pthread_create(&thread2, NULL, &read_func, &read_arguments);
    
    /* main has to wait for the threads to end. */
    while (!isFinished_write) sleep(0.0001); 
    while (!isFinished_read) sleep(0.0001); 

    /* Frees. */
    printf("[*] Freeing dynamicaly allocated memory.\n");
    sleep(2);
    free(write_arguments.array);
    free(read_arguments.array);
    free(write_arguments.array2);
    free(read_arguments.array2);

    /* Exiting. */
    printf("[*] Done.\n");
    return 0;
}