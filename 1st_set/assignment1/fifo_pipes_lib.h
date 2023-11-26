#ifndef __FIFO_PIPES_LIB_H_
#define __FIFO_PIPES_LIB_H_

#include <stdio.h>
#include <stdbool.h>

/* Structs. */
typedef struct pipe {
    char c;
    struct pipe *next;
} PIPE;

typedef struct pipe_t {
    PIPE *pipe_root;
    PIPE *write_ptr;
    PIPE *read_ptr;
    int isEmpty;
    int write_access;
    int ID;
} PIPE_T;

typedef struct read_args {
    char *array;
    char *array2;
} READ_ARGS;

typedef struct write_args {
    char *array;
    char *array2;
} WRITE_ARGS;

/* Functions. */
extern int   pipe_open(int size);
extern int   pipe_write(int p, char c);
extern void *write_func(void *args);
extern int   pipe_writeDone(int p);
extern void *read_func(void *args);
extern int   pipe_read(int p, char *c);
extern int   is_pipe_empty(PIPE *pipe_root);
extern void  print_pipe(PIPE *pipe_root);
extern void  free_pipe(int p);
extern void  nop_func();


/* Other functions -> create another library. */
extern void init_list(PIPE **pipe_root);
extern void create_node(PIPE **pipe_root);


/* Variables - arrays. */
#define NUM_OF_PIPES 2
#define SIZE_OF_PIPE 64
extern PIPE_T pipe_info[NUM_OF_PIPES];
extern int isFinished_write;
extern int isFinished_read;
static volatile bool want_write;
static volatile bool want_read;
static volatile char thread_turn;

#endif
