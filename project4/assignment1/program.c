#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ucontext.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "libroutines.h"

char* buffer;           // buffer to write data
volatile int size;      // size of buffer
volatile int steps=0;
volatile int w_pos=0;       // position of writing
volatile int r_pos=0;       // position of reading
volatile int close_f=0;   // close file
volatile int write_f=0;   // write flag
volatile int read_f=0;    // read flag
co_t co_main, co_consumer, co_producer;

void *producer();
void *consumer();
void p_read(char* c);
void p_write(char c);

int main(int argc,char* argv[]){ 
    if(argc != 2){
        printf("Give correct number of arguments <size of pipe>\n");
        return (EXIT_FAILURE);
    }
    size = atoi(argv[1]);
    // initialize pipe
    buffer = (char*)malloc(sizeof(char)*size);
    
    //Initialize main coroutine
    mycoroutines_init(&co_main);     

    /*Create contexts for 2 coroutines*/
    mycoroutines_create(&co_producer,(void(*))&producer,NULL);
    mycoroutines_create(&co_consumer,(void(*))&consumer,NULL);
    
    printf("Switch to the other coroutine\n");
    mycoroutines_switchto(&co_producer);

    printf("Destroy the coroutines\n");
    mycoroutines_destroy(&co_producer);
    mycoroutines_destroy(&co_consumer);
    free(buffer);
    
    return 0;
}

void* producer(){
    FILE *fd;
    int r_value;
	
	fd = fopen("sample.pdf", "rb");
    if(!fd){
 		fprintf(stderr,"Error at opening file in producer");
        exit(EXIT_FAILURE);
    }
	long int offset = size*steps;
	fseek(fd, offset, SEEK_SET);
    while(1){

       	char byte;
       	r_value = fread(&byte,sizeof(char),1,fd);

       	if(r_value < 0){
           	fprintf(stderr,"Error in reading in file");
           	exit(EXIT_FAILURE);
       	}
       	// End of file
       	if(r_value == 0){
           	close_f = 1;
		   	fclose(fd);
			//co_producer.old_context = &co_producer.context;
           	mycoroutines_switchto(&co_consumer);
           	write_f = 1;
           	break;
       	}
       	p_write(byte);
       	w_pos++;
       	if(w_pos == size){       // buffer full
           	w_pos = 0;
			steps++;
			//co_producer.old_context = &co_producer.context;
           	mycoroutines_switchto(&co_consumer);
       	} 
    }
	//co_producer.old_context = &co_producer.context;
    mycoroutines_switchto(&co_main);
    return NULL;
}


void* consumer(){
	FILE *fd;
    int w_value;
	
	fd = fopen("sample2.pdf", "ab");
   	if(!fd){
   		fprintf(stderr,"File doesn't exist.");
		exit(EXIT_FAILURE);
    }

    while(1){
        if(read_f == 1){
			fclose(fd);
			//co_consumer.old_context = &co_consumer.context;
            mycoroutines_switchto(&co_main);
            break;
        }
        if(close_f == 1){       // producer stop writing
            while(1){           // for all remaining spots on buffer
                if(w_pos == r_pos){
					//co_consumer.old_context = &co_consumer.context;
                    mycoroutines_switchto(&co_main);
                    read_f = 1;
                    break;
                }
                
                char character = buffer[r_pos];
                //w_value = write(fd,&character,sizeof(char));
                //w_value = fwrite(&character, sizeof(char), 1, fd);
				w_value = fputc(character, fd);
				if(w_value == EOF){
                    fprintf(stderr,"Error at writing in buffer");
                    exit(EXIT_FAILURE);
                }
                r_pos++;
            }
        }
        else{
                char character;         // read_character
                p_read(&character);
                //w_value = write(fd,&character,sizeof(char));
				//w_value = fwrite(&character, sizeof(char), 1, fd);
                w_value = fputc(character, fd);
				if(w_value == EOF){
                    fprintf(stderr,"Error at writing in buffer");
                    exit(EXIT_FAILURE);
                }
                r_pos++;
                if(r_pos == size){
                    r_pos = 0;
					//co_consumer.old_context = &co_consumer.context;
                    fclose(fd);
					mycoroutines_switchto(&co_producer);
                }
        }
    }
	fclose(fd);
    return NULL;
}

void p_write(char c){
    buffer[w_pos] = c;
}

void p_read(char* c){
    c[0] = buffer[r_pos];
}