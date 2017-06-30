#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string.h>

int received_signals_counter = 0;

void child_sigusr1_handler(int signo){
    received_signals_counter++;
    printf("Child received SIGUSR1. Count: %d\n",received_signals_counter);
    if (getppid() != 15634){
        kill(getppid(),SIGUSR1);
    }
}

void child_sigrt1_handler(int signo){
    received_signals_counter++;
    printf("Child received SIGRT1. Count: %d\n",received_signals_counter);
    if (getppid() != 15634){
        kill(getppid(),SIGRTMIN+1);
    }
}

void child_sigusr2_handler(int signo){
    printf("Child received SIGUSR2. Exit\n");

    exit(received_signals_counter);

}

void child_sigrt2_handler(int signo){
    received_signals_counter++;
    printf("Child received SIGRT2. Count: %d\n",received_signals_counter);
    if (getppid() != 15634){
        kill(getppid(),SIGRTMIN+2);
    }
}

int main(){


    struct sigaction act_end;
    memset (&act_end, '\0', sizeof(act_end));
    act_end.sa_sigaction = &child_sigusr2_handler;
    act_end.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &act_end, NULL) < 0) {
        printf("sigaction\n");
        return 1;
    }

    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &child_sigusr1_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &act, NULL) < 0) {
        printf("sigaction\n");
        return 1;
    }

    signal(SIGRTMIN+1, &child_sigrt1_handler);
    signal(SIGRTMIN+2, &child_sigrt2_handler);

    printf("child\n");



    /*if (getppid() != 15634){
        //kill(getppid(),SIGUSR1);
        sigqueue(getppid(),SIGUSR1,value);
        //sleep(10);
    }*/

    while(1){}

    //printf("child");
    return 0;
}
