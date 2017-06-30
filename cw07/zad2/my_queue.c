#include <stdlib.h>
#include <stdio.h>
#include "my_queue.h"

void my_queue_init(my_queue *queue, int cn){
    queue->max = cn;
    queue->head = -1;
    queue->tail = 0;
    queue->chair = 0;
}

int my_queue_is_empty(my_queue *queue){
    if(queue->head == -1) return 1;
    else return 0;
}

int my_queue_is_full(my_queue *queue){
    if(queue->head == queue->tail) return 1;
    else return 0;
}

pid_t my_queue_pop(my_queue *queue){
    if(my_queue_is_empty(queue) == 1) return -1;

    queue->chair = queue->tab[queue->head++];
    if(queue->head == queue->max) queue->head = 0;

    if(queue->head == queue->tail) queue->head = -1;

    return queue->chair;
}

int my_queue_push(my_queue *queue, pid_t x){
    if(my_queue_is_full(queue) == 1) return -1;
    if(my_queue_is_empty(queue) == 1){
        queue->head = queue->tail;
    }

    queue->tab[queue->tail++] = x;

    if(queue->tail == queue->max) queue->tail = 0;

    return 0;
}


void print_error(const char *message){
    printf("Error! %s\n", message);
    exit(3);
}
