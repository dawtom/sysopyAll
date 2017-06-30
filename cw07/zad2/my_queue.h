#ifndef QUEUE_H
#define QUEUE_H

typedef struct My_queue{
  int max;
  int head;
  int tail;
  pid_t chair;
  pid_t tab[1000];
} my_queue;

void my_queue_init(my_queue *queue, int cn);
pid_t my_queue_pop(my_queue *queue);
int my_queue_push(my_queue *queue, pid_t x);
int my_queue_is_empty(my_queue *queue);
int my_queue_is_full(my_queue *queue);
void print_error(const char *err);

#endif
