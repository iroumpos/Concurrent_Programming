#include "sem_lib.h"
#include <pthread.h>

#define TIME_OF_THE_RIDE 5

typedef struct train {
    int     num_of_passengers;
    int     maxNum;
    int     currNum;
    mysem_t sem_t;  // train semaphore
    mysem_t sem_p;  // passenger semaphore
    mysem_t done;
} my_train;


void* train_fun(void* args) {
    my_train *t_arguments = (my_train*) args;
    
    while(mysem_down(&t_arguments->sem_t)) {
        printf("[*] Train starting its ride with %d passengers.\n", t_arguments->currNum);
        sleep(TIME_OF_THE_RIDE);
        printf("[*] Current ride is finished.\n\n");
        sleep(2);
        t_arguments->currNum = 0; /* Initialize the number of passengers in the train number. */

        /* Train starts only if it's full. */
        /* This block of code rules out the number of passengers that cannot fill up all the train seats. */
        if (t_arguments->num_of_passengers < t_arguments->maxNum) {
            printf("[*] Train has to be full in order to start its ride.\n");
            sleep(1);
            printf("[*] %d passengers are going to be left out.\n", t_arguments->num_of_passengers);
            sleep(1);
            mysem_up(&t_arguments->done);
            return NULL;
        }

        if (!t_arguments->num_of_passengers) {
            printf("[*] Train is set out of order for today.\n");
            sleep(1);
            mysem_up(&t_arguments->done);   
            return NULL;
        }

        /* Send entering signal to the next set of passengers. */
        mysem_up(&t_arguments->sem_p);
    }
    return NULL;
}

void* pass_fun(void* args){
    my_train *p_arguments = (my_train*)args;

    mysem_down(&p_arguments->sem_p);
    printf("Passenger %d enters the train: (%d/%d)\n", p_arguments->currNum, p_arguments->currNum+1, p_arguments->maxNum);
    sleep(1);
    p_arguments->num_of_passengers--;
    p_arguments->currNum++;
    if (!p_arguments->num_of_passengers) { /* I am the last to enter. */
        mysem_up(&p_arguments->sem_t); /* Start the train signal. */
        return NULL;
    }
    if (p_arguments->currNum == p_arguments->maxNum)
        mysem_up(&p_arguments->sem_t); /* Start the train. */
    else 
        mysem_up(&p_arguments->sem_p); /* Continue with the next passenger. */
    
    return NULL;
}

int main(int argc,char* argv[]){
    my_train   train;
    int        num;
    pthread_t  train_t;

    printf("[*] Starting program.\n");
    sleep(2);

    if(argc != 2){
        fprintf(stderr,"[!] ERROR: usage: ./train <max_num_of_seats>\n");
        exit(EXIT_FAILURE);
    }
    printf("[*] Initializing structures and variables.\n");
    sleep(2);

    train.maxNum  = atoi(argv[1]);
    train.currNum = 0;
    mysem_init(&train.sem_t, 0);
    mysem_init(&train.sem_p, 1);
    mysem_init(&train.done, 0);
    
    printf("[*] Enter number of passengers: ");
    scanf("%d", &num);    
        
    printf("[*] Creating train thread\n");
    sleep(2);
    pthread_create(&train_t, NULL, train_fun, (void*) &train);
    
    train.num_of_passengers = num;
    pthread_t pass[train.num_of_passengers];
    printf("[*] Creating passengers threads\n");
    sleep(2);
    for(int i = 0; i < num; i++) {
        pthread_create(&pass[i], NULL, pass_fun, (void*) &train);
    }
    mysem_down(&train.done); /* Wait until everything is done. */
    
    return 0;
}