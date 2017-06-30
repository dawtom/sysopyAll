#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define BUFFERSIZE 1024

int threads_number;
char* file_name;
int number_of_records;
char* search_word;
int file_handler;
pthread_t *threads_list;
int thread_read = 1;

void* start_routine(void *);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    file_name = calloc(20, sizeof(char));
    search_word = calloc(10, sizeof(char));

    if(argc != 5) {
        printf("Wrong arguments number.\n Use with arguments: threads_number file_name number_of_records search_word\n");\
        exit(1);
    }

    threads_number = atoi(argv[1]);
    file_name = argv[2];
    number_of_records = atoi(argv[3]);
    search_word = argv[4];


    if((file_handler = open(file_name, O_RDONLY)) == -1) {
        printf("Error while opening file to search");
        exit(1);
    }

    threads_list = calloc(threads_number, sizeof(pthread_t));

    for(int i = 0; i < threads_number; i++){
        if(pthread_create(&threads_list[i], NULL, start_routine, NULL) != 0) {
            printf("Error while creating threads");
            exit(1);
        }
    }

    thread_read = 0;

    for(int i = 0; i < threads_number; i++) {
        pthread_join(threads_list[i], NULL);
    }

    free(threads_list);
    close(file_handler);

    return 0;
}


void* start_routine(void *arg) {
    // can be cancelled at any time
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    char** records_buffer = calloc(number_of_records, sizeof(char*));
    char* record_id = calloc(2, sizeof(char));

    int characters_read_no;
    int shutdown = 0;

    for(int i = 0; i < number_of_records; i++) {
        records_buffer[i] = calloc(1024, sizeof(char));
    }

    // wait for threads to start
    while(thread_read);

    while(!shutdown) {
        // read lock
        pthread_mutex_lock(&mutex);

        for(int i = 0; i < number_of_records; i++){
            if((characters_read_no = read(file_handler, records_buffer[i], BUFFERSIZE) ) == -1) {
              printf("Error while reading from file");
              exit(-1);
            }
        }

        pthread_mutex_unlock(&mutex);

        for(int i = 0; i < number_of_records; i++) {
            if(strstr(records_buffer[i], search_word) != NULL) {
                // occurence found
                strncpy(record_id, records_buffer[i], 2);
                printf("TID: %ld, record id: %d\n", pthread_self(), atoi(record_id));

                for(int j = 0; j < threads_number; j++) {
                    // cancel all but self
                    if(threads_list[j] != pthread_self()) {
                        pthread_cancel(threads_list[j]);
                    }
                }
                shutdown = 1;
                // break
                i = number_of_records;
            }
        }
    }

    return NULL;
}
