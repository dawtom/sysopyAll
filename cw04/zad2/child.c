#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

clock_t begin;
clock_t end;


void child_sigusr2_handler(int signal_number){
    int random_signal = rand()%32;

    printf("\nchild_handler %d\n",getpid());
    //exit(1);
    union sigval sig_value;
    //sleep(1);
    end = clock();
    //double time_spent =((double) (end - begin) / ((double) CLOCKS_PER_SEC));
    long time_spent = end - begin;
    printf("Time spent: %ld\n",time_spent);
    sig_value.sival_int = time_spent;
    printf("%d",getppid());
    if (getppid()!=15634){
        sigqueue(getppid(),SIGRTMIN+random_signal,sig_value);
    }
}

int main(){

    signal(SIGUSR2,child_sigusr2_handler);

    sleep(3); //FIXME change to 10 seconds according to the task

    union sigval value;
    value.sival_int = getppid();

    if (getppid() != 15634){
        //kill(getppid(),SIGUSR1);
        sigqueue(getppid(),SIGUSR1,value);
        begin = clock();
        //sleep(10);
    }

    pause();
    //while(1){}

    //printf("child");
    return 0;
}