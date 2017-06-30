#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "my_queue.h"

void help(){
    printf("Wrong arguments number. Exactly one required (queue length).\n");
    exit(1);
}
void sigint_hr(int signo){
    printf("Received signal number %d.\nExit.\n", signo);
    exit(2);
}

void detach_and_delete(void);
void queue_init(int queue_length);
void semaphores_init();
void work();
void cut(pid_t pid);
pid_t take_client_from_queue(struct sembuf *);

key_t queue_key;
int sharedmID = -1;
myqueue* queue_handler = NULL;
struct timespec time_value;
int semaphore_id = -1;

int main(int argc, char** argv){
    if(argc != 2) {
      help();
    }
    //set functions to be called at exit
    if(atexit(detach_and_delete) == -1) {
      print_error("Error while setting atexit functions.");
    }

    if (signal(2, sigint_hr) == SIG_ERR) {
      print_error("Error while setting barbers signal");
    }

    queue_init(atoi(argv[1]));
    semaphores_init();
    work();

    return 0;
}

void work(){
    while(1){
        struct sembuf sops;
        sops.sem_num = BARBER;
        sops.sem_op = -1; // decrement
        sops.sem_flg = 0;

        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while performing operations on semaphore.\n");
        } //decrement - block and wait for waking up

        pid_t client_pid = take_client_from_queue(&sops);
        cut(client_pid);

        while(1){
            sops.sem_num = QUEUE;
            sops.sem_op = -1;
            if(semop(semaphore_id, &sops, 1) == -1) {
                print_error("Error while decrementing semaphore");
            }
            // first client from queue to cut
            client_pid = my_queue_pop(queue_handler);

            if(client_pid != -1){
                // client in queue - must cut
                sops.sem_op = 1;
                if(semop(semaphore_id, &sops, 1) == -1) {
                    print_error("Error while incrementing semaphore.\n");
                }

                cut(client_pid);
            } else {
                // -1 - no client in queue - go to sleep
                clock_gettime(CLOCK_MONOTONIC, &time_value);
                printf("%li.%lis >>> Barber sleeps.\n",time_value.tv_sec,time_value.tv_nsec/1000);
                fflush(stdout);

                sops.sem_num = BARBER;
                sops.sem_op = -1;

                if(semop(semaphore_id, &sops, 1) == -1) {
                    print_error("Error while blocking barbers semaphore - barber has an insomnia ;)\n");
                }

                sops.sem_num = QUEUE;
                sops.sem_op = 1;
                if(semop(semaphore_id, &sops, 1) == -1) {
                    print_error("Error while releasing the queue");
                }
                break;
            }
        }
    }
}

pid_t take_client_from_queue(struct sembuf *sops){
    sops->sem_num = QUEUE;
    sops->sem_op = -1;

    if(semop(semaphore_id, sops, 1) == -1) {
      print_error("Cannot decrement semaphore\n");
    }

    pid_t client_pid = queue_handler->chair;

    sops->sem_op = 1;

    if(semop(semaphore_id, sops, 1) == -1) {
      print_error("Cannot increment semaphore.\n");
    }

    return client_pid;
}

void cut(pid_t pid){
    clock_gettime(CLOCK_MONOTONIC, &time_value);
    printf("%li.%lis >>> Start cutting client with pid %d\n",time_value.tv_sec,time_value.tv_nsec/1000,pid);
    fflush(stdout);
    kill(pid, SIGRTMIN);    //actual cut
    clock_gettime(CLOCK_MONOTONIC, &time_value);
    printf("%li.%lis >>> Finished cutting client with pid %d\n",time_value.tv_sec,time_value.tv_nsec/1000,pid);
    fflush(stdout);
}

void queue_init(int queue_length){
    if(queue_length < 5 || queue_length > 1000) {
        print_error("Wrong number of Chairs!");
        exit(1);
    }

    char* home_env_var = getenv("HOME");
    if(home_env_var == NULL) {
        print_error("Error while getting HOME value");
    }

    queue_key = ftok(home_env_var, 'p');

    if(queue_key == -1) {
        print_error("Error while accessing key for shared memory.\n");
    }

    sharedmID = shmget(queue_key, sizeof(struct my_queue), IPC_CREAT | IPC_EXCL | 0666);

    if(sharedmID == -1) {
        print_error("Error while accesing shared memory.\n");
    }

    void* access_memory = (myqueue*) shmat(sharedmID, NULL, 0);
    if(access_memory == (void*)(-1)) {
        print_error("Error while accessing memory.\n");
    }

    queue_handler = (myqueue*) access_memory;

    my_queue_init(queue_handler, queue_length);
}

void semaphores_init(){
    semaphore_id = semget(queue_key, 4, IPC_CREAT | IPC_EXCL | 0666);
    if(semaphore_id == -1) {
      print_error("Error while accessing semaphore.\n");
    };

    for(int i=1; i<3; i++){
    if(semctl(semaphore_id, i, SETVAL, 1) == -1) {
        print_error("Error while setting semaphores 1-3 values.");
    }
    }
    if(semctl(semaphore_id, 0, SETVAL, 0) == -1) {
      print_error("Error while setting semaphore 0 value");
    }
}

void detach_and_delete(void){
    int error = 0;

    if(shmdt(queue_handler) == -1) {
      printf("Error while shmdt(queue_handler)\n");
      error = 1;
    }

    if(shmctl(sharedmID, IPC_RMID, NULL) == -1) {
      printf("Error while removing shared memory\n");
      error = 1;

    }

    if(semctl(semaphore_id, 0, IPC_RMID) == -1) {
      printf("Error while deleting semafors!");
      error = 1;
    }

    if (!error) {
        printf("Detach and delete has been succesfully completed");
    }
}
