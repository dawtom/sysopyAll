#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "my_queue.h"

void help(){
  printf("Two arguments required (number of clients, number of cuts).\n");
  exit(1);
}

void sigint_hr(int signo){
  exit(2);
}

void cl_queue_init();
void semaphores_init();
void signals_mask_init();
void detach_and_close(void);
int enter_barber_shop();
void get_cut(int number_of_cuts);

my_queue* m_queue = NULL;

sem_t* barber_semaphore;
sem_t* queue_semaphore;
sem_t* check_semaphore;
sem_t* wait_semaphore;

struct timespec time_value;

volatile int cuts_already = 0;
sigset_t fullMask;

void sigrtmin_handler(int signo){
  cuts_already++;
}

int main(int argc, char** argv){
    if(argc != 3) {
        help();
    }
    if(atexit(detach_and_close) == -1) {
        print_error("Error while setting atexit.\n");
    }
    if(signal(SIGINT, sigint_hr) == SIG_ERR) {
        print_error("Error while setting sigint handler.\n");
    }
    if(signal(SIGRTMIN, sigrtmin_handler) == SIG_ERR) {
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
    cl_queue_init();
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
  while(cuts_already < number_of_cuts){
    if(sem_wait(check_semaphore) == -1) {
        print_error("Error while decrementing check semaphore");
    }

    if(sem_wait(queue_semaphore) == -1) {
        print_error("Error while decrementing queue semaphore");
    }

    int res = enter_barber_shop();

    if(sem_post(queue_semaphore) == -1) {
        print_error("Error while incrementing queue semaphore");
    }

    if(sem_post(check_semaphore) == -1) {
        print_error("Error while incrementing check semaphore");
    }

    if(res != -1){
        sigsuspend(&fullMask);

        clock_gettime(CLOCK_MONOTONIC, &time_value);
        printf("%li.%lis >>> Client %d just got cut \n",time_value.tv_sec,time_value.tv_nsec/1000,getpid());
        fflush(stdout);
    }
  }
}

int enter_barber_shop(){
    int barber_state;

    if(sem_getvalue(barber_semaphore, &barber_state) == -1){
        print_error("Error while getting barber semaphore value.\n");
    }

    pid_t my_pid = getpid();

    if(barber_state == 0){
        if(sem_post(barber_semaphore) == -1) {
            print_error("Error while waking barber up.\n");
        }
        clock_gettime(CLOCK_MONOTONIC, &time_value);
        printf("%li.%lis >>> Client %d woke barber up.\n",time_value.tv_sec,time_value.tv_nsec/1000,getpid());

        if(sem_wait(wait_semaphore) == -1) {
            print_error("Error while decrementing wait semaphore");
        }

        m_queue->chair = my_pid;

        return 1;
    } else {
        int tmp = my_queue_push(m_queue, my_pid);
        if(tmp == -1){
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

void cl_queue_init(){
    int shared_m_id = shm_open("/shm", O_RDWR, 0666);
    if(shared_m_id == -1) {
      print_error("Error while opening shared memory.\n");
    }

    void* tmp = mmap(NULL, sizeof(my_queue), PROT_READ | PROT_WRITE, MAP_SHARED, shared_m_id, 0);

    if(tmp == (void*)(-1)) {
        print_error("Error while mapping shared memory.\n");
    }

    m_queue = (my_queue*) tmp;
}

void semaphores_init(){
    barber_semaphore = sem_open("/barber", O_RDWR);
    queue_semaphore = sem_open("/queue", O_RDWR);
    check_semaphore = sem_open("/check", O_RDWR);
    wait_semaphore = sem_open("/wait", O_RDWR);

    if(barber_semaphore == SEM_FAILED) {
        print_error("Error while opening barber semaphore");
    }
    if(queue_semaphore == SEM_FAILED) {
        print_error("Error while opening queue semaphore");
    }
    if(check_semaphore == SEM_FAILED) {
        print_error("Error while opening check semaphore");
    }
    if(wait_semaphore == SEM_FAILED) {
        print_error("Error while opening wait semaphore");
    }
}

void signals_mask_init(){
    sigfillset(&fullMask);
    sigdelset(&fullMask, SIGRTMIN);
    sigdelset(&fullMask, SIGINT);
}

void detach_and_close(void){

    int error = 0;

    if(munmap(m_queue, sizeof(m_queue)) == -1) {
        printf("Error while munmap queue.\n");
        error = 1;
    }

    if(sem_close(barber_semaphore) == -1) {
        printf("Error while closing barber semaphore.\n");
        error = 1;
    }
    if(sem_close(queue_semaphore) == -1) {
        printf("Error while closing queue semaphore.\n");
        error = 1;
    }
    if(sem_close(check_semaphore) == -1) {
        printf("Error while closing check semaphore.\n");
        error = 1;
    }
    if(sem_close(wait_semaphore) == -1) {
        printf("Error while closing wait semaphore.\n");
        error = 1;
    }
    if (!error) {
        printf("Detach and close has been succesfully completed.\n");
    }
}
