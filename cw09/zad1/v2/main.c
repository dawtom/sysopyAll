#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_NUMBER 20


int* numbers_array;
int array_size;

pthread_t * readers;
pthread_t * writers;

int readers_number;
int writers_number;

int reader_reads_number;
int writer_writes_number;

int curr_readers = 0;

bool extended_communicate = false;
bool wait = true;
__time_t get_time() {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp.tv_nsec/1000;
}

sem_t reader_sem;
sem_t writer_sem;

bool check_arguments(int argc, char **argv);
bool is_number(char *arg);
void* reader_thread(void *arg);
void* writer_thread(void *arg);

void array_init(int size);

void threads_run();

void clean_memory();

int main(int argc, char ** argv) {
    if (!check_arguments(argc, argv)){
        printf("Wrong usage. Should be: \n"
                       "1) readers number \n"
                       "2) writers number \n"
                       "3) how much reader reads \n"
                       "4) how much writer writes \n"
                       "5) array size \n"
                       "6) communicate if extended (-i option) \n");
        exit(1);
    }
    readers_number = atoi(argv[1]);
    writers_number = atoi(argv[2]);
    reader_reads_number = atoi(argv[3]);
    writer_writes_number = atoi(argv[4]);
    array_init(atoi(argv[5]));

    if (argv[6] != NULL){
        extended_communicate = true;
    }

    // initialize semaphore shared between threads with value 1
    if (sem_init(&reader_sem, 0, 1) == -1){
        printf("Error while creating reader_sem");
        exit(1);
    }

    // initialize semaphore shared between threads with value 1
    if (sem_init(&writer_sem, 0, 1) == -1){
        printf("Error while creating writer_sem");
        exit(1);
    }
    threads_run();
    clean_memory();
    return 0;
}

void clean_memory() {
    free(numbers_array);
    free(readers);
    free(writers);

    sem_destroy(&reader_sem);
    sem_destroy(&writer_sem);
}

void threads_run() {
    readers = malloc(sizeof(pthread_t) * readers_number);
    writers = malloc(sizeof(pthread_t) * writers_number);

    int* dividers = malloc(sizeof(int) * readers_number);

    for (int i = 0; i < readers_number; i++){
        dividers[i] = rand() % (MAX_NUMBER - 1)  + 1;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);


    for(int i = 0; i < readers_number; i++){
        if (pthread_create(&readers[i], NULL, reader_thread, dividers + i) != 0){
            printf("Error while creating reader thread.\n");
            exit(1);
        }
    }
    wait = false;
    for(int i = 0; i < writers_number; i++){
        if (pthread_create(&writers[i], NULL, writer_thread, NULL) != 0){
            printf("Error while creating writer thread.\n");
            exit(1);
        }
    }

    for (int i = 0 ; i < readers_number; i++){
        if (pthread_join(readers[i], NULL) != 0){
            printf("Error while joining threads.\n");
            exit(1);
        }
    }
}
void* reader_thread(void *arg){
    while(wait);
    int factor = *(int*) arg;
    for (int i = 0; i < reader_reads_number; i++){
        sem_wait(&reader_sem);

        // conditional variable equivalent
        if(++curr_readers == 1){
            sem_wait(&writer_sem);
        }
        sem_post(&reader_sem);

        int counter = 0;
        for (int k = 0; k < array_size; k++){
            if (numbers_array[k] % factor == 0){
                counter++;
                if (extended_communicate){
                    printf("Time: %d ** READER ** TID: %lu: index: %d, value: %d, factor: %d\n",
                           get_time(), pthread_self(), k, numbers_array[k], factor);
                }
            }
        }
        printf("Time: %d ** READER ** TID: %lu: %d numbers\n", get_time(), pthread_self(), counter);

        sem_wait(&reader_sem);
        if (--curr_readers == 0){
            sem_post(&writer_sem);
        }
        sem_post(&reader_sem);
        sleep(1);

    }
}
void* writer_thread(void *arg){
    while(wait);
    int written_numbers_number, index, value;
    for (int i = 0; i < writer_writes_number; i++){
        sem_wait(&writer_sem);

        written_numbers_number = rand() % array_size + 1;

        printf("Time: %d ** WRITER ** TID:  %lu wrote something.\n", get_time(), pthread_self());
        for (int j = 0; j < written_numbers_number; j++){
            index = rand() % array_size;
            value = rand() % MAX_NUMBER;

            numbers_array[index] = value;
            if (extended_communicate){
                printf("Time: %d ** WRITER ** TID: %lu changed number with index %d "
                               "to %d\n", get_time(), pthread_self(), index, value);
            }
        }

        sem_post(&writer_sem);
        sleep(1);
    }
}
void array_init(int size) {
    array_size = size;
    numbers_array = malloc(array_size * sizeof(int));

    for (int i = 0; i < array_size; i++){
        numbers_array[i] = rand() % MAX_NUMBER;
    }
    for (int i = 0; i < array_size; i++){
        printf("%d ", numbers_array[i]);
    }
    printf("\n");
}

bool check_arguments(int argc, char **argv) {
    if ((argc != 6) && (argc != 7)) {
        return false;
    }
    for (int i = 1; i < 6; i++){
        if (!is_number(argv[i])){
            return false;
        }
    }
    if (argv[6] != NULL && strcmp(argv[6], "-i") != 0) {
        return false;
    }

    return true;

}

bool is_number(char *arg) {
    int i = 0;
    while(arg[i] != '\0'){
        if (arg[i] >= '0' && arg[i] <= '9') {
            i++;
        }
        else {
            return false;
        }
    }
    return true;
}