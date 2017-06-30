#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>

#define BACKWARDS -1
#define FORWARDS 1
#define ALPHABET_SIZE 26

int how_to_display = BACKWARDS;

void signal_handler_function(int signal_number){
    if (how_to_display == BACKWARDS){
        how_to_display = FORWARDS;
    } else{
        how_to_display = BACKWARDS;
    }

    printf("\nHow to display %d\n",how_to_display);
    printf("Received signal %d.\n Changing display order.\n",signal_number);
}

void sigint_handler(int signal_number){
    printf("Received signal SIGINT (number %d).\nExit\n",signal_number);
    exit(1);
}

int main(){



    signal(SIGTSTP,signal_handler_function);

    struct sigaction sigint_action;
    sigint_action.sa_handler = sigint_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;
    sigaction(SIGINT, &sigint_action, NULL);

    sleep(2);

    char * letters = malloc(ALPHABET_SIZE * sizeof(char));
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        letters[i] = i + 65;
    }
    unsigned int k = 10400;

    while(1){
        printf("%c\n",letters[k % ALPHABET_SIZE]);
        k += how_to_display;
        sleep(1);
    }



    return 0;
}