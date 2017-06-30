#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define BARBER 0
#define QUEUE 1
#define CHECKER 2

void print_error(const char *message);

typedef struct my_queue{
  int max;
  int head;
  int tail;
  pid_t chair;
  pid_t tab[1000];
} myqueue;

void my_queue_init(myqueue *myqueue, int len);
pid_t my_queue_pop(myqueue *myqueue);
int my_queue_push(myqueue *myqueue, pid_t pid);
int my_queue_is_empty(myqueue *myqueue);
int my_queue_is_full(myqueue *myqueue);

#endif
