#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv){

    if (getenv(argv[1]) != NULL){
        printf("\nI am a tester program.\nname: %s",getenv(argv[1]));
    } else{
        printf("No such variable");
    }

    /*for (int i = 0; i < 50; ++i) {
        printf("Forks");
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
