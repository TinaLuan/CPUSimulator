/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#ifndef SWAP_H
#define SWAP_H

#include "list.h"
#include "queue.h"

typedef enum algorithm{
    FIRST = 0, BEST = 1, WORST = 2
} algorithm_t;

#define LN_SIZE 100 // the max number of chars per line of the input file
#define FILE_NAME_LEN 20

/* Function Prototypes */
void store_all_processes(char *filename, list_t *load_list);

void event_1(list_t *disk, list_t *memory, queue_t *queue, int clock,
    algorithm_t algorithm);
void event_2(list_t *disk, list_t *memory, queue_t *queue, int clock,
    int *running_id, int *used_quantum, algorithm_t algorithm);
void event_3(list_t *disk, list_t *memory, queue_t *queue, int clock,
    int *running_id, int *used_quantum, algorithm_t algorithm);

int swap(list_t *disk, list_t *memory, int clock, queue_t *queue,
    algorithm_t algorithm);
void schedule(queue_t *queque, int new_pid, int to_clear_dead, int running_id);
int swap_out(list_t *memory, list_t *disk, int clock);

bool first_fit(list_t *memory, process_t *new);
bool best_fit(list_t *memory, process_t *new);
bool worst_fit(list_t *memory, process_t *new);

#endif
