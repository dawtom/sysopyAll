//
// Created by administrator on 29.03.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>
#include <string.h>

int forks_number, child_requests_number;
int * child_processes;
int * requested_child_processes;
int count_requests = 0;



/*void parent_sigusr1_handler(int signal_number){
    count_requests++;
    printf("Parent_handler : %d : Received signal number %d.\n",count_requests,signal_number);

    //pause();
}*/

void real_time_signal_handler(int sig_no){
    printf("Finally received a proper signal");
    exit(44);
}

static void parent_sigusr1_handler(int sig, siginfo_t *siginfo, void *context)
{
    requested_child_processes[count_requests] = siginfo->si_pid;
    count_requests++;
    if (count_requests > child_requests_number){
        kill(siginfo->si_pid,SIGKILL);
    } else{
        printf ("Sending PID: %ld :: %d\n", (long)siginfo->si_pid,count_requests);

        //sleep(10);
        printf("Parent of child is %d.\n",siginfo->si_value);
        if (count_requests == child_requests_number){
            printf("\n%d %d\n",count_requests,child_requests_number);
            sigset_t * signal_set = NULL;
            sigfillset(signal_set);

            struct sigaction act;
            memset (&act, '\0', sizeof(act));
            act.sa_sigaction = &real_time_signal_handler;
            act.sa_flags = SA_SIGINFO;
            sigfillset(&act.sa_mask);
            //act.sa_mask = (*signal_set);
            if (sigaction(SIGRTMIN+7, &act, NULL) < 0) {
                printf("sigaction\n");
                exit(99);
            }


            for (int i = 0; i < child_requests_number; ++i) {
                //printf("\n%d",requested_child_processes[i]);
                if (kill(requested_child_processes[i],SIGUSR2) != 0){
                    printf("Error when killing child process.\n");
                }
            }
            //exit(0);
        }
    }

    //pause();

}

int main(int argc, char ** argv){


    forks_number = atoi(argv[1]);
    child_requests_number = atoi(argv[2]);
    child_processes = malloc(forks_number * sizeof(int));
    requested_child_processes = malloc(child_requests_number * sizeof(int));
    //printf("%d",child_requests_number);


    int parent_pid = getpid();
    printf("%d\n\n",parent_pid);



    //system("ps aux | grep 15634");
    for (int i = 0; i < forks_number; ++i) {
        if (parent_pid == getpid()){
            child_processes[i] = fork();
            usleep(100000);
        }
    }

    if (parent_pid == getpid()){
        sleep(1);
        printf("parent\n");

        struct sigaction act;
        memset (&act, '\0', sizeof(act));
        act.sa_sigaction = &parent_sigusr1_handler;
        act.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &act, NULL) < 0) {
            printf("sigaction\n");
            return 1;
        }

        while(1){}

        //usleep(10000);

        //kill(child_processes[0], SIGUSR2);
        //sleep(10);
    } else{
        // CHILD
        //usleep(1000);
        char *env[] = { NULL};
        char *args[] = { NULL };
        if(execve("child", args, env) < 0) {
            printf("Cannot run child program.\nExit.\n");
            exit(1);
        }

    }



    //usleep(1000);
    //system("ps aux| grep 15634");
    //printf("ppid:%d\n",getppid());
    /*if (getppid() != 15634){
        my_child_pid = getpid();
        kill(getppid(),SIGUSR1);
    }*/
    //pause();
    /*usleep(100000);
    if(child_processes[i] == 0){
        // CHILD
        usleep(1000);
        signal(SIGUSR2,child_sigusr2_handler);
        //system("ps aux| grep 15634");
        printf("ppid:%d\n",getppid());
        pause();
        //printf("child\n");
        *//*if (getppid() != 15634){
            kill(getppid(),SIGUSR1);
        }*//*
    } else{
        // PARENT
        usleep(1000);
        signal(SIGUSR1,parent_sigusr1_handler);
        printf("parent\n");
        kill(child_processes[i], SIGUSR2);
        sleep(5);
    }*/


    /*for (int j = 0; j < forks_number; ++j) {
        *//*if (child_processes[j] > 0){
            signal(SIGUSR1,parent_sigusr1_handler);
        }*//*

        if (child_processes[j] == 0){
            // CHILD PROCESS
            //sleep(10);

            printf("%d\n",getppid());
            if (getppid() != 15634){
                //kill(getppid(),SIGUSR1);
            }

            signal(SIGUSR2,child_sigusr2_handler);

        } else{
            // PARENT PROCESS
            kill(child_processes[j],SIGUSR2);
            while (1){
                //printf("lorem\n");
                sleep(1);
            };

        }
    }*/



    return 0;
}