#include "sem_lib.h"


struct sembuf asem;

int mysem_init(mysem_t *s,int n){
        
    if(n<0 || n>1)
        return 0;

    s->id = semget(IPC_PRIVATE,1,S_IRWXU);
    if(s->id == -1){
        perror("error in semget init");
        exit(EXIT_FAILURE);
    }

    if(semctl(s->id,0,SETVAL,n) == -1){
        perror("error in semctl init");
        exit(EXIT_FAILURE);
    }
    s->if_init = true;
    if(s->if_init == false)
        return -1;

    return 1;   
}


int mysem_down(mysem_t* s){

    /*if not initialized*/
    if(s->if_init == false)
        return -1;

    
    /*decreases value*/
    asem.sem_num = 0;
    asem.sem_op = -1;
    asem.sem_flg = 0;
    
    if(semop(s->id,&asem,1) == -1){
        perror("error in semop down");
        exit(1);
    }    
    return 1;
}


int mysem_up(mysem_t* s){
    int ret_val;

    ret_val = semctl(s->id,0,GETVAL);
    if(ret_val == 1)
        return 0;
    
    /*if not initialized*/
    if(s->if_init == false)
        return -1;


    // increases value
    asem.sem_op = 1;
    asem.sem_num = 0;
    asem.sem_flg = 0;

    if(semop(s->id,&asem,1) == -1){
        perror("error in semop up");
        exit(1);
    }

    return 1;

}

int mysem_destory(mysem_t* s){
    /*check if not initialized*/
    if(s->if_init == false)
        return -1;


    if(semctl(s->id,0,IPC_RMID) == -1){
        perror("error in semctl destroy");
        exit(1);
    }
    return 1;
}