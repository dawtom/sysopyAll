#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>
#include <string.h>

#define TYPE_KILL 1
#define TYPE_SIGQUEUE 2
#define TYPE_SIGRT_KILL 3

int sent_signals_counter = 0;
int received_signals_counter = 0;
int signals_number;
int type;

void my_kill_function(int pid, int signo, int type){
    if (type == TYPE_KILL){
        kill(pid,signo);
    } else{
        if (type == TYPE_SIGQUEUE){
            union sigval value;
            sigqueue(pid, signo, value);
        } else{
            if (type == TYPE_SIGRT_KILL){
                kill(pid,SIGRTMIN+1);
            } else{
                printf("Error with type assignment. Exit.\n");
                exit(1);
            }
        }
    }
}

struct sigaction act_end;

int child_pid;

void parent_sigusr1_handler(int signo){
    received_signals_counter++;
    // printf("Parent received sigusr1, count: %d.\n",received_signals_counter);
}

void parent_sigrt1_handler(int signo){
    received_signals_counter++;
}

void sigint_handler(int signo){
    printf("Received sigint. Exit and kill child process\n");
    my_kill_function(child_pid,9,type);
    exit(2);
}

int main(int argc, char ** argv){



    signals_number = atoi(argv[1]);
    type = atoi(argv[2]);

    int parent_pid = getpid();
    printf("%d\n\n",parent_pid);
    signal(SIGINT,sigint_handler);
    child_pid = fork();

    sleep(1);

    if (child_pid < 0){
        printf("FORK ERROR.\nEXIT.\n");
        exit(1);
    } else

    if (child_pid == 0){
        // CHILD
        //usleep(1000);
        char *env[] = { NULL};
        char *args[] = { NULL };
        if(execve("child", args, env) < 0) {
            printf("Cannot run child program.\nExit.\n");
            exit(1);
        }

    } else{
        // PARENT

        memset (&act_end, '\0', sizeof(act_end));
        act_end.sa_sigaction = &parent_sigusr1_handler;
        act_end.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &act_end, NULL) < 0) {
            printf("sigaction\n");
            return 1;
        }

        signal(SIGRTMIN+1,&parent_sigrt1_handler);

        //sleep(1);
        printf("parent\n");

        sleep(1);

        for (int i = 0; i < signals_number; ++i) {
            my_kill_function(child_pid,SIGUSR1,type);
            sent_signals_counter++;
            sleep(1);
        }

        my_kill_function(child_pid,SIGUSR2,type);

        sleep(1);
        printf("Parent sent %d signals.\n",signals_number);
        printf("Parent received %d signals.\n",received_signals_counter);
        exit(0);

        //wait(NULL);
        //while(1){}

    }





    return 0;
}