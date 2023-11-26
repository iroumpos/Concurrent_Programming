#ifndef _UTHREADS_H_
#define _UTHREADS_H_

#include <ucontext.h>

/* *************** Structs ************** */
typedef struct thread {
	int         thread_id;
	int         status;   // -1: terminated, 0: blocked, 1: ready, 2: running         
	ucontext_t 	*context;
	char 		mystack[8192];
	struct thread* next;
} thread_t;

typedef struct scheduler {
	thread_t *head;
	thread_t *tail;
	thread_t *current;
	thread_t *index;
	int    size;
} scheduler_t;

typedef struct tuple {
	int    integer;
	double real;
	char   string[64];
} tuple_t;



/* *************** Variables ************** */
scheduler_t *FIFO_scheduler;
int          thread_count;
ucontext_t   scheduler_context;
tuple_t     *tuple_space; 
int          tuple_size;
thread_t *mainctx;


/* *************** Functions ************** */
extern int mythreads_init();
extern int mythreads_create(thread_t *thread, void (body)(void *), void *arguments);
extern int mythreads_yield();
extern int mythreads_join(thread_t *thread);
extern int mythreads_destroy(thread_t *thread);
extern int mythreads_tuple_out(char *fmt,...);
extern int mythreads_tuple_in(char *fmt,...);
extern void scheduler();
extern void create_CPU_clock();
extern void block_SIGALRM();
extern void unblock_SIGALRM();

#endif 
