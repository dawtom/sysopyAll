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

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

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
pid_t take_client_from_queue();

my_queue* m_queue = NULL;

sem_t* barber_semaphore;
sem_t* queue_semaphore;
sem_t* check_semaphore;
sem_t* wait_semaphore;

struct timespec time_value;

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
        if (sem_wait(barber_semaphore) == -1) {
            print_error("Error while decrementing barber semaphore"); // wait for a client
        }

        if (sem_post(barber_semaphore) == -1) {
            print_error("Error while incrementing barber semaphore, barber does not realize he does not sleep.");
        }

        if (sem_post(wait_semaphore) == -1) {
            print_error("Error while incrementing wait semaphore");
        }

        pid_t tmp_client_pid = take_client_from_queue();
        cut(tmp_client_pid);

        while(1){
            if (sem_wait(queue_semaphore) == -1) {
                print_error("Error while decrementing queue semaphore");
            }
            // block queue semaphore and cut first client from the queue
            tmp_client_pid = my_queue_pop(m_queue);

            // if there is a client cut and keep waiting
            if(tmp_client_pid != -1){
                if (sem_post(queue_semaphore) == -1) {
                    print_error("Error while incrementing queue semaphore");
                }
                cut(tmp_client_pid);
            } else {
                // no clients - barber waits, queue semaphore is released
                clock_gettime(CLOCK_MONOTONIC, &time_value);
                printf("%li.%lis >>> Barber sleeps.\n",time_value.tv_sec,time_value.tv_nsec/1000);
                fflush(stdout);


                if (sem_wait(barber_semaphore) == -1) {
                    print_error("Error while decrementing barber semaphore");
                }

                if (sem_post(queue_semaphore) == -1) {
                    print_error("Error while incrementing queue semaphore");
                }
                break;
          }
        }
    }
}

pid_t take_client_from_queue(){
    if (sem_wait(queue_semaphore) == -1) {
        print_error("Error while decrementing queue semaphore");
    }

    pid_t client_pid = m_queue->chair;

    if (sem_post(queue_semaphore) == -1) {
        print_error("Error while incrementing queue semaphore");
    }

    return client_pid;
}

void cut(pid_t pid){
    clock_gettime(CLOCK_MONOTONIC, &time_value);
    printf("%li.%lis >>> Start cutting client with pid %d\n",time_value.tv_sec,time_value.tv_nsec/1000,pid);
    fflush(stdout);

    kill(pid, SIGRTMIN);

    clock_gettime(CLOCK_MONOTONIC, &time_value);
    printf("%li.%lis >>> Finished cutting client with pid %d\n",time_value.tv_sec,time_value.tv_nsec/1000,pid);
    fflush(stdout);
}

void queue_init(int queue_length){
    if(queue_length < 5 || queue_length > 1000) {
        print_error("Wrong number of chairs!");
        exit(1);
    }

    int shared_m_ID = shm_open("/shm", O_CREAT | O_EXCL | O_RDWR, 0666);

    if(shared_m_ID == -1) {
        print_error("Error while creating shared memory.\n");
    }

    if(ftruncate(shared_m_ID, sizeof(my_queue)) == -1) {
        print_error("Error while truncating shared memory.\n");
    }

    void* memory_access = mmap(NULL, sizeof(my_queue), PROT_READ | PROT_WRITE, MAP_SHARED, shared_m_ID, 0);
    if(memory_access == (void*)(-1)) {
        print_error("Error while mapping shared memory.\n");
    }

    m_queue = (my_queue*) memory_access;

    my_queue_init(m_queue, queue_length);
}

void semaphores_init(){

    barber_semaphore = sem_open("/barber", O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    queue_semaphore = sem_open("/queue", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    check_semaphore = sem_open("/check", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    wait_semaphore = sem_open("/wait", O_CREAT | O_EXCL | O_RDWR, 0666, 0);

    if(wait_semaphore == SEM_FAILED) {
        print_error("Error while accessing barber semaphore.\n");
    }
    if(barber_semaphore == SEM_FAILED) {
        print_error("Error while accessing barber semaphore.\n");
    }
    if(queue_semaphore == SEM_FAILED) {
        print_error("Error while accessing queue semaphore.\n");
    }
    if(check_semaphore == SEM_FAILED) {
        print_error("Error while accessing check semaphore.\n");
    }

}

void detach_and_delete(void){

    int error = 0;
    if(munmap(m_queue, sizeof(m_queue)) == -1) {
        printf("Error while munmap shared memory.\n");
        error = 1;
    }
    if(shm_unlink("/shm") == -1) {
        printf("Error while unlinking barber semaphore");
        error = 1;
    }

    if(sem_close(barber_semaphore) == -1) {
        printf("Error while closing barber semaphore");
        error = 1;
    }
    if(sem_close(queue_semaphore) == -1) {
        printf("Error while closing queue semaphore");
        error = 1;
    }
    if(sem_close(check_semaphore) == -1) {
        printf("Error while closing check semaphore");
        error = 1;
    }
    if(sem_close(wait_semaphore) == -1) {
        printf("Error while closing wait semaphore");
        error = 1;
    }

    if(sem_unlink("/barber") == -1) {
        printf("Error while unlinking barber semaphore");
        error = 1;
    }
    if(sem_unlink("/queue") == -1) {
        printf("Error while unlinking queue semaphore");
        error = 1;
    }
    if(sem_unlink("/check") == -1) {
        printf("Error while unlinking check semaphore");
        error = 1;
    }
    if(sem_unlink("/wait") == -1) {
        printf("Error while unlinking wait semaphore");
        error = 1;
    }

    if (!error) {
        printf("Detach and delete has been successfully completed.\n");
    }

}
