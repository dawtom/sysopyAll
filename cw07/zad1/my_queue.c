#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "my_queue.h"

void my_queue_init(myqueue *myqueue, int len){
  myqueue->max = len;
  myqueue->head = -1;
  myqueue->tail = 0;
  myqueue->chair = 0;
}

int my_queue_is_empty(myqueue *myqueue){
  if(myqueue->head == -1) return 1;
  else return 0;
}

int my_queue_is_full(myqueue *myqueue){
  if(myqueue->head == myqueue->tail) return 1;
  else return 0;
}

pid_t my_queue_pop(myqueue *myqueue){
  if(my_queue_is_empty(myqueue) == 1) return -1;

  myqueue->chair = myqueue->tab[myqueue->head++];
  if(myqueue->head == myqueue->max) myqueue->head = 0;

  if(myqueue->head == myqueue->tail) myqueue->head = -1;

  return myqueue->chair;
}

int my_queue_push(myqueue *myqueue, pid_t pid){
  if(my_queue_is_full(myqueue) == 1) return -1;
  if(my_queue_is_empty(myqueue) == 1){
    myqueue->head = myqueue->tail;
  }
  myqueue->tab[myqueue->tail++] = pid;
  if(myqueue->tail == myqueue->max) myqueue->tail = 0;
  return 0;
}

void print_error(const char *message){
  printf("Error! %s\n", message);
  exit(3);
}