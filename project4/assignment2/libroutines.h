#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ucontext.h>

typedef struct coroutines{
    ucontext_t context;
	ucontext_t *old_context;
    char stack[8192];
}co_t;

/*Prototypes*/
int mycoroutines_init(co_t* main);
int mycoroutines_create(co_t* co,void(body)(void*),void* arg);
int mycoroutines_switchto(co_t* co);
int mycoroutines_destroy(co_t* co);


/*Creating functions*/
int mycoroutines_init(co_t* main){
    if(getcontext(&main->context) == -1){
        perror("Error in getcontext in create");
        return EXIT_FAILURE;
    }
    return 0;
}

int mycoroutines_create(co_t* co,void(body)(void*),void* args){
    
    if(getcontext(&(co->context)) == -1){
        perror("Error in creating context\n");
        return EXIT_FAILURE;
    }
    printf("Creating a coroutine\n");
    co->context.uc_link = 0;
    co->context.uc_stack.ss_sp = co->stack;
    co->context.uc_stack.ss_size = sizeof(co->stack);	

    makecontext(&(co->context),(void(*)(void))body,1,args);
    return 0;
}

int mycoroutines_switchto(co_t* co){
    setcontext(&co->context);
	//swapcontext(co->old_context, &co->context);
    return 0;
}

int mycoroutines_destroy(co_t* co){
    if(getcontext(&(co->context)) == -1){
        perror("Error in destroy\n");
        return EXIT_FAILURE;
    }

    co->context.uc_link = 0;
    co->context.uc_stack.ss_size = 0;
    //free(co->context.uc_stack.ss_sp);
	co->context.uc_stack.ss_sp = NULL;

    return 0;
}