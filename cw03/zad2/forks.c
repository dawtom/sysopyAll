#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


int long_recursion(int x){
    return long_recursion(x-1);
}

int main(int argc, char ** argv){

    for (int i = 0;i < 1000;i++){
        for (int j = 0; j < 1000; ++j) {
            for (int k = 0; k < 2000; ++k) {
                int x = 1;
            }
        }
    }



    struct rlimit processor_time_limiter;// = malloc(sizeof(struct rlimit));
    struct rlimit virtual_memory_limiter;// = malloc(sizeof(struct rlimit));
    unsigned int time_limit = 1;
    unsigned int memory_limit = 5000;


    processor_time_limiter.rlim_max = time_limit;
    virtual_memory_limiter.rlim_max = memory_limit;
    processor_time_limiter.rlim_cur = time_limit/2;
    virtual_memory_limiter.rlim_cur = memory_limit/2;

    pid_t child_PID;
    pid_t parent_id = getpid();
    char* arg[] = {"ps", "aux", NULL};

    if (parent_id == getpid()){
        child_PID = fork();
        if (child_PID == 0){
            if (setrlimit(RLIMIT_AS, &virtual_memory_limiter) != 0){
                printf("Cannot set memory limit for this process");
            }
            if (setrlimit(RLIMIT_CPU, &processor_time_limiter) != 0){
                printf("Cannot set time limit for this process");
            }

            if (execvp(arg[0],arg) != 0){
                printf("Cannot execute %s",arg[0]);
            }
        } else{
            wait(NULL);
        }
    }


   /* for (int i = 0; i < 50; ++i) {
        //printf("Forks");
        for (int j = 0; j < 10000; ++j) {
            for (int k = 0; k < 1000; ++k) {
                int z = 9;
                z--;
            }
        }
    }
    printf("\n");*/

    /*int i = 0;
    pid_t child_PID;
    pid_t parent_PID = getpid();

    while (++i < 10){
        if (parent_PID == getpid()){
            child_PID = fork();
        }
    }
    printf("%d\n",child_PID);
    //system("pwd");
    //i++;*/


    return 0;
}
