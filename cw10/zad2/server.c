#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <zconf.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/un.h>
#include "commons.h"

#define MAXEVENTS 64

static int create_and_bind_inet(char *port);
static int make_socket_non_blocking (int sfd);
void parse_args(int argc, char ** argv);
void call_error(char * message);
void * network_thread_function(void *arg);
void * computing_thread_function(void *arg);
int string_array_contains(char *array[], int array_length, char *string);
int * client_descriptors;
char * clients_names[MAX_CLIENTS_NUMBER];
struct sockaddr_un un_serverAddr;

int client_count = 0;




pthread_t network_thread_handler;
pthread_t computing_thread_handler;
int sfd, s;
int efd;
void check_doubles(int * arr, int len);
struct epoll_event event;
struct epoll_event *events;
struct operation_message * op_message;
char * arg1_addr_port;
char * arg2_unix_path;



void sigint_handler(int signo){
    printf("Received SIGINT. Server shutdown\n");
    close (sfd);
    exit(0);
}

static int create_and_bind_unix();
int ufd;


int main (int argc, char *argv[])
{
    srand(time(NULL));

    signal(SIGINT, sigint_handler);

    for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
        clients_names[i] = calloc(MAX_CLIENT_NAME_LENGTH, sizeof(char));
    }
    client_descriptors = calloc(MAX_CLIENTS_NUMBER, sizeof(int));
    op_message = malloc(sizeof(struct operation_message));
    op_message->operation = malloc(OPERATION_NAME_SIZE * sizeof(char));
    op_message->first_argument = malloc(OPERATION_FIRST_ARG_SIZE * sizeof(char));
    op_message->second_argument = malloc(OPERATION_SECOND_ARG_SIZE * sizeof(char));

    parse_args(argc, argv);


    sfd = create_and_bind_inet("7891");
    if (sfd == -1)
        abort ();
    ufd = create_and_bind_unix();
    if (ufd == -1) {
        printf("Wrong create and bind unix\n");
    }


    s = make_socket_non_blocking (sfd);
    s = make_socket_non_blocking (ufd);
    if (s == -1)
        abort ();



    if (pthread_create(&network_thread_handler, NULL, network_thread_function, 0) != 0) {
        call_error("Error while starting network thread.\n");
    }

    if (pthread_create(&computing_thread_handler, NULL, computing_thread_function, 0) != 0) {
        call_error("Error while starting computing thread.\n");
    }

    if (pthread_join(network_thread_handler, NULL) != 0) {
        call_error("Error while joining network thread");
    }
    if (pthread_join(computing_thread_handler, NULL) != 0) {
        call_error("Error while joining computing thread");
    }


}

static int make_socket_non_blocking (int sfd)
{
    int flags, s;

    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror ("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1)
    {
        perror ("fcntl");
        return -1;
    }

    return 0;
}

static int create_and_bind_inet(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */
    hints.ai_addr = malloc(sizeof(struct sockaddr));
    strcpy(hints.ai_addr->sa_data, arg1_addr_port);

    s = getaddrinfo (NULL, port, &hints, &result);
    if (s != 0)
    {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0)
        {
            /* We managed to bind successfully! */
            break;
        }

        close (sfd);
    }

    if (rp == NULL)
    {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo (result);

    return sfd;
}
static int create_and_bind_unix(){

    int unix_socket;
    unlink(arg2_unix_path);

    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&un_serverAddr, 0, sizeof(un_serverAddr));

    un_serverAddr.sun_family = AF_UNIX;
    strncpy(un_serverAddr.sun_path, "un_server", strlen("un_server"));
    unlink("un_server");

    if (bind(unix_socket, (struct sockaddr *) &un_serverAddr, sizeof(un_serverAddr)) == -1) {
        printf("Error binding unix socket\n");
        return -1;
    }

    return unix_socket;
}

void * network_thread_function(void *arg){
    s = listen (sfd, SOMAXCONN);
    if (s == -1)
    {
        perror ("listen");
        abort ();
    }

    efd = epoll_create1 (0);
    if (efd == -1)
    {
        perror ("epoll_create");
        abort ();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1)
    {
        perror ("epoll_ctl");
        abort ();
    }

    event.data.fd = ufd;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);

    /* Buffer where events are returned */
    events = calloc (MAXEVENTS, sizeof event);

    /* The event loop */
    while (1)
    {
        int n;

        n = epoll_wait (efd, events, MAXEVENTS, -1);
        for (int i = 0; i < n; i++)
        {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                /* An error has occured on this fd, or the socket is not
                   ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            }

            else if (sfd == events[i].data.fd)
            {
                /* We have a notification on the listening socket, which
                   means one or more incoming connections. */
                while (1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof in_addr;
                    infd = accept (sfd, &in_addr, &in_len);
                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            /* We have processed all incoming
                               connections. */
                            break;
                        }
                        else
                        {
                            perror ("accept");
                            break;
                        }
                    }

                    s = getnameinfo (&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (s == 0)
                    {
                        printf("Accepted connection on descriptor %d "
                                       "(host=%s, port=%s)\n", infd, hbuf, sbuf);




                    }

                    /* Make the incoming socket non-blocking and add it to the
                       list of fds to monitor. */
                    s = make_socket_non_blocking (infd);
                    if (s == -1)
                        abort ();

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
                    if (s == -1)
                    {
                        perror ("epoll_ctl");
                        abort ();
                    }
                }
                continue;
            }
            else
            {
                /* We have data on the fd waiting to be read. Read and
                   display it. We must read whatever data is available
                   completely, as we are running in edge-triggered mode
                   and won't get a notification again for the same
                   data. */
                int done = 0;

                while (1)
                {
                    ssize_t count;
                    char buf[512];
                    memset(buf, 0, strlen(buf));
                    count = read (events[i].data.fd, buf, sizeof buf);

                    printf("\nAll clients\n");
                    for (int j = 0; j < MAX_CLIENTS_NUMBER; ++j) {
                        printf("%s ", clients_names[j]);
                    }
                    printf("\n***\n");

                    if (string_array_contains(clients_names, client_count, buf) == 1) {
                        printf("Such client exists");
                        strcpy(buf, "n");
                    } else {
                        printf("Client registered on descriptor %d\n", events[i].data.fd);
                        client_descriptors[client_count] = events[i].data.fd;
                        strcpy(clients_names[client_count], buf);
                        client_count++;
                    }


                    if (count == -1)
                    {
                        /* If errno == EAGAIN, that means we have read all
                           data. So go back to the main loop. */
                        if (errno != EAGAIN)
                        {
                            perror ("read");
                            done = 1;
                        }
                        break;
                    }
                    else if (count == 0)
                    {
                        /* End of file. The remote has closed the
                           connection. */
                        done = 1;
                        break;
                    }

                    s = write (1, buf, count);
                    if (s == -1)
                    {
                        perror ("write");
                        abort ();
                    }
                }

                if (done)
                {
                    printf ("Closed connection on descriptor %d\n",
                            events[i].data.fd);
                    for (int j = 0; j < MAX_CLIENTS_NUMBER; ++j) {
                        if (client_descriptors[j] == events[i].data.fd) {
                            client_descriptors[j] = 0;
                        }
                    }

                    /* Closing the descriptor will make epoll remove it
                       from the set of descriptors which are monitored. */
                    close (events[i].data.fd);
                }
            }
        }
    }

    free (events);



    return 0;
}

void * computing_thread_function(void *arg){
    while (1) {
        char * c;
        c = malloc(12 * sizeof(char));

        printf("Operation:\n");
        scanf("%s", c);
        strcpy(op_message->operation, c);

        printf("First argument:\n");
        scanf("%s", c);
        strcpy(op_message->first_argument,c);

        printf("Second argument:\n");
        scanf("%s", c);
        strcpy(op_message->second_argument,c);

        char *serialized_operation_message =
                malloc((5 + OPERATION_NAME_SIZE + OPERATION_FIRST_ARG_SIZE + OPERATION_SECOND_ARG_SIZE)
                       * sizeof(char));
        strcpy(serialized_operation_message, op_message->operation);
        strcat(serialized_operation_message, " ");
        strcat(serialized_operation_message, op_message->first_argument);
        strcat(serialized_operation_message, " ");
        strcat(serialized_operation_message, op_message->second_argument);

        printf("Descriptors:\n");
        for (int j = 0; j < MAX_CLIENTS_NUMBER; ++j) {
            printf("%d ", client_descriptors[j]);
        }

        int cl_desc = 0;

        while (cl_desc == 0) {
            cl_desc = client_descriptors[rand() % MAX_CLIENTS_NUMBER];
        }

        printf("Client descriptor: %d\n", cl_desc);

        write(cl_desc, serialized_operation_message, strlen(serialized_operation_message));

        check_doubles(client_descriptors, MAX_CLIENTS_NUMBER);

    }
}

void parse_args(int argc, char ** argv){
    arg1_addr_port = malloc(20 * sizeof(char));
    arg2_unix_path = malloc(20 * sizeof(char));
    strcpy(arg1_addr_port, argv[1]);
    strcpy(arg2_unix_path, argv[2]);
}

void call_error(char * message){
    printf("%s\n", message);
    exit(1);
}

int string_array_contains(char *array[], int array_length, char *string){
    for (int i = 0; i < array_length; ++i) {
        if (strcmp(array[i], string) == 0) {
            return 1;
        }
    }
    return 0;
}

void check_doubles(int * arr, int len){
    printf("\nBefore:");
    for (int k = 0; k < len; ++k) {
        printf("%d ", arr[k]);
    }
    if (len == 1) {
        return;
    } else {
        for (int i = 0; i < len; ++i) {
            for (int j = i + 1; j < len; ++j) {
                if (arr[i] == arr[j]) {
                    arr[j] = 0;
                }
            }
        }
    }

    printf("\nAfter:");
    for (int k = 0; k < len; ++k) {
        printf("%d ", arr[k]);
    }
}

