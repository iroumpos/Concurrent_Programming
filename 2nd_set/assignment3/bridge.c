#include "sem_lib.h"
#include <pthread.h>

#define BLUE          0
#define RED           1 
#define SLEEP_TIME    2
#define CROSSING_TIME 3

/* Structs. */
typedef struct bridge {
    int     type;
    int     curr_direction;      /* where cars coming from. */
    int     num_of_cars;         /* # of cars on the bridge. */
    int     max_cars;
    int     blue_cars_count;
    int     red_cars_count;
    mysem_t isDone;
    mysem_t blue_sem;
    mysem_t red_sem;
    mysem_t count_lock_blue;
    mysem_t count_lock_red;
    mysem_t bridge_lock;
} BRIDGE;

/* Protypes. */
void *car_func(void*args);
int   is_safe(BRIDGE *arg);

int main(int argc, char **argv) {
    BRIDGE b;
    int    cars_coming;
    int    main_sleep, i;
    int    total_cars = 0;

    printf("[*] Starting program.\n");
    sleep(SLEEP_TIME);

    if (argc != 2) {
        fprintf(stderr, "[!] Error: usage: ./bridge <# of max cars allowed>\n");
        exit(EXIT_FAILURE);
    }
    
    /* Inits. */
    printf("[*] Initializing structures and semaphores.\n");
    sleep(SLEEP_TIME);
    b.max_cars = atoi(argv[1]);
    b.type = BLUE;
    b.curr_direction = BLUE;
    b.num_of_cars = 1;
    b.blue_cars_count = 0;
    b.red_cars_count = 0;
    mysem_init(&b.isDone, 0);
    mysem_init(&b.blue_sem, 1);   
    mysem_init(&b.red_sem, 1);  
    mysem_init(&b.bridge_lock, 1);
    mysem_init(&b.count_lock_blue, 1);
    mysem_init(&b.count_lock_red, 1);

    printf("[*] Creating threads.\n");
    while (scanf("%d:%d:%d", &b.type, &cars_coming, &main_sleep) != EOF) {
        pthread_t car_threads[cars_coming];
        total_cars += cars_coming;
        for (i = 0; i < cars_coming; i++) {
            pthread_create(&car_threads[i], NULL, car_func, (void *) &b);
        }
        sleep(main_sleep);
    }
    printf("[*] Total cars came: %d\n", total_cars);
    for (i = 0; i < total_cars; i++) {
        mysem_down(&b.isDone);
        printf("<terminated> car: %d\n", i+1);
    }

    printf("[*] Destroying semaphores and exiting.\n");
    sleep(SLEEP_TIME);
    /*mysem_destroy(&b.blue_sem);
    mysem_destroy(&b.red_sem);
    mysem_destroy(&b.isDone);
    mysem_destroy(&b.bridge_lock);
    mysem_destroy(&b.count_lock_blue);
    mysem_destroy(&b.count_lock_red);
*/
    printf("[*] Done.\n");
    return 0;
}

int is_safe(BRIDGE *arg) {
    sleep(SLEEP_TIME);
    if (!arg->num_of_cars) return 1;
    else if (arg->num_of_cars < (arg->max_cars-1) && arg->curr_direction == arg->type) return 1;
    else return 0;
}

void *car_func(void*args) {
    BRIDGE *arg = (BRIDGE *) args;

    if (arg->type == BLUE) {
        /* Entry code. */
        if (!is_safe(arg)) mysem_down(&arg->blue_sem);
        mysem_down(&arg->count_lock_blue);
        arg->blue_cars_count++;
        arg->num_of_cars++;
        if (arg->blue_cars_count == 1) 
            mysem_down(&arg->bridge_lock);
        mysem_up(&arg->count_lock_blue);

        /* Inform. */
        printf("Car %ld heading west on the bridge.\n", pthread_self() % 10000);
        sleep(CROSSING_TIME);
        printf("Car %ld heading west off the bridge.\n", pthread_self() % 10000);

        /* Exit code. */
        mysem_down(&arg->count_lock_blue);
        arg->blue_cars_count--;
        arg->num_of_cars--;
        if (arg->blue_cars_count == 0) {
            arg->type = RED;
            arg->curr_direction = RED;
            mysem_up(&arg->bridge_lock);
        }
        mysem_up(&arg->count_lock_blue);
        mysem_up(&arg->blue_sem);
        while(!mysem_up(&arg->isDone)); /* Send a termination signal. */
        return (void *) NULL;
    }
    /* Entry code. */
    if (!is_safe(arg)) mysem_down(&arg->red_sem);
    mysem_down(&arg->count_lock_red);
    arg->red_cars_count++;
    arg->num_of_cars++;
    if (arg->red_cars_count == 1) 
        mysem_down(&arg->bridge_lock);
    mysem_up(&arg->count_lock_red);

    /* Inform. */
    printf("Car %ld heading east on the bridge.\n", pthread_self() % 10000);
    sleep(CROSSING_TIME);
    printf("Car %ld heading east off the bridge.\n", pthread_self() % 10000);

    /* Exit code. */
    mysem_down(&arg->count_lock_red);
    arg->red_cars_count--;
    arg->num_of_cars--; 
    if (arg->red_cars_count == 0) {
        arg->type = BLUE;
        arg->curr_direction = BLUE;
        mysem_up(&arg->bridge_lock);
    }
    mysem_up(&arg->count_lock_red);
    mysem_up(&arg->red_sem);
    while(!mysem_up(&arg->isDone)); /* Send a termination signal. */
    return (void *) NULL;
}
