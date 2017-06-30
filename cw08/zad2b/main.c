#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define BUFFERSIZE 1024

int threads_number;
char* file_name;
int number_of_records;
char* search_word;
int file_handler;
pthread_t *threads_list;
int thread_read = 1;
int option_number, signo;

void sig_handler(int signo){
    if(signo == SIGUSR1) {
        printf("Received SIGUSR1\n");
    } else if(signo == SIGTERM) {
        printf("Received SIGTERM\n");
    }
    printf("PID: %d\n TID: %ld\n", getpid(), pthread_self());
    return;
}

void* start_routine(void *);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    file_name = calloc(20, sizeof(char));
    search_word = calloc(10, sizeof(char));

    if(argc != 3) {
        printf("Wrong arguments number.\n Use with arguments: option_number signal_number\n"
                       "option_number: choices = [1,2,3,4,5]\n"
                       "signal_number: choices = [9,10,15,19]\n"
                       "9 - SIGKILL\n"
                       "10 - SIGUSR1\n"
                       "15 - SIGTERM\n"
                       "19 - SIGSTOP\n");
        exit(1);
    }

    threads_number = 10;
    strcpy(file_name, "input.txt");
    number_of_records = 5;
    search_word = "247F9HGNUI";
    option_number = atoi(argv[1]);
    signo = atoi(argv[2]);


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

    if(option_number == 1){
        kill(getpid(), signo);
    } else if(option_number == 2) {
        signal(signo, SIG_IGN);
        kill(getpid(), signo);
    } else if(option_number == 4) {
        if(pthread_kill(threads_list[0], signo) != 0){
            printf("Error:\n"
                           "Function: pthread_kill\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
            exit(1);
        }
    } else if(option_number == 3) {
        if(signal(SIGUSR1, sig_handler) == SIG_ERR){
            printf("Error:\n"
                           "Function: signal\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
            exit(1);
        }
        if(signal(SIGTERM, sig_handler) == SIG_ERR){
            printf("Error:\n"
                           "Function: signal\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
            exit(1);
        }
        kill(getpid(), signo);
    } else if(option_number == 5) {
        if(pthread_kill(threads_list[0], signo) != 0){
            printf("Error:\n"
                           "Function: pthread_kill\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
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

    int y = rand()%4;
    if (y == 0) {
        printf("Will be divided by 0.\n");
        int x = 2/y;
    } else {
        printf("Thread does not divide by 0\n");
    }

    char** records_buffer = calloc(number_of_records, sizeof(char*));
    char* record_id = calloc(2, sizeof(char));

    int characters_read_no;
    int shutdown = 0;

    for(int i = 0; i < number_of_records; i++) {
        records_buffer[i] = calloc(1024, sizeof(char));
    }

    if(option_number == 4) {
        signal(signo, SIG_IGN);
    } else if(option_number == 3 || option_number == 5) {
        if(signal(SIGUSR1, sig_handler) == SIG_ERR) {
            printf("Error:\n"
                           "Function: signal\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
            exit(1);
        }
        if(signal(SIGTERM, sig_handler) == SIG_ERR) {
            printf("Error:\n"
                           "Function: signal\n"
                           "Errno: %d: \n"
                           "Message: %s\n", errno, strerror(errno));
            exit(1);
        }
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

        if(characters_read_no == 0) {
            for(int j = 0; j < threads_number; j++) {
                if(threads_list[j] != pthread_self()) {
                    pthread_cancel(threads_list[j]);
                }
            }
            pthread_mutex_unlock(&mutex);
            shutdown = 1;
            break;
        }

        pthread_mutex_unlock(&mutex);

        for(int i = 0; i < number_of_records; i++) {
            if(strstr(records_buffer[i], search_word) != NULL) {
                // occurence found
                strncpy(record_id, records_buffer[i], 2);
                printf("TID: %ld, record id: %d\n", pthread_self(), atoi(record_id));
            }
        }
    }

    return NULL;
}


