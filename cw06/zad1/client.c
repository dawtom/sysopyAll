#include <stdio.h>
#include <time.h>
#include <zconf.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "common_header.h"


int main() {

    srand(time(NULL));
    int msqid;
    key_t key;
    key_t key_for_server;


    if ((key = ftok(".", 'p')) == -1 ) {
        perror("ftok");
        exit(1);
    }


    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

   // printf("Client will send id on key %d\n", msqid);

    struct message_with_id msg_with_id;
    int x = ftok(".", 'i');
    key_t my_client_msqid;// = x;

    if ((my_client_msqid = msgget(msg_with_id.id, 0666 | IPC_CREAT)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }

    msg_with_id.id = my_client_msqid; //ftok(".",rand()%1000);
    msg_with_id.mtype = NEW_CLIENT_MESSAGE_TYPE;
    //printf("Client sends id (value = %d, key=%d)\n", msg_with_id.id, msqid);

    if (msgsnd(msqid, &msg_with_id, sizeof(struct message_with_id) - sizeof(long), 0) < 0) {
        perror("msgsnd");
        printf("error while sending message");
        exit(1);
    }
    //printf("Message sent %d \n", my_client_msqid);



    struct message_with_id received_message;

    // sleep(1);
    // printf("sizeof msgwithid - sizeof long %d\n", sizeof(struct message_with_id) - sizeof(long));
    if (msgrcv(my_client_msqid, &received_message, sizeof(struct message_with_id) - sizeof(long), 0, 0) < 0) {
        perror("msgrcv");
        printf("Here mistake");
        exit(1);
    } else {
        //printf("Id received from server: %d\n", received_message.id);

        int current_message_type;
        char *receiver_buffer = malloc(sizeof(MAX_MESSAGE_LENGHT_OR_ECHO * sizeof(char)));

        struct request_str to_do_for_server;
        while (1) {
            current_message_type = (rand() % 4) + 1;
            to_do_for_server.mtype = current_message_type;
            to_do_for_server.client_pid = getpid();
            strcpy(to_do_for_server.message, "Message for server");

            printf("Sending to server:\nmtype: %li\n client_pid: %d\n message: %s\n",
                   to_do_for_server.mtype, to_do_for_server.client_pid, to_do_for_server.message);

            if (msgsnd(msqid, &to_do_for_server, sizeof(struct request_str) - sizeof(long), 0) < 0) {
                perror("msgsnd");
            }

            if (current_message_type == TERMINATE_MESSAGE_TYPE) {
                printf("Sending terminate message to server and shut down\n");
                exit(0);
            }

            sleep(1);

            if (msgrcv(msqid, &to_do_for_server, sizeof(struct request_str) - sizeof(long), 0, 0) < 0) {
                perror("msgrcv");
                exit(1);
            }
            printf("Client eventually received: %s\n", to_do_for_server.message);

            sleep(2);
        }



        /*sleep(1);
        if (msgsnd(my_client_msqid, &to_do_for_server, sizeof(struct request_str) - sizeof(long), 0) < 0) {
            perror("msgsnd");
        }*/
        sleep(6);

        printf("Client end\n");
        // msgctl(my_client_msqid, IPC_RMID, NULL);
    }

//*************************************************
    /*struct complex complex1;
    complex1.re = 5;
    complex1.im = -1;

    if (msgsnd(msqid, &complex1, sizeof(struct complex), 0) < 0) {
        perror("msgsnd");
        printf("msgsnd");
    }*/

    /*key_t key = ftok(".", 'p');

    int msqid = msgget(key, 0666);

    char * message_buffer = malloc(50 * sizeof(char));

    size_t size = sizeof(message_buffer);

    sleep(1);
    if (msgrcv(msqid, &message_buffer, size, 0, NULL) == -1) {
        printf("error msgrcv");
        exit(1);
    }
    //printf("%s", message_buffer);*/

    // msgctl(msqid, IPC_RMID, NULL);

    return 0;
}