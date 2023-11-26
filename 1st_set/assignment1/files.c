#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *reading_from_file(char *path_to_file) {
    FILE *file;
    char *file_array;
    file = fopen(path_to_file, "rb"); /* file: Opening. */
    if (!file) {
        fprintf(stderr, "[!] Error: fopen() failed.\n");
        exit(-1);
    }
    fseek(file, 0L, SEEK_END);
    long int file_length = ftell(file); /* file: length calculation. */
    if (fseek(file, 0L, SEEK_SET) < 0) {
        fprintf(stderr, "[!] Error: fseek() failed.\n");
        exit(-1);
    }
    file_array = (char *) calloc(file_length, sizeof(char)); /* file_array: creation and initialization. */
    if (!file_array) {
        fprintf(stderr, "[!] Error: calloc() failed.\n");
        exit(-1);
    }
    int bytes_read = fread(file_array, sizeof(char), file_length, file);
    if (ferror(file)) {
        fprintf(stderr, "[!] Error: fread() failed.\n");
        exit(-1);
    }
    if (fflush(file)) {
        fprintf(stderr, "[!] Error: fflush() failed.\n");
        exit(-1);
    }
    if (fclose(file)) {
        fprintf(stderr, "[!] Error: fclose() failed.\n");
        exit(-1);
    }
    
    return file_array;
}

void creating_copy_file(char *path_to_copy_file, char *file_array_output) {
    printf("[*] Creating copy file.\n");
    sleep(2);
    FILE *file_copy;
    file_copy = fopen(path_to_copy_file, "wb"); /* file_copy creation. */
    if (!file_copy) {
        fprintf(stderr, "[!] Error: fopen() failed.\n");
        exit(-1);
    }
    for (int i = 0; i < strlen(file_array_output); i++) {
        fputc(file_array_output[i], file_copy);
        if (ferror(file_copy)) {
            fprintf(stderr, "[!] Error: fputc() failed.\n");
            exit(-1);
        }
        if (fflush(file_copy)) {
            fprintf(stderr, "[!] Error: fflush() failed.\n");
        exit(-1);
        }
    }
    if (fclose(file_copy)) {
        fprintf(stderr, "[!] Error: fclose() failed.\n");
        exit(-1);
    }
    return;
}