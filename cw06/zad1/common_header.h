//
// Created by administrator on 14.04.17.
//

#ifndef SYSOPYLAB6_STRUC_H
#define SYSOPYLAB6_STRUC_H

#define MAX_CLIENTS_NUMBER 10
#define MAX_MESSAGE_LENGHT_OR_ECHO 100

#define ECHO_MESSAGE_TYPE 1
#define UPPERCASE_MESSAGE_TYPE 2
#define GET_TIME_MESSAGE_TYPE 3
#define TERMINATE_MESSAGE_TYPE 4
#define NEW_CLIENT_MESSAGE_TYPE 5



struct message_with_id {
    long int mtype;
    pid_t id;
    char message[100];
};

struct request_str{
    long int mtype;
    pid_t client_pid;
    char message[100];
};


#endif //SYSOPYLAB6_STRUC_H
