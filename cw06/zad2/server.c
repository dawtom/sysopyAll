#include "common_header.h"
#include <errno.h>
#include <time.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <zconf.h>
#include <ctype.h>
#include <sys/time.h>

mqd_t mq;
mqd_t client_mq;

void sigint_handler(int signo) {
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));
    CHECK((mqd_t)-1 != mq_close(client_mq));
}

int main(int argc, char **argv)
{
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    char cliend_id_buffer[MAX_SIZE + 1];
    int shutdown = 0;
    time_t rawtime;
    struct tm *info;
    char * p;

    int client_ids[MAX_CLIENTS_NUMBER];
    int tmp_client_id = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 != mq);

    do {
        ssize_t bytes_read;

        /* receive the message with queue name */
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        CHECK(bytes_read >= 0);
        buffer[bytes_read] = '\0';
        printf("Server has received: %s\n", buffer);

        client_ids[tmp_client_id] = tmp_client_id;
        tmp_client_id++;

        client_mq = mq_open(buffer, O_CREAT | O_RDWR, 0644, &attr);
        sprintf(cliend_id_buffer, "%d", tmp_client_id);
        CHECK(0 <= mq_send(client_mq, cliend_id_buffer, MAX_SIZE, 0));
        printf("Server has sent id to client (id=%s)\n", cliend_id_buffer);
        while (!shutdown) {
            bytes_read = mq_receive(client_mq, buffer, MAX_SIZE, NULL);
            CHECK(bytes_read >= 0);

            p = buffer + 1;

	    printf("\n*******************************************\n");

            switch (buffer[0]) {
                case ECHO_MESSAGE_TYPE + 48:
                    break;
                case UPPERCASE_MESSAGE_TYPE + 48:
                    for (int i = 0; i < strlen(buffer); ++i) {
                        buffer[i] = toupper(buffer[i]);
                    }
                    break;
                case GET_TIME_MESSAGE_TYPE + 48:
                    time( &rawtime );
                    info = localtime( &rawtime );
                    strftime(buffer,80,"%x - %I:%M%p", info);
                    printf("Formatted date & time : |%s|\n", buffer );
                    break;
                case TERMINATE_MESSAGE_TYPE + 48:
                    shutdown = 1;
                    break;
                default:
                    printf("Wrong message type\n Exit\n");
                    exit(1);
            }

            printf("Server sends back: %s\n", buffer);
            CHECK(0 <= mq_send(client_mq, buffer, MAX_SIZE, 0));

            sleep(1);
        }
    } while (!shutdown);

    signal(2, sigint_handler);

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

    return 0;
}
