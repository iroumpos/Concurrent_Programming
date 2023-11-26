#include "sem_lib.h"
#include <pthread.h>


void mysem_print(mysem_t *s) {
    int retVal;

    if ((retVal = semctl(s->id, 0, GETVAL)) == -1) {
        fprintf(stderr, "In file %s, line %d ", __FILE__, __LINE__);
        perror("semctl error");
        exit(1);
    }
    printf("%d\n", retVal);
}

void *func(void *args) {
    mysem_t *s = (mysem_t *) args;
    long int pid;

    pid = pthread_self() % 1000;

    printf("%ld started\n", pid);

    mysem_down(s);
    printf("%ld subtracted sem by one, sleeping\n", pid);
    sleep(5);
    printf("%ld is awake, adding one to sem\n", pid);
    mysem_up(s);

    printf("%ld is exiting\n", pid);

    return (void *)NULL;
}

int main(int argc, char *argv[]) {
    mysem_t s;
    pthread_t pid[3];    
    int i;

    mysem_init(&s, 1);

    for(i= 0; i < 3; i++) {
        pthread_create(&pid[i], NULL, func, &s);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(pid[i], NULL);
    }

    return 0;
}