#ifndef __SEM_LIB_H_
#define __SEM_LIB_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>


typedef struct mysem{
    int id;
    bool if_init;
}mysem_t;

union semun{
    int val;
    struct semid_de *info;
    unsigned short *vals;
};

int mysem_init(mysem_t *s,int n);
int mysem_down(mysem_t *s);
int mysem_up(mysem_t *s);
int mysem_destroy(mysem_t *s);


#endif