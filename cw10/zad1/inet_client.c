#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "commons.h"

int internet_socket, local_socket;
char in_buffer[BUF_SIZE];
char un_buffer[BUF_SIZE];
struct sockaddr_in in_serverAddr;
struct sockaddr_un un_serverAddr;
socklen_t in_addr_size;
socklen_t un_addr_size;

char * arg1_client_name;
char * arg2_how_to_connect;
char * arg3_address_or_path;
int arg4_port_number_only_inet;

void parse_args(int argc, char ** argv);
void socket_init_and_connect();
void deserialize_opmessage(char * source);

struct operation_message * op_message;

int main(int argc, char ** argv){

    srand(time(NULL));
    op_message = malloc(sizeof(struct operation_message));
    op_message->operation = malloc(OPERATION_NAME_SIZE * sizeof(char));
    op_message->first_argument = malloc(OPERATION_FIRST_ARG_SIZE * sizeof(char));
    op_message->second_argument = malloc(OPERATION_SECOND_ARG_SIZE * sizeof(char));

    parse_args(argc, argv);

    socket_init_and_connect();

    memset(in_buffer, 0, strlen(in_buffer));

    strcpy(in_buffer, argv[1]);
    write(internet_socket,in_buffer,strlen(in_buffer));

    printf("Sent name to server: %s\n", in_buffer);

    memset(in_buffer,0,strlen(in_buffer));
    while(1){
        recv(internet_socket, in_buffer, BUF_SIZE, 0);

        printf("%s\n", in_buffer);

        deserialize_opmessage(in_buffer);

        printf("Deserialized\n");

        printf("Operation: %s\nFirst argument: %s\nSecond argument: %s\n",
               op_message->operation, op_message->first_argument, op_message->second_argument);

        char operation = op_message->operation[0];
        int first_argument = atoi(op_message->first_argument);
        int second_argument = atoi(op_message->second_argument);

        int result;
        switch (operation) {
            case '+':
                result = first_argument + second_argument;
                break;
            case '-':
                result = first_argument - second_argument;
                break;
            case '*':
                result = first_argument * second_argument;
                break;
            case '/':
                result = first_argument / second_argument;
                break;
        }

        printf("Result: %d\n", result);

        memset(in_buffer,0,strlen(in_buffer));

        sprintf(in_buffer, "Result is %d\n", result);

        write(internet_socket,in_buffer,strlen(in_buffer));

    }

    return 0;
}


void parse_args(int argc, char ** argv){
    arg1_client_name = malloc(MAX_CLIENT_NAME_LENGTH * sizeof(char));
    arg2_how_to_connect = malloc(MAX_CLIENT_NAME_LENGTH * sizeof(char));
    arg3_address_or_path = malloc(MAX_CLIENT_NAME_LENGTH * sizeof(char));
    strcpy(arg1_client_name, argv[1]);
    strcpy(arg2_how_to_connect, argv[2]);
    strcpy(arg3_address_or_path, argv[3]);
    if (argc > 4) {
        printf("port\n");
        arg4_port_number_only_inet = atoi(argv[4]);
    }
}

void socket_init_and_connect(){
    unlink(arg3_address_or_path);
    internet_socket = socket(AF_INET, SOCK_STREAM, 0);
    local_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    in_serverAddr.sin_family = AF_INET;
    in_serverAddr.sin_port = htons(arg4_port_number_only_inet);
    in_serverAddr.sin_addr.s_addr = inet_addr(arg3_address_or_path);
    memset(in_serverAddr.sin_zero, '\0', sizeof in_serverAddr.sin_zero);
    memset(&un_serverAddr, 0, sizeof(un_serverAddr));

    un_serverAddr.sun_family = AF_UNIX;
    strcpy(un_serverAddr.sun_path, arg3_address_or_path);

    in_addr_size = sizeof in_serverAddr;
    un_addr_size = sizeof un_serverAddr;

    connect(internet_socket, (struct sockaddr *) &in_serverAddr, in_addr_size);
}

void deserialize_opmessage(char * source){

    char *token;
    int which = 1;
    token = strtok(source, " ");
    //printf(" %s\n", token);
    strcpy(op_message->operation, token);
    while( token != NULL ) {
        //printf(" %s\n", token);

        token = strtok(NULL, " ");
        if (which == 1) {
            strcpy(op_message->first_argument, token);
            which++;
        } else if (which == 2){
            strcpy(op_message->second_argument, token);
            which++;
        }
    }
}
