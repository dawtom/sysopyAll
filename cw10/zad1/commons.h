//
// Created by administrator on 25.05.17.
//



#ifndef SYSOPYLAB10CW_COMMONS_H
#define SYSOPYLAB10CW_COMMONS_H

#define MAX_CLIENTS_NUMBER 20
#define MAX_CLIENT_NAME_LENGTH 50
#define BUF_SIZE 1024
#define OPERATION_NAME_SIZE 5
#define OPERATION_FIRST_ARG_SIZE 5
#define OPERATION_SECOND_ARG_SIZE 5

struct operation_message {
    char * operation;
    char * first_argument;
    char * second_argument;
};

#endif //SYSOPYLAB10CW_COMMONS_H
