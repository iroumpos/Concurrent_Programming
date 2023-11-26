#include "sem_lib.h"
#include <pthread.h>

#define BLUE 0
#define RED 1
#define CROSSING_TIME 3



typedef struct bridge{
    mysem_t bridge;
    mysem_t r_car,b_car;
    int rw,bw;
   // int r_in,b_in;
    int max_cars;
    int num_of_cars;
    int curr_direction;
    int direction;
}BRIDGE;

int is_safe(BRIDGE *arg) {
    if (!arg->num_of_cars) 
        return 1;
    else if (arg->num_of_cars < arg->max_cars && arg->curr_direction == arg->direction) 
        return 1;
    else return 0;
}

void* red_car(void* args){
    BRIDGE *info = (BRIDGE*)args;
    
    //ENTRY
    mysem_down(&info->bridge);
    if(!is_safe(info)){
        info->rw++;
        mysem_up(&info->bridge);
        mysem_down(&info->r_car);
    }
    info->num_of_cars++;
    info->direction = RED;
    info->curr_direction = info->direction;
    mysem_up(&info->bridge);

    //CS
    printf("Car %ld heading east on the bridge.\n", pthread_self() % 10000);
    sleep(CROSSING_TIME);
    printf("Car %ld heading east off the bridge.\n", pthread_self() % 10000);

    //EXIT
    mysem_down(&info->bridge);
    info->num_of_cars--;
    if(info->num_of_cars > 0){
        mysem_up(&info->r_car);
    }
    else{
        if(info->bw !=0)
            mysem_up(&info->b_car);
        mysem_up(&info->r_car);
    }
    mysem_up(&info->bridge);
    return NULL;
}

void* blue_car(void* args){
BRIDGE *info = (BRIDGE*)args;
    
    //ENTRY
    
    mysem_down(&info->bridge);
    if(!is_safe(info)){
        info->bw++;
        mysem_up(&info->bridge);
        mysem_down(&info->b_car);
    }
    info->num_of_cars++;
    info->direction = BLUE;
    info->curr_direction = info->direction;
    mysem_up(&info->bridge);

    //CS
    printf("Car %ld heading west on the bridge.\n", pthread_self() % 10000);
    sleep(CROSSING_TIME);
    printf("Car %ld heading west off the bridge.\n", pthread_self() % 10000);

    //EXIT
    mysem_down(&info->bridge);
    info->num_of_cars--;
    if(info->num_of_cars > 0){
        mysem_up(&info->b_car);
    }
    else{
        if(info->rw !=0)
            mysem_up(&info->r_car);
        mysem_up(&info->b_car);
    }
    mysem_up(&info->bridge);

    return NULL;
}

int main(int argc,char *argv[]){
    BRIDGE info;
    int type;
    int cars_coming;
    int main_sleep;
    int total_cars;

    info.max_cars = atoi(argv[1]);
    /*info.r_in = 0;
    info.b_in = 0;*/
    info.rw = 0;
    info.bw = 0;
    info.num_of_cars = 0;
    info.curr_direction = BLUE;
    info.direction = BLUE;
    mysem_init(&info.bridge,1);
    mysem_init(&info.b_car,0);
    mysem_init(&info.r_car,0);
    

    printf("[*] Creating threads.\n");
    while (scanf("%d:%d:%d", &type, &cars_coming, &main_sleep) != EOF){
        pthread_t car_threads[cars_coming];
        total_cars += cars_coming;
        if(type == RED){
            for(int i=0; i<cars_coming; i++){
                pthread_create(&car_threads[i],NULL,red_car,(void *)&info);
            }
            sleep(main_sleep);
        }
        if(type == BLUE){
            for(int i=0; i<cars_coming; i++){
                pthread_create(&car_threads[i],NULL,blue_car,(void *)&info);
            }
            sleep(main_sleep);
        }
    }

    /*printf("[*] Total cars came: %d\n", total_cars);
    for (int i = 0; i < total_cars; i++) {
        mysem_down(&info.isDone);
        printf("<terminated> car: %d\n", i+1);
    }*/

    //printf("[*] Done.\n");
    return 0;
}

