#ifndef __FILES_H_
#define __FILES_H_

#include <stdio.h> 

/* Functions. */
extern char *reading_from_file(char *path_to_file);
extern void  creating_copy_file(char *path_to_copy_file, char *file_array_output);

#endif