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
#include <sys/wait.h>

#include "my_queue.h"

void help(){
    printf("Two arguments required (number of clients, number of cuts).\n");
    exit(1);
}

void sigint_hr(int signo){
    exit(2);
}

void queue_init();
void semaphores_init();
void signals_mask_init();
void detach(void);
int enter_barber_shop();
void get_cut(int number_of_cuts);

key_t queue_key;
int sharedmID = -1;
myqueue* queue_handler = NULL;
int semaphore_id = -1;

volatile int cuts_number_already = 0;
sigset_t full_mask;

struct timespec time_value;

void signalrtmin_increase_counts_handler(int signo){
    cuts_number_already++;
}

int main(int argc, char** argv){
    if(argc != 3) {
      help();
    }
    if(atexit(detach) == -1) {
      print_error("Error while setting atexit.\n");
    }
    if(signal(SIGINT, sigint_hr) == SIG_ERR) {
      print_error("Error while setting sigint handler.\n");
    }
    if(signal(SIGRTMIN, signalrtmin_increase_counts_handler) == SIG_ERR) {
      print_error("Error while setting sigrtmin handler.\n");
    }

    int number_of_clients = atoi(argv[1]);

    if(number_of_clients < 1 || number_of_clients > 500) {
        print_error("Too few or too many clients");
    }

    int number_of_cuts = atoi(argv[2]);

    if (number_of_cuts < 1 || number_of_cuts > 15) {
        print_error("Too few or too many cuts");
    }


    queue_init();
    semaphores_init();
    signals_mask_init();

    sigset_t mask;
    if(sigemptyset(&mask) == -1) {
        print_error("Error while setting empty set.\n");
    }
    if(sigaddset(&mask, SIGRTMIN) == -1) {
        print_error("Error while adding SIGRTMIN set.\n");
    }
    if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        print_error("Error while sigprocmask");
    }

    for(int i=0; i<number_of_clients; i++){
        pid_t child_pid = fork();
        if(child_pid == -1) {
            print_error("Error while forking.\n");
        }
        if(child_pid == 0){
            get_cut(number_of_cuts);
            return 0;
        }
    }

    printf("All clients cut.\n");

    while(1){
        wait(NULL);
        if (errno == ECHILD) break;
    }

return 0;
}

void get_cut(int number_of_cuts){
    while(cuts_number_already < number_of_cuts){
        struct sembuf sops;
        sops.sem_num = CHECKER;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while performing operations on semaphore(decrement).\n");
        }

        sops.sem_num = QUEUE;
        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while performing operations on semaphore(decrement).\n");
        }

        int res = enter_barber_shop();

        sops.sem_op = 1;
        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while performing operations on semaphore(increment).\n");
        }

        sops.sem_num = CHECKER;
        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while performing operations on semaphore(increment).\n");
        }

        if(res != -1){
            sigsuspend(&full_mask);
            clock_gettime(CLOCK_MONOTONIC, &time_value);
            printf("%li.%lis >>> Client %d just got cut \n",time_value.tv_sec,time_value.tv_nsec/1000,getpid());
        }
    }
}

int enter_barber_shop(){
    int barber_state = semctl(semaphore_id, 0, GETVAL);
    if(barber_state == -1) {
        print_error("Error while getting state of barber.\n");
    }

    pid_t my_pid = getpid();

    if(barber_state == 0){
        struct sembuf sops;
        sops.sem_num = BARBER;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        if(semop(semaphore_id, &sops, 1) == -1){
            print_error("Error while waking barber up.\n");
        }

        clock_gettime(CLOCK_MONOTONIC, &time_value);
        printf("%li.%lis >>> Client %d has woken barber up \n",time_value.tv_sec,time_value.tv_nsec/1000,my_pid);

        fflush(stdout);

        if(semop(semaphore_id, &sops, 1) == -1) {
            print_error("Error while waking barber up.\n");
        }

        queue_handler->chair = my_pid;

        return 1;
    } else {
        int res = my_queue_push(queue_handler, getpid());
        if(res == -1){
            clock_gettime(CLOCK_MONOTONIC, &time_value);
            printf("%li.%lis >>> No place in queue for client %d \n",time_value.tv_sec,time_value.tv_nsec/1000,my_pid);
            fflush(stdout);
            return -1;
        } else {
            clock_gettime(CLOCK_MONOTONIC, &time_value);
            printf("%li.%lis >>> Client %d is in the queue \n",time_value.tv_sec,time_value.tv_nsec/1000,my_pid);
            fflush(stdout);
            return 0;
    }
}
}

void queue_init(){
    char* home_env_var = getenv("HOME");
    if(home_env_var == NULL) {
        print_error("Error while getting PATH value.\n");
    }

    queue_key = ftok(home_env_var, 'p');
    if(queue_key == -1) {
        print_error("Error while accessing key for shared memory.\n");
    }

    sharedmID = shmget(queue_key, 0, 0);

    if(sharedmID == -1) {
        print_error("Error while accesing shared memory.\n");
    }

    void* access_memory = (myqueue*) shmat(sharedmID, NULL, 0);

    if(access_memory == (void*)(-1)) {
        print_error("Error while accessing memory.\n");
    }

    queue_handler = (myqueue*) access_memory;
}

void semaphores_init(){
    semaphore_id = semget(queue_key, 0, 0);
    if(semaphore_id == -1) {
      print_error("Error while opening client semaphores.\n");
    }
}

void signals_mask_init(){
    sigfillset(&full_mask);
    sigdelset(&full_mask, SIGRTMIN);
    sigdelset(&full_mask, SIGINT);
}

void detach(void){
    if(shmdt(queue_handler) == -1) {
        printf("Error while detaching queue handler.\n");
    }
}
