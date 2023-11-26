#include "uthreads.h"
#include "libroutines.h"
#include <stdio.h> 
#include <string.h>
#include <ucontext.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>

#define DEFAULT_TUPLE_SPACE_SIZE 3

sigset_t sig_set;
int my_turn=0;		// turn = 0  -> tuple_out
					// turn = 1  -> tuple_in
ucontext_t terminate;


void ERROR_MSG(char*msg){
	fprintf(stderr,"%s",msg);
	exit(EXIT_FAILURE);
}

/* ********** timer creation, blocking and unblocking functions ********** */
void create_CPU_clock() {
	struct itimerval timer = {{0}};
	timer.it_value.tv_sec     = 0;
	timer.it_value.tv_usec    = 50000;
	timer.it_interval.tv_sec  = 0;
	timer.it_interval.tv_usec = 50000;
	setitimer(ITIMER_REAL, &timer, NULL);
}

void block_SIGALRM(){  
	sigemptyset(&sig_set);
   	sigaddset(&sig_set, SIGALRM);
   	if(sigprocmask(SIG_BLOCK, &sig_set, NULL) < 0){
    	ERROR_MSG("[!] sigprocmask:");
   	}
}

void unblock_SIGALRM() {
	 if(sigprocmask(SIG_BLOCK, &sig_set, NULL) < 0){
          ERROR_MSG("[!] sigprocmask:");
      }
}


/* ********************* scheduler's functions ******************** */
int scheduler_append(thread_t *thread) {
	thread_t* last;
	if(FIFO_scheduler->head == NULL)
		FIFO_scheduler->head = thread;
	else{
		last = FIFO_scheduler->head;
		while(last->next!=NULL){
			last = last->next;
		}
		last->next = thread;
	}
	return 0;
}

void scheduler() {
	block_SIGALRM();
	
	//Need to grab current running context. 
	FIFO_scheduler->current = FIFO_scheduler->index;	
	
	FIFO_scheduler->index = FIFO_scheduler->index->next;
	while(!FIFO_scheduler->index->status) 
		FIFO_scheduler->index = FIFO_scheduler->index->next;	

	swapcontext(FIFO_scheduler->current->context, FIFO_scheduler->index->context);

	unblock_SIGALRM();
}

/* ********************* mythreads functions ******************** */
int mythreads_init() {
	struct sigaction act = {{0}};

	thread_count = 0;
	
	/* tuple initilization. */
	tuple_size = DEFAULT_TUPLE_SPACE_SIZE;
	tuple_space = (tuple_t *) malloc(sizeof(tuple_t) * tuple_size);
	if (!tuple_space) ERROR_MSG("[!] malloc:");
	for (int i = 0; i < DEFAULT_TUPLE_SPACE_SIZE; i++) {
     	tuple_space[i].integer = 0;
		tuple_space[i].real    = 0.0;
		//strcpy(tuple_space[i].string,"AVAILABLE");     
	}
	for (int i = 0; i < DEFAULT_TUPLE_SPACE_SIZE; i++) {
		mythreads_tuple_out("AVAILABLE");
	}	

	/* Scheduler creation. */
	FIFO_scheduler = (scheduler_t *) malloc(sizeof(scheduler_t));
	if (!FIFO_scheduler) ERROR_MSG("[!] malloc:");
	FIFO_scheduler->head = FIFO_scheduler->tail = FIFO_scheduler->index = NULL;

	/* main thread setup. */
	mainctx = (thread_t *) malloc(sizeof(thread_t));
	if (!mainctx) ERROR_MSG("[!] malloc:");
	mainctx->thread_id = thread_count++;
	mainctx->status    = 1; // ready.
	mainctx->context   = (ucontext_t*)malloc(sizeof(ucontext_t));
	if (!mainctx->context) ERROR_MSG("[!] malloc:");
	scheduler_append(mainctx); // appending main to the scheduler's FIFO list.
	
	getcontext(&terminate);
	if(FIFO_scheduler->size>=2){
		FIFO_scheduler->index->status = -1;
		FIFO_scheduler->size--;
		setcontext(FIFO_scheduler->head->context);
	}
	
	/* Activate Scheduler Timer. */
  	act.sa_handler = scheduler;
  	if(sigaction(SIGALRM, &act, NULL) < 0) ERROR_MSG("[!] sigaction:");

	/* Activate Scheduler. */
	create_CPU_clock();

	return 0;
}

int mythreads_create(thread_t *thread, void (body)(void *), void *arguments) {
	thread = (thread_t*)malloc(sizeof(thread_t));
	thread->thread_id = thread_count++;
	thread->status = 1; // ready.
	//mycoroutines_create(thread->context, (void(*)(void))body, arguments); // context building.
	/**/
	thread->context = (ucontext_t*)malloc(sizeof(ucontext_t));
	if(getcontext((thread->context)) == -1){
        perror("Error in creating context\n");
        return EXIT_FAILURE;
    }
    printf("Creating a coroutine\n");
    thread->context->uc_link = FIFO_scheduler->head->context;
    thread->context->uc_stack.ss_sp = thread->mystack;
    thread->context->uc_stack.ss_size = sizeof(thread->mystack);
	scheduler_append(thread);	

    makecontext((thread->context),(void(*)(void))body,1,arguments);
	
	return 0;
}


int mythreads_yield() {
  	if(kill(getpid(), SIGALRM) < 0)
    	return -1;
	return 0;
}


int mythreads_join(thread_t *thread){
  	while(thread->status != -1); // run while status != <terminated>.
  	return 0;
}

int mythreads_destroy(thread_t *thread) {
	thread_t *pr, *cur;
	for(pr = FIFO_scheduler->head, cur = FIFO_scheduler->head->next; cur != FIFO_scheduler->head; pr = cur, cur = cur->next) {
		cur = cur->next;
    	pr->next = cur;	
	}
	unblock_SIGALRM();
	return 0;
}



/* ********************* tuple space's functions ******************** */
int mythreads_tuple_out(char *fmt,...) {
	int c = 0;
	int i;
	
	my_turn = 0;
	va_list ap;	
	va_start(ap, fmt);	

	/* Checking whether tuple space is full or not. */
	if (strcmp(tuple_space[tuple_size-1].string, "AVAILABLE")) {
		tuple_space = (tuple_t *) realloc(tuple_space, sizeof(tuple_t) * (++tuple_size));
		if (!tuple_space) ERROR_MSG("[!] realloc:");
		tuple_space[tuple_size-1].integer = 0;
		tuple_space[tuple_size-1].real    = 0.0;
		strcpy(tuple_space[tuple_size-1].string,"AVAILABLE");

		/* Now we know the last index is available. */
	}

	/* We need to find out the first available index. */
	for (i = 0; i < tuple_size; i++) {
		if (!strcmp(tuple_space[i].string, "AVAILABLE")) break; // i = available index.
	}	

	while (fmt && fmt[c]) {
		if (fmt[c] == '%') {
			c++;
			switch(fmt[c]) {
				case 'd':
					tuple_space[i].integer = (int) va_arg(ap, int);
					break;
				case 'l':
					tuple_space[i].real = (double) va_arg(ap, double);
					break;
				case 's':
					strcpy(tuple_space[i].string,(char *) va_arg(ap, char *));
					break;
			}
		}
		else {
			c++;
		}
	}
	va_end(ap);
	return 0;
}


int mythreads_tuple_in(char *fmt,...) {
	va_list ap;
	va_start(ap, fmt);
	
	int i=0;
	int c=0;
	int t_integer;
	double t_real;
	char* temp;

	while (fmt && fmt[c]) {
		if (fmt[c] == '%') {
			c++;
			switch(fmt[c]) {
				case 'd':
					t_integer = (int) va_arg(ap, int);
					break;
				case 'l':
					t_real = (double) va_arg(ap, double);
					break;
				case 's':
					temp = (char *) va_arg(ap, char *);
					break;
			}
		}
		else {
			c++;
		}
	}

	while(my_turn==1){
		for(i=0; i<tuple_size; i++){
			if(t_integer == tuple_space[i].integer && t_real == tuple_space->real && (strcmp(temp,tuple_space->string) == 0)){
				printf("%s---%d---%lf",tuple_space[i].string,tuple_space[i].integer,tuple_space[i].real);
				tuple_space[i].integer = 0;
				tuple_space[i].real = 0.0;
				strcpy(tuple_space[i].string,"AVAILABLE");
				my_turn = 0;
				break;
			}
		}
	}
	return 0;
}
