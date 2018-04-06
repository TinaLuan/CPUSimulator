/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

/* Create a new queue */
queue_t *
new_queue(int size) {
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    assert(queue);
    queue->pid_arr = (int *)malloc(sizeof(int) * size);
    assert(queue->pid_arr);
    queue->num = 0;
    return queue;
}

/* Put the new item in the queue */
void
enqueue(queue_t *queue, int pid) {
    // It's impossible to be full.
    // Since the max size is the num of all processes
    queue->pid_arr[queue->num++] = pid;
}

/* Get the first item in the queue and remove it */
int
dequeue(queue_t *queue) {
    if (queue->num > 0) {
        int result = queue->pid_arr[0];
        int i;
        for (i = 1; i < queue->num; i++) {
            queue->pid_arr[i-1] = queue->pid_arr[i];
        }
        queue->num--;
        return result;
    }
    return -1;
}

/* Remove pid from queue */
void
delete_one(queue_t *queue, int out_pid) {
    if (queue->num > 0) {
        int i, j;
        for (i = 0; i < queue->num; i++) {
            if (queue->pid_arr[i] == out_pid) {
                for (j = i+1; j < queue->num; j++){
                    queue->pid_arr[j-1] = queue->pid_arr[j];
                }
                queue->num--;
                break;
            }
        }
    }
}
