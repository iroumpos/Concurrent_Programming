#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "uthreads.h"

void myfun(){
    int j;
    for (j = 0; j < 100; j++) {
        printf("Thread 4 %d\n", j);
    }
}

int main(int argc,char* argv[]){
    int total_threads;
    thread_t* threads = NULL;
    
    signal(SIGALRM,scheduler);
    total_threads = atoi(argv[1]);
    
    mythreads_init();
    
    for(int i=0; i<total_threads; i++){
        mythreads_create(&threads[i],myfun,NULL);
    }
    
    printf("Starting timer\n");
    create_CPU_clock();
    swapcontext(mainctx->context,FIFO_scheduler->head->next->context);
    

    return 0;
}