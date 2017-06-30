
#ifndef SYSOPYLAB6_STRUC_H
#define SYSOPYLAB6_STRUC_H

#define MAX_CLIENTS_NUMBER 10
#define MAX_MESSAGE_LENGHT_OR_ECHO 100

#define ECHO_MESSAGE_TYPE 1
#define UPPERCASE_MESSAGE_TYPE 2
#define GET_TIME_MESSAGE_TYPE 3
#define TERMINATE_MESSAGE_TYPE 4

#define QUEUE_NAME  "/server_posix_queue"
#define MAX_SIZE    1024
#define MSG_STOP    "exit"

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \




#endif //SYSOPYLAB6_STRUC_H
