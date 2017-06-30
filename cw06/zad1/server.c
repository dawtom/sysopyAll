#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include "common_header.h"

int msqid;
int client_msqid;
int shutdown = 0;

void shutdown_server(int signo){
    msgctl(msqid, IPC_RMID, NULL);
    msgctl(client_msqid, IPC_RMID, NULL);
    printf("Received signal 2\n");
    exit(1);
}

int main() {

    srand(time(NULL));

    signal(2, &shutdown_server);
    key_t key;

    int client_ids[MAX_CLIENTS_NUMBER];
    int tmp_client_id = 0;

    if ((key = ftok(".", 'p')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) { /* msqid - server queue */
        perror("msgget");
        exit(1);
    }

    printf("Server waiting for messages with id %d.\n", msqid);
    // Now server is waiting on msqid queue

    // sleep(2);

    int r = 1;
    int z = 0;
    time_t rawtime;
    struct tm *info;
    char buffer[80];


    while (!shutdown) {
        struct message_with_id response;
        // server wants to receive message with client queue id
        if (msgrcv(msqid, &response, sizeof(struct message_with_id) - sizeof(long), 0, 0) == -1) {
             perror("msgrcv");
             printf("Cannot receive message");
             exit(1);
        } else{
            printf("Server has received: type: %li, message: %s\n", response.mtype, response.message);

            switch (response.mtype) {
                case ECHO_MESSAGE_TYPE:
                    printf("Server sends back the same message: %s\n", response.message);
                    break;
                case UPPERCASE_MESSAGE_TYPE:
                    for (int i = 0; i < strlen(response.message); ++i) {
                        response.message[i] = toupper(response.message[i]);
                    }
                    printf("Server sends back uppercase message: %s\n", response.message);
                    break;
                case GET_TIME_MESSAGE_TYPE:
                    time( &rawtime );
                    info = localtime( &rawtime );
                    strftime(buffer,80,"%x - %I:%M%p", info);
                    printf("Server sends back formatted date & time : |%s|\n", buffer );
                    strcpy(response.message, buffer);
                    break;
                case TERMINATE_MESSAGE_TYPE:
                    shutdown = 1;
                    printf("Server will shut down.\n");
                    break;
                case NEW_CLIENT_MESSAGE_TYPE:
                    printf("\nClient msqid: %d\n", response.id);
                    client_msqid = response.id;

                    client_ids[tmp_client_id] = tmp_client_id;
                    struct message_with_id id_to_client;
                    id_to_client.mtype = 3;
                    id_to_client.id = tmp_client_id;


                    if (msgsnd(client_msqid, &id_to_client, sizeof(struct message_with_id) - sizeof(long),0) < 0) {
                        perror("msgsnd");
                    }
                    printf("Sent client id: %d\n", id_to_client.id);
                    tmp_client_id++;

                    sleep(1);
                    break;
                default:
                    printf("Wrong message type\n Exit\n");
                    exit(1);

            }

            if (response.mtype != NEW_CLIENT_MESSAGE_TYPE) {
                // printf("Type: %li\nMessage: %s \n",response.mtype, response.message);
                if (msgsnd(msqid, &response, sizeof(struct request_str) - sizeof(long),0) < 0) {
                    perror("msgsnd");
                }
                printf("Sent more: %s\n", response.message);
                sleep(1);
            }


        }
    }
    raise(2);
    // msgctl(msqid, IPC_RMID, NULL);


    /*key_t key = ftok(".", 'p');

    int msqid = msgget(key, IPC_CREAT | 0666);

    char * message_buffer = malloc(50 * sizeof(char));

    int size = sizeof(message_buffer);

    for (int i = 0; i < 10; ++i) {
        sprintf(message_buffer, "dupa%d\n", i);
        msgsnd(msqid, &message_buffer, size, 0);
        sleep(1);
    }*/




    return 0;
}