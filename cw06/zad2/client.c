#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "common_header.h"
#include <string.h>
#include <mqueue.h>
#include <zconf.h>

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    mqd_t mq;
    mqd_t my_client_mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];
    char message_type = 1;
    ssize_t bytes_read;
    int shutdown = 0;
    char message_for_server_buffer[100];
    int current_message_type;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* open the mail queue */
    mq = mq_open(QUEUE_NAME, O_RDWR);
    // CHECK((mqd_t)-1 != mq);


    printf("Send to server (enter \"exit\" to stop it):\n");

    printf("> ");
    fflush(stdout);

    sprintf(buffer, "Message for server");
    /*memset(buffer, 0, MAX_SIZE);
    fgets(buffer, MAX_SIZE, stdin);*/

    rand_str(buffer, 15);
    buffer[0] = '/';

    my_client_mq = mq_open(buffer, O_CREAT | O_RDWR, 0644, &attr);

    /* send the message */
    CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));

    //receive id message from server

    bytes_read = mq_receive(my_client_mq, buffer, MAX_SIZE, NULL);
    CHECK(bytes_read >= 0);
    printf("Client has received its id=%s\n", buffer);

    while(!shutdown){
        current_message_type = (rand() % 4) + 1;
        if (current_message_type == 4) {
            shutdown = 1;
        }

        sprintf(message_for_server_buffer, "%dMessage for server to process", current_message_type);

        CHECK(0 <= mq_send(my_client_mq, message_for_server_buffer, MAX_SIZE, 0));
	printf("\n*******************************************\n");
        printf("Client has sent: %s\n", message_for_server_buffer);

        bytes_read = mq_receive(my_client_mq, buffer, MAX_SIZE, NULL);
        CHECK(bytes_read >= 0);
        printf("Client receives back: %s\n", buffer);

        sleep(2);
    }

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_close(my_client_mq));

    return 0;
}
