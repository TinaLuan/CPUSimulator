/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

#include "swap.h"
#include "utilities.h"
#include "queue.h"
#include "list.h"

int
main(int argc, char *argv[]) {
    // Read arguments from command line
    char filename[FILE_NAME_LEN];
    algorithm_t algorithm;
    int mem_capacity = 0;
    int quantum = 0;
    read_command(argc, argv, filename, &algorithm, &mem_capacity, &quantum);

    // Prepare related data structures
    list_t *load_list = new_list();
    store_all_processes(filename, load_list);
    list_t *disk = new_list();
    list_t *memory = new_list();
    memory->capacity = mem_capacity;
    // the maximum possible size should be all num of all processes
    queue_t *queue = new_queue(load_list->num);

    int used_quantum = 0;
    int clock = 0;
    bool terminate = false;

    // Simulation starts
    while (!terminate) {
        // when it is the creation time, load the process(es) to disk
        while (load_list->num != 0 && clock == load_list->head->crt_time) {
            process_t *new = remove_at_head(load_list);
            new->arrv_time = clock;
            insert_at_foot(disk, new);
        }

        // event 1
        if (memory->num == 0 && disk->num != 0) {
            event_1(disk, memory, queue, clock, algorithm);
        }

        int running_id = -1;
        process_t *running = NULL;
        if (queue->num != 0) {
            running_id = queue->pid_arr[0];
            running = get_one(memory, running_id);
        }

        // event 3
        if (running && running->job_time == 0) {
            event_3(disk, memory, queue, clock, &running_id,
                &used_quantum, algorithm);
        }

        // event 2
        if (used_quantum == quantum) {
            event_2(disk, memory, queue, clock, &running_id,
                &used_quantum, algorithm);
        }

        // Definitely put this block after the three event
        if (memory->num == 0 && disk->num == 0 && load_list->num == 0 ) {
            terminate = true;
            fprintf(stdout, "time %d, simulation finished.\n", clock);
            return 0;
        }

        running = get_one(memory, running_id);
        if (running) {
            used_quantum++;
            running->job_time--;
        }

        clock++;
    }

    return 0;
}

void
event_1(list_t *disk, list_t *memory, queue_t *queue, int clock,
    algorithm_t algorithm) {

    int new_pid = swap(disk, memory, clock, queue, algorithm);
    // no running id, so the last argument is -1
    schedule(queue, new_pid, false, -1);
    output_line(clock, new_pid, memory);
}

void
event_2(list_t *disk, list_t *memory, queue_t *queue, int clock,
    int *running_id, int *used_quantum, algorithm_t algorithm) {

        int new_pid = swap(disk, memory, clock, queue, algorithm);
        schedule(queue, new_pid, false, *running_id);
        *used_quantum = 0;
        *running_id = queue->pid_arr[0];
        output_line(clock, new_pid, memory);
}

/* Process finishes */
void
event_3(list_t *disk, list_t *memory, queue_t *queue, int clock,
    int *running_id, int *used_quantum, algorithm_t algorithm) {

    // Remove the process from memory
    bool to_update_hole = true;
    process_t *running = get_one(memory, *running_id);
    remove_one(memory, running, to_update_hole);
    free(running);
    running = NULL;

    // Swap and Schedule
    int new_pid = swap(disk, memory, clock, queue, algorithm);
    schedule(queue, new_pid, true, *running_id);
    *used_quantum = 0;
    *running_id = queue->pid_arr[0];
    output_line(clock, new_pid, memory);
}

/* Swap in process from disk to memory */
int
swap(list_t *disk, list_t *memory, int clock, queue_t *queue,
    algorithm_t algorithm) {

    // Apply fit function
    bool(*fit_function)(list_t *, process_t *);
    switch (algorithm) {
        case FIRST:
            fit_function = &first_fit;
            break;
        case BEST:
            fit_function = &best_fit;
            break;
        case WORST:
            fit_function = &worst_fit;
            break;
        default:
            break;
    }

    process_t *longest = get_longest(disk, clock);
    // Remove the process which waited for the longest time in disk
    if (longest) {
        remove_one(disk, longest, 0);
        longest->arrv_time = clock;
        longest->hole = 0;

        // Put the process into memory with a particular fit function
        while ((*fit_function)(memory, longest) == 0){
            int out_pid = swap_out(memory, disk, clock);
            delete_one(queue, out_pid);
        }
        return longest->pid;
    }
    // Disk is empty, nothing got swapped in
    return -1;
}

/* Swap out process(es) from memory to disk */
int
swap_out(list_t *memory, list_t *disk, int clock) {
    process_t *longest = get_longest(memory, clock);
    remove_one(memory, longest, 1);
    longest->arrv_time = clock;
    longest->hole = 0;
    insert_at_foot(disk, longest);
    return longest->pid;
}

/* Schedule the round robin queue */
void
schedule(queue_t *queue, int new_pid, int to_clear_dead, int running_id) {
    // new swapped-in process will be schedule in the RRQ
    if (new_pid >= 0) enqueue(queue, new_pid);

    // Put the process which was running but just expired to the end
    // The process which has been finished will disappear in the queue
    if (running_id == queue->pid_arr[0]) {
        int expired_pid = dequeue(queue);
        if (!to_clear_dead)
            enqueue(queue, expired_pid);
    }
}

/* Find the first (with the highest address) hole to fit in the new process */
bool
first_fit(list_t *memory, process_t *new) {
    process_t *curr = memory->head, *prev = NULL;
    int usage = 0;
    int holes_except_last = 0;
    // Check if any suitable hole exits before each process
    // Pick up the first hole and insert the process at the hole
    while (curr) {
        if (curr->hole >= new->size) {
            if (curr == memory->head) {
                insert_at_head(memory, new);
            } else {
                new->next = curr;
                prev->next = new;
                memory->num++;
            }
            // update the hole
            new->next->hole -= new->size;
            return true;
        }
        usage += curr->size;
        holes_except_last += curr->hole;
        prev = curr;
        curr = curr->next;
    }
    // no suitable hole has been found
    // check if a hole exits in the end
    int last_hole = memory->capacity - usage - holes_except_last;
    if (last_hole >= new->size) {
        //left = memory->foot;
        insert_at_foot(memory, new);
        // no need to update the last hole, cuz the last hole is always gonna be calculated when needed
        return true;
    }
    return false;
}

/* Find the smallest suitable hole to fit the new process */
bool
best_fit(list_t *memory, process_t *new) {
    process_t *curr = memory->head, *prev = NULL, *left = NULL, *right = NULL;
    int smallest = memory->capacity + 1;
    int usage = 0;
    int holes_except_last = 0;
    bool found = false;

    // Find the smallest suitable hole (except the last hole)
    while (curr) {
        if (curr->hole >= new->size && curr->hole < smallest) {
            found = true;
            left = prev;
            right = curr;
            smallest = curr->hole;
        }
        usage += curr->size;
        holes_except_last += curr->hole;
        prev = curr;
        curr = curr->next;
    }
    // check if the last hole is more suitable
    int last_hole = memory->capacity - usage - holes_except_last;
    if (last_hole >= new->size && last_hole < smallest) {
        insert_at_foot(memory, new);
        // no need to update the last hole
        // cuz the last hole is always gonna be calculated when needed
        return true;
    }
    // Insert the new process into the best fit position
    if (found) {
        if (memory->head->hole == smallest) {
            insert_at_head(memory, new);
        } else {
            new->next = right;
            left->next = new;
            memory->num++;
        }
        new->next->hole -= new->size;
    }
    return found;
}

/* Find the biggest suitable hole to fit the new process */
bool
worst_fit(list_t *memory, process_t *new) {
    process_t *curr = memory->head, *prev = NULL, *left = NULL, *right = NULL;
    int biggest = -1, usage = 0, holes_except_last = 0;
    bool found = false;

    // Find the biggest suitable hole (expect the last hole)
    while (curr) {
        if (curr->hole >= new->size && curr->hole > biggest) {
            found = true;
            left = prev;
            right = curr;
            biggest = curr->hole;
        }
        usage += curr->size;
        holes_except_last += curr->hole;
        prev = curr;
        curr = curr->next;
    }
    // check if the last hole is more suitable
    int last_hole = memory->capacity - usage - holes_except_last;
    if (last_hole >= new->size && last_hole > biggest) {
        insert_at_foot(memory, new);
        // No need to update the last hole
        // cuz the last hole is always gonna be calculated when needed
        return true;
    }
    // Insert the new process into the best fit position
    if (found) {
        if (memory->head->hole == biggest) {
            insert_at_head(memory, new);
        } else {
            new->next = right;
            left->next = new;
            memory->num++;
        }
        new->next->hole -= new->size;
    }
    return found;
}

/* Read input file and initially create prosesses to load_list */
void
store_all_processes(char *filename, list_t *load_list) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Fail to open input file\n");
        exit(EXIT_FAILURE);
    }
    char line[LN_SIZE];
    char **tuple;
    process_t *new = NULL;

    // Read line by line and create prosesses
    while(fgets(line, LN_SIZE, fp)) {
        // trim the last '\n'
        line[strlen(line)-1] = '\0';
        tuple = tokenizer(line);

        // Pass the arguments and Create all processes to load list
        new = new_process(atoi(tuple[0]), atoi(tuple[1]),
        atoi(tuple[2]), atoi(tuple[3]));
        insert_at_foot(load_list, new);

    }
    fclose(fp);
}
