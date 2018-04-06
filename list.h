/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

typedef struct process process_t;

struct process {
    int crt_time;
    int pid;
    int size;
    int job_time;
    int arrv_time;
    int hole;
    process_t *next;
};

typedef struct {
	process_t *head;
	process_t *foot;
    int num;      // the number of processes
    int capacity; // max size
} list_t;

process_t *new_process(int crt_time, int pid, int size, int job_time);
list_t *new_list();
void free_list(list_t *list);
void insert_at_head(list_t *list, process_t *new);
void insert_at_foot(list_t *list, process_t *new);
process_t *remove_at_head(list_t *list);
process_t *get_longest(list_t *list, int clock);
process_t *remove_one(list_t *list, process_t *one, bool to_update_hole);
process_t *get_one(list_t *list, int pid);

#endif
