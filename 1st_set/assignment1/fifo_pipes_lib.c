#include "fifo_pipes_lib.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

int      id = 0;
int      num_of_iterations;
PIPE_T   pipe_info[NUM_OF_PIPES];
int      isFinished_write = 0;
int      isFinished_read = 0;
static volatile bool want_write = false;
static volatile bool want_read = false;
static volatile char thread_turn = 'w';


void nop_func() {
    return;
}

int pipe_open(int size) {
    PIPE *pipe_root;
    init_list(&pipe_root);
    
    int i;
    for (i = 0; i < size; i++) {
        create_node(&pipe_root);
    }
    for (i = 0; i < NUM_OF_PIPES; i++) {
        if (pipe_info[i].isEmpty == 1) break;
    }
    pipe_info[i].pipe_root = pipe_root;
    pipe_info[i].ID = ++id;
    pipe_info[i].isEmpty = 0;
    pipe_info[i].write_access = 1;
    pipe_info[i].write_ptr = pipe_root;
    pipe_info[i].read_ptr = pipe_root;

    return pipe_info[i].ID;
}

int pipe_write(int p, char c) {
    PIPE *tmp;
    int i; 
    for (i = 0; i < NUM_OF_PIPES; i++) {
        if (p == pipe_info[i].ID) { 
            if (pipe_info[i].write_ptr->c == '\0') {
                pipe_info[i].write_ptr->c = c;
                pipe_info[i].write_ptr = pipe_info[i].write_ptr->next;
                return 1;
            }
            tmp = pipe_info[i].pipe_root;
            do {
                if (tmp->c == '\0') break;
                tmp = tmp->next;
            } while(tmp != pipe_info[i].pipe_root);
            pipe_info[i].write_ptr = tmp;
            pipe_info[i].write_ptr->c = c;
            pipe_info[i].write_ptr = pipe_info[i].write_ptr->next;
            return 1;
        } 
    }
    return -1;
}

void *write_func(void *args) {
    WRITE_ARGS *arguments = (WRITE_ARGS *) args;
    int i, j = 0;

    printf("\n--- 1st Stage ---\n");
    //arguments->array = reading_from_file("/home/dusu/Desktop/Sxoli/3o-etos/tautoxronos/project1/file2.txt");
    num_of_iterations = strlen(arguments->array);
    for (i = 0; i < num_of_iterations; i++) {
        want_write = true;
        thread_turn = 'r';
        while (want_read && thread_turn == 'r');
        
        printf("\n");
        printf("w-b: ");
        print_pipe(pipe_info[0].pipe_root);
        pipe_write(pipe_info[0].ID, arguments->array[i]);
        printf("\nw-a: ");
        print_pipe(pipe_info[0].pipe_root);         
        printf("\n");

        want_write = false;
    }

    while (is_pipe_empty(pipe_info[1].pipe_root)) { sleep(0.001); }
    for (i = 0; i < num_of_iterations; i++) {
        want_write = true;
        thread_turn = 'r';
        while (want_read && thread_turn == 'r');

        printf("\n");
        printf("r-b: ");
        print_pipe(pipe_info[1].pipe_root);
        pipe_read(pipe_info[1].ID, &(arguments->array2[j++]));
        printf("\nr-a: ");
        print_pipe(pipe_info[1].pipe_root);         
        printf("\n");
        want_write = false;
    }
    creating_copy_file("/home/dusu/Desktop/Sxoli/3o-etos/tautoxronos/project1/exercise_1/file2_copy", arguments->array2);
    pipe_writeDone(pipe_info[1].ID);
    isFinished_write = 1;

    return NULL;
}

void *read_func(void *args) {
    READ_ARGS *arguments = (READ_ARGS*) args;

    int i, j = 0;
    while (is_pipe_empty(pipe_info[0].pipe_root)) { nop_func(); } /* in order for 'num_of_iterations' to grab the right value. */
    for (i = 0; i < num_of_iterations; i++) {
        want_read = true;
        thread_turn = 'w';
        while (want_write && thread_turn == 'w');

        printf("\n");
        printf("r-b: ");
        print_pipe(pipe_info[0].pipe_root);         
        pipe_read(pipe_info[0].ID, &(arguments->array[j++]));
        printf("\nr-a: ");
        print_pipe(pipe_info[0].pipe_root);         
        printf("\n");
        want_read = false;
    }
    /* Here we know that 'reading' has come to an end. */
    creating_copy_file("/home/dusu/Desktop/Sxoli/3o-etos/tautoxronos/project1/exercise_1/file_copy", arguments->array);
    arguments->array2 = reading_from_file("/home/dusu/Desktop/Sxoli/3o-etos/tautoxronos/project1/exercise_1/file_copy");
    pipe_writeDone(pipe_info[0].ID);

    printf("\n--- 2nd Stage ---\n");
    for (i = 0; i < strlen(arguments->array2); i++) {
        want_read = true;
        thread_turn = 'w';
        while (want_write && thread_turn == 'w');

        printf("\n");
        printf("w-b: ");
        print_pipe(pipe_info[1].pipe_root);         
        pipe_write(pipe_info[1].ID, arguments->array2[i]);
        printf("\nw-a: ");
        print_pipe(pipe_info[1].pipe_root);         
        printf("\n");
        want_read = false;
    }
    isFinished_read = 1;
    return NULL;
}

int pipe_writeDone(int p) {
    int i;
    for (i = 0; i < NUM_OF_PIPES; i++) {
        if (p == pipe_info[i].ID && pipe_info[i].write_access == 1) {
            free_pipe(p);
            pipe_info[i].write_access = 0;
            return 1; /* Success. */
        }
    }
    return -1; /* Failure. */
}

void free_pipe(int p) {
    PIPE *tmp, *current;
    
    int i;
    for (i = 0; i < NUM_OF_PIPES; i++) {
        if (p == pipe_info[i].ID) {
            current = pipe_info[i].pipe_root;
            while(current != pipe_info[i].pipe_root) {
                tmp = current->next;
                free(current);
                current = tmp;
            }
            free(pipe_info[i].pipe_root);
            pipe_info[i].pipe_root = NULL;
        }
    }
}


int pipe_read(int p, char *c) {
    int i;
    for (i = 0; i < NUM_OF_PIPES; i++) {
        if (p == pipe_info[i].ID) {
            while(1) {
                if (pipe_info[i].write_access == 0 && is_pipe_empty(pipe_info[i].pipe_root)) {
                    printf("[!] Write access: locked!\nReady to destroy the pipe.\n");
                    sleep(2);
                    pipe_writeDone(p);
                    return 0;
                }
                if (pipe_info[i].read_ptr->c == '\0') sleep(2);
                else break;
            }
            /* Reading. */
            *c = pipe_info[i].read_ptr->c;
            pipe_info[i].read_ptr->c = '\0';
            pipe_info[i].read_ptr = pipe_info[i].read_ptr->next;
            return 1;
        }
    }
    return -1;
}

int is_pipe_empty(PIPE *pipe_root) {
    PIPE *tmp = pipe_root;
    do {
        if (tmp->c != '\0') return 0;
        tmp = tmp->next;
    } while (tmp != pipe_root);
    return 1;
}

void init_list(PIPE **pipe_root) {
    *pipe_root = NULL;
}

void create_node(PIPE **pipe_root) {
    PIPE *new_node = (PIPE *) malloc(sizeof(PIPE));
    if (!(*pipe_root)) {
        *pipe_root = new_node;
        new_node->c = '\0';
        new_node->next = *pipe_root;
        return;
    }
    PIPE *tmp = *pipe_root;
    while(tmp->next != *pipe_root) {
        tmp = tmp->next;
    } 
    tmp->next = new_node;
    new_node->c = '\0';
    new_node->next = *pipe_root;
    return;
}

void print_pipe(PIPE *pipe_root) {
    PIPE *tmp = pipe_root;
    if (pipe_root) {
        do {
            if (tmp->c == '\0') {
                printf("- ");
                tmp = tmp->next;
            }
            else {
                printf("%c ", tmp->c);
                tmp = tmp->next;
            }
        } while(tmp != pipe_root);
    }
}