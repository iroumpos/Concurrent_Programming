#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>



#define red 0
#define blue 1


/*
Add function to delete previous data from the file 
in order to write the new data.
*/

int main(int argc,char *argv[]){
    
    int num_of_vehicles;
    int type;
    int delay;
    int n_of_lines;
    FILE* stream;


    if(argc!=2){
        printf("Error in arguments.Give <number_of_lines>.");
        return -1;
    }

    n_of_lines = atoi(argv[1]);
    stream = fopen("/mnt/c/Users/giann/Documents/HMMY/tauto/2nd_set/assignment3/test_file","r+");
    if(stream == NULL){
        perror("error opening the file stream.");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<n_of_lines; i++){
        num_of_vehicles = rand()%10;
        if(num_of_vehicles == 0)
            num_of_vehicles = 1;
        type = rand()%2;
        delay = rand()%7;
        if(delay == 0)
            delay = 1;
        if(type == 0)
            fprintf(stream,"%d vehicles of red color with %d sec delay.\n",num_of_vehicles,delay);
        if(type == 1)
            fprintf(stream,"%d vehicles of blue color with %d sec delay.\n",num_of_vehicles,delay);
    }

    fflush(stream);
    fclose(stream);
    return 0;
}