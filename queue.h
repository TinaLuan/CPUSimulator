/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int *pid_arr;
    int num;
} queue_t;

queue_t *new_queue(int size);
void enqueue(queue_t *queque, int pid);
int dequeue(queue_t *queque);
void delete_one(queue_t *queue, int out_pid);

#endif
