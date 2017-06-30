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

int internet_socket, unix_socket;
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

    int r = rand()%4;
    if (r < 2) {
        sprintf(in_buffer,"MyName%d",r);
    } else {
        sprintf(in_buffer, "MyName%d", r + 100);
    }
    if (write(unix_socket, in_buffer, strlen(in_buffer)) == -1) {
        printf("Error while sending data");
    }

    printf("Sent name to server: %s\n", in_buffer);


    memset(in_buffer,0,strlen(in_buffer));
    while(1){
        recv(unix_socket, in_buffer, BUF_SIZE, 0);

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

        write(unix_socket,in_buffer,strlen(in_buffer));

    }

    // recv(internet_socket, in_buffer, BUF_SIZE, 0);

    // shutdown(internet_socket, SHUT_RDWR);
    // close(internet_socket);

/*    printf("Received:\nOperation: %s\nArg1: %s\nArg2: %s\n",
           op_message_received->operation,op_message_received->first_argument,op_message_received->second_argument);*/

/*    strcpy(un_buffer,"Message from client on unix socket\n");
    send(unix_socket,un_buffer,strlen("Message from client on unix socket\n"),0);*/

    /*---- Read the message from the server into the buffer ----*/
/*    recv(internet_socket, in_buffer, 1024, 0);*/

    /*---- Print the received message ----*/
/*    printf("Data received via internet socket %s",in_buffer);*/

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
    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    // unlink(arg3_address_or_path);
    // internet_socket = socket(AF_INET, SOCK_STREAM, 0);
    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket == -1) {
        printf("Error while socket unix\n");
    }

/*

    *//*---- Configure settings of the server address struct ----*//*
    *//* Address family = Internet *//*
    in_serverAddr.sin_family = AF_INET;
    *//* Set port number, using htons function to use proper byte order *//*
    in_serverAddr.sin_port = htons(arg4_port_number_only_inet);
    *//* Set IP address to localhost *//*
    in_serverAddr.sin_addr.s_addr = inet_addr(arg3_address_or_path);
    *//* Set all bits of the padding field to 0 *//*
    memset(in_serverAddr.sin_zero, '\0', sizeof in_serverAddr.sin_zero);*/
    memset(&un_serverAddr, 0, sizeof(un_serverAddr));



    un_serverAddr.sun_family = AF_UNIX;
    strncpy(un_serverAddr.sun_path, arg3_address_or_path, sizeof(un_serverAddr) - 1);

    // strcpy(un_serverAddr.sun_path, arg3_address_or_path);

    /*---- Connect the socket to the server using the address struct ----*/
    // in_addr_size = sizeof in_serverAddr;
    un_addr_size = sizeof un_serverAddr;

    // connect(internet_socket, (struct sockaddr *) &in_serverAddr, in_addr_size);
    if (connect(unix_socket, (struct sockaddr *) &un_serverAddr, un_addr_size) == -1) {
        printf("Error while connect unix\n");
    }
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


/*
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <memory.h>

char * data;
#define DATA_LEN 256

int main() {
    int my_socket;

    my_socket = 3; //socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_UNIX;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = INADDR_LOOPBACK;

    printf("Client port: %d", addr.sin_port);



    data = malloc(DATA_LEN * sizeof(char));
    strcpy(data,"Hello I am socket client");

    if (send(my_socket, data, sizeof(data), 0) == -1) {
        printf("Error while sending via socket");
    }



    return 0;
}*/
