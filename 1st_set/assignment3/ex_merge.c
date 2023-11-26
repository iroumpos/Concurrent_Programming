#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_MIN_LIMIT  64
#define NUM_OF_THREADS 2

/* Structs. */
typedef struct file_info {
    FILE *file_assigned;
    char *filename;
    int   file_assigned_size;
    int   isFinished;
} FILE_INFO;

/* Functions' Prototypes. */
void  merge(int arr[], int l, int m, int r);
void  mergeSort(int arr[], int l, int r);
void  printArray(int A[], int size);
FILE *mergeFiles(FILE* file1, FILE* file2, int file1_numbers, int file2_numbers, FILE *output_file);
void *thread_func(void *args);

int main(int argc, char **argv) {
    printf("[*] Starting program.\n");
    sleep(2);
    if (argc != 2) {
        fprintf(stderr, "[!] ERROR: usage: ./exercise2 <filename>\n");
        exit(EXIT_FAILURE);
    }
    /* Declarations. */
    FILE *input_file;
    int   file_length, num_of_nums, *number_array;
    int   i = 0;
    
    /* Check how many numbers exist in the 'filename'. */
    printf("[*] Opening '%s'.\n", argv[1]);
    sleep(2);
    input_file = fopen(argv[1], "rb");
    fseek(input_file, 0L, SEEK_END);
    file_length = ftell(input_file);
    num_of_nums = file_length / sizeof(int);
    fseek(input_file, 0L, SEEK_SET);

    if (num_of_nums <= NUM_MIN_LIMIT) { /* Simple Merge Sort in RAM. */
        number_array = (int *) malloc(sizeof(int) * num_of_nums);
        memset(number_array, 0, sizeof(int) * num_of_nums);
        while ((number_array[i++] = getw(input_file)) != EOF);
        printf("[*] Sorting.\n\n");
        sleep(2);
        mergeSort(number_array, 0, num_of_nums - 1);
        printArray(number_array, num_of_nums);
        free(number_array);
        printf("\n[*] Done.\n");
        sleep(2);
        return 0;
    }
    FILE *output_file = fopen("output_file", "wb");
    pthread_t *threads;
    threads = (pthread_t *) malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    FILE_INFO file_information[NUM_OF_THREADS];

    /* Main breaks the 'numbers_file' in 2 parts (2 different files). */
    FILE *input_file_middle = input_file;
    fseek(input_file, 0L, SEEK_SET);
    fseek(input_file_middle, file_length/2, SEEK_SET);
    file_information[0].file_assigned = input_file;
    file_information[0].file_assigned_size = file_length/2;
    file_information[0].filename = argv[1];
    file_information[0].isFinished = 0;
    file_information[1].file_assigned = input_file_middle;
    file_information[1].file_assigned_size = file_length/2;
    file_information[1].filename = argv[1];
    file_information[1].isFinished = 0; 
    pthread_create(&threads[0], NULL, thread_func, (void *) &file_information[0]);
    pthread_create(&threads[1], NULL, thread_func, (void *) &file_information[1]);

    /* Waiting for threads to finish. */
    while (file_information[0].isFinished == 0 && file_information[1].isFinished == 0) { sleep(0.1); }
    
    /* Getting ready to merge the files. */
    output_file = mergeFiles(file_information[0].file_assigned, file_information[1].file_assigned, 
    file_information[0].file_assigned_size/sizeof(int), file_information[1].file_assigned_size/sizeof(int), output_file);
    fclose(output_file);

    return 0;
}

/* Functions' implementations. */
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
  
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
  
    i = 0; 
    j = 0; 
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int l, int r)
{
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
  
        merge(arr, l, m, r);
    }
}

void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

FILE *mergeFiles(FILE* file1, FILE* file2, int file1_numbers, int file2_numbers, FILE *output_file) {
    int i = 0, j = 0, num1, num2;

    fseek(file1, 0L, SEEK_SET);
    fseek(file2, 0L, SEEK_SET);
    while(i != file1_numbers && j != file2_numbers) {
        if (j == file2_numbers) {
            for (int counter = i; counter < file1_numbers; i++) {
                num1 = getw(file1);
                putw(num1, output_file);
            }
            break;
        }
        if (i == file1_numbers) {
            for (int counter = j; counter < file2_numbers; j++) {
                num2 = getw(file2);
                putw(num2, output_file);
            }
            break;
        }
        num1 = getw(file1);
        num2 = getw(file2);
        if (num1 <= num2) {
            putw(num1, output_file);
            fseek(file2, -sizeof(int), SEEK_CUR);
            i++;
            continue;
        }
        putw(num2, output_file);
        fseek(file1, -sizeof(int), SEEK_CUR);
        j++;
        continue;
    }
    return output_file;
}

void *thread_func(void *args) {
    FILE_INFO *file_info = (FILE_INFO *) args;
    FILE *file_assigned = fopen(file_info->filename, "wb");
    file_assigned = file_info->file_assigned;   /* take the value of the pointer which points to a specific place inside the file. */
    fseek(file_assigned, file_info->file_assigned_size, SEEK_SET);
    int file_length = ftell(file_assigned);
    int numbers = file_length / sizeof(int);
    fseek(file_assigned, 0L, SEEK_SET);
    
    /* check num_of_nums. */ 
    if (numbers <= NUM_MIN_LIMIT) {
        int i = 0;
        int *numbers_array = (int *) malloc(sizeof(int) * numbers);
        memset(numbers_array, 0, sizeof(int) * numbers);
        for (i = 0; i < numbers; i++) {
            numbers_array[i] = getw(file_assigned);
        }
        mergeSort(numbers_array, 0, numbers - 1);
        fseek(file_info->file_assigned, 0L, SEEK_SET);
        for (i = 0; i < numbers; i++) putw(numbers_array[i], file_info->file_assigned);
        file_info->isFinished = 1; /* working threads has finished. */
        return (void *) NULL;
    }
    
    /* Working thread breaks the file_assigned into 2 parts (2 different files). */
    pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    FILE_INFO file_information[NUM_OF_THREADS];
    FILE *file_assigned_middle = file_assigned;
    fseek(file_assigned, 0L, SEEK_SET);
    fseek(file_assigned_middle, file_length/2, SEEK_SET);
    file_information[0].file_assigned = file_assigned;
    file_information[0].file_assigned_size = file_length / 2;
    file_information[0].filename = file_info->filename;
    file_information[1].file_assigned = file_assigned_middle;
    file_information[1].file_assigned_size = file_length / 2;
    file_information[1].filename = file_info->filename;
    pthread_create(&threads[0], NULL, thread_func, (void *) &file_information[0]);
    pthread_create(&threads[1], NULL, thread_func, (void *) &file_information[1]);

    /* Waiting for threads to finish. */
    while (file_information[0].isFinished == 0 && file_information[1].isFinished == 0) { sleep(0.1); }
    
    /* Getting ready to merge the files. */
    mergeFiles(file_assigned, file_assigned_middle, file_length/2/sizeof(int), file_length/2/sizeof(int), file_information[0].file_assigned);
    return (void *) NULL;
}