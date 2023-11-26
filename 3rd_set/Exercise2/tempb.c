#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define RED              0
#define BLUE             1
#define TIME_OF_THE_RIDE 2

typedef struct bridge { 
    pthread_mutex_t monitor;
    pthread_cond_t  blue;
    pthread_cond_t  red;
    pthread_cond_t  isDone;
    int vehicles;
	int capacity;
    int currDirection;
    int bluew, redw;        // blue and red cars waiting in queue.
    int blue_in, red_in;    // blue and red on bridge. (CS)
    int type;               // blue or red.
} bridge;


int is_safe(void* args,int type){
    bridge* arguments = (void*)args;
   

    if(arguments->red_in == 0 || arguments->blue_in == 0)
        return 1;
    if(type == BLUE){
        if(arguments->blue_in < arguments->capacity && arguments->currDirection == type)
            return 1;
    }
    if(type == RED){
        if(arguments->red_in < arguments->capacity && arguments->currDirection == type)
            return 1;
    }
    else
        return 0;
    return 0;
}

void entry_red(void* args) {
    bridge *arguments = (void *) args;

    pthread_mutex_lock(&arguments->monitor);
    while (1) {
		if(!is_safe(args,RED)){    
       		arguments->redw++;
       		pthread_cond_wait(&arguments->red, &arguments->monitor);
    	}
    	if((arguments->red_in < arguments->capacity) && (arguments->blue_in == 0)) {
    		arguments->red_in++;
            arguments->currDirection = RED;
			break;
    	}
		else {
			pthread_cond_wait(&arguments->red, &arguments->monitor);
		}
	}
    pthread_mutex_unlock(&arguments->monitor);
	return;
}

void exit_red(void* args) {
    bridge *arguments = (void *) args;

    pthread_mutex_lock(&arguments->monitor);
    arguments->red_in--;
	arguments->vehicles--;
	    
    if (arguments->red_in == 0) {
		if (arguments->bluew > 0) {
			arguments->bluew--;
			pthread_cond_signal(&arguments->blue);
		}
		if (arguments->redw > 0) {
       		arguments->redw--;
        	pthread_cond_signal(&arguments->red);
    	}
	}
    pthread_cond_signal(&arguments->red);
    /* Last one to inform main. */
    if(arguments->blue_in == 0 && arguments->bluew == 0 && arguments->red_in == 0 && arguments->redw == 0){
        pthread_cond_signal(&arguments->isDone);
        pthread_mutex_unlock(&arguments->monitor);
        return;    
    }
	pthread_cond_signal(&arguments->isDone);
    pthread_mutex_unlock(&arguments->monitor);
	return;
}

void entry_blue(void*args) {
    bridge *arguments = (void*)args;
    
    pthread_mutex_lock(&arguments->monitor);
	while (1) {
    	if (!is_safe(args,BLUE)) {
        	arguments->bluew++;
        	pthread_cond_wait(&arguments->blue, &arguments->monitor);
    	}
    	if((arguments->blue_in < arguments->capacity) && (arguments->red_in == 0)) {
        	arguments->blue_in++;
            arguments->currDirection = BLUE;
			break;
    	}
		else {
        	pthread_cond_wait(&arguments->blue, &arguments->monitor);
		}
	}
    pthread_mutex_unlock(&arguments->monitor);
	return;
}


void exit_blue(void* args){
    bridge *arguments = (void *) args;

    pthread_mutex_lock(&arguments->monitor);
    arguments->blue_in--;
	arguments->vehicles--;

	if (arguments->blue_in == 0) {
    	if (arguments->redw > 0) {
        	arguments->redw--;
            pthread_cond_signal(&arguments->red);
        }
        if (arguments->bluew > 0) {
            arguments->bluew--;
            pthread_cond_signal(&arguments->blue);
       	}
    }
	pthread_cond_signal(&arguments->blue);
    
    /* Last one to inform main. */
    if(arguments->blue_in == 0 && arguments->bluew == 0 && arguments->red_in == 0 && arguments->redw == 0){
        pthread_cond_signal(&arguments->isDone);
        pthread_mutex_unlock(&arguments->monitor);
        return;    
    }
	pthread_cond_signal(&arguments->isDone); 
    pthread_mutex_unlock(&arguments->monitor);
	return;
}


void *vehicle_fun(void* args){
    bridge *arguments = (void *) args;

    /* Blue code. */
    if(arguments->type == BLUE){
        entry_blue(arguments);
        printf("Blue Car <%ld> heading west.\n",pthread_self());
        sleep(TIME_OF_THE_RIDE);
        printf("Blue Car <%ld> heading out of bridge.\n",pthread_self());
        exit_blue(arguments);
        return NULL;
    }
    
    /* Red code. */
    entry_red(arguments);
    printf("Red Car <%ld> heading east.\n",pthread_self());
    sleep(TIME_OF_THE_RIDE);
    printf("Red Car <%ld> heading out of bridge.\n",pthread_self());
    exit_red(arguments);
    return NULL;
}


int main(int argc,char* argv[]){
    bridge info;  
    int i;
    int delay, num_of_vehicles;

    printf("[*] Starting program.\n");
    sleep(2);

    if(argc != 2){
        fprintf(stderr,"[!] ERROR: usage: ./shared_wc <capacity>\n");
        exit(EXIT_FAILURE);
    }
    printf("[*] Initializing structures and variables.\n");
	sleep(2);
 
	info.vehicles = 0; 
    info.capacity = atoi(argv[1]);
    pthread_mutex_init(&info.monitor, NULL);
    pthread_cond_init(&info.blue, NULL);
    pthread_cond_init(&info.red, NULL);
    pthread_cond_init(&info.isDone, NULL);
    info.red_in = 0;
    info.blue_in = 0;
    info.bluew = 0;
    info.redw = 0;
    
    
    printf("[*] Reading from stdin and creating threads.\n");
    sleep(2);	
    while (scanf("%d:%d:%d", &num_of_vehicles, &info.type, &delay) != EOF) {
        pthread_t vehicle_threads[num_of_vehicles];
        info.vehicles += num_of_vehicles;
		for (i = 0; i < num_of_vehicles; i++) {
            pthread_create(&vehicle_threads[i], NULL, vehicle_fun, (void *) &info);
        }
        sleep(delay);
    }
   	
	/* Main waiting to get signaled for termination. */ 
	while (info.vehicles > 0)
		pthread_cond_wait(&info.isDone, &info.monitor); 
    
	printf("[*] Destroying condition variables and monitor and exiting.\n");
    sleep(2);
    pthread_mutex_destroy(&info.monitor);
    pthread_cond_destroy(&info.blue);
    pthread_cond_destroy(&info.red);
    pthread_cond_destroy(&info.isDone);
    
	printf("[*] Done.\n");
	sleep(2);
    return 0;
}
