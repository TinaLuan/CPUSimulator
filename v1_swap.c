#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "process.h"

#define LN_SIZE 30 // right?

typedef struct {
    int *pid_arr;
    int num;
} queue_t;



list_t *read_command(int argc, char *argv[]);
int swap(list_t *disk, list_t *memory, int clock);
void schedule(queue_t *queque, int new_pid);
queue_t *new_queque(int size);
void enqueque(queue_t *queque, int pid);
int dequeque(queue_t *queque);

void first_fit(list_t *memory, process_t *left, process_t *right, process_t *new);

int
main(int argc, char *argv[]) {

    int clock = 0;
    int quantum = 0;

    list_t *load_list = read_command(argc, argv);
    list_t *disk = new_list();
    list_t *memory = new_list();
    queue_t *queue = new_queque(load_list->num);
    // the maximum possible size should be all num of all processes

    int mem_capacity = 1000;
    memory->capacity = mem_capacity;
    int terminate = 0;
    //int event_1 = 0, event_2 = 0, event_3 = 0;
    //while (!terminate) {
    while (clock <= 35) {

        // when it is the creation time, load the process to disk
        if (load_list->num != 0 && clock == load_list->head->crt_time) {
            process_t *new = remove_at_head(load_list);
            new->arrv_time = clock;
            insert_at_foot(disk, new);
            //if (memory->num == 0) event_1 = 1;
        }
        //printf("c: %d  p:%d\n", clock, disk->foot->crt_time);

        // event 1
        if (memory->num == 0) {
            int new_pid = swap(disk, memory, clock);
            schedule(queue, new_pid);
            //printf("c: %d", clock);//, disk->foot->crt_time);
            //event_1 = 0;
        }
        // event 2
        if (quantum == 7) {
            //printf("c: %d  q:%d\n", clock, quantum);
            int new_pid = swap(disk, memory, clock);
            schedule(queue, new_pid);
            quantum = 0;
        }

        // event 3
        // if () {
        //
        // }

        clock++;
        quantum++;
    }

    return 0;
}

int
swap(list_t *disk, list_t *memory, int clock) {
    process_t *longest = pop_longest(disk, clock);
    longest->arrv_time = clock;

// need to check and change
/* use left and right:
can have insertion funtion for all three algo
but checking if fit is finished, if swap_out is need
needs too many flags in while condition

use: int , every fit function returns an int to indicate fit is finished or not
while condition can be less,
but in every fit function, need to insert the new (which is the same procedure in every fit func)
*/
    process_t *left = NULL, *right = NULL;
    first_fit(memory, &left, &right, longest);
    // if before new, memory is empty, new should be fit in now,
    // mem num == 1
    // so if memory->num > 1: the new process was not the only one
    while (left == NULL && right == NULL && memory->num > 1) {
        //swap_out();
        first_fit(memory, left, right, longest);
    }
    //insert_at_foot(memory, longest);
    //memory->usage += longest->size;

    // update the hole
    // if (longest != memory->foot) {
    //     longest->next->hole -= longest->size;
    // }

    return longest->pid;
    //printf("C: %d  %d\n", clock, memory->head->crt_time);
}

void
schedule(queue_t *queque, int new_pid) {
    // new swapped-in process will be schedule in the RRQ
    enqueque(queque, new_pid);

    // put the process just expired to the end of the queue
    int expired_pid = dequeque(queque);

    enqueque(queque, expired_pid);
}

void
first_fit(list_t *memory, process_t **left, process_t **right, process_t *new) {
    process_t *curr = memory->head, *prev = NULL;
    int usage = 0;
    int holes_except_last = 0;
    // check if any suitable hole exits before each process
    // Pick up the first hole and insert the process at the hole
    while (curr) {
        if (curr->hole >= new->size) {
            new->next = curr;
            right = curr;
            // newly swapped-in process will be the first process
            if (prev == NULL) {
                left = NULL;
            // newly swapped-in process will not be the first
            } else {
                prev->next = new;
                left = prev;
            }
            new->next->hole -= new->size;
            memory->num++;
            return;
            // break; ???
        }
        usage += curr->size;
        holes_except_last += curr->hole;
        prev = curr;
        curr = curr->next;
    }
    // no suitable hole has been found
    // check if a hole exits in the end
    if (right == NULL) {
        int last_hole = memory->capacity - usage - holes_except_last;
        if (last_hole >= new->size) {
            left = memory->foot;
            insert_at_foot(memory, new);
        }
    }
}

queue_t *
new_queque(int size) {
    queue_t *queque = (queue_t *)malloc(sizeof(queue_t));
    queque->pid_arr = (int *)malloc(sizeof(int) * size);
    queque->num = 0;
    return queque;
}

void
enqueque(queue_t *queque, int pid) {
    // check if full, bring in a new variabl called max_num
    //if ()
    queque->pid_arr[queque->num++] = pid;
}

int
dequeque(queue_t *queque) {
    if (queque->num > 0) {
        int result = queque->pid_arr[0];
        int i;
        for (i = 1; i < queque->num; i++) {
            queque->pid_arr[i-1] = queque->pid_arr[i];
        }
        queque->num--;
        return result;
    }
    return -1;
}

list_t *
read_command(int argc, char *argv[]) {
    // assume valid command???
    // no output file
    if (argc == 9) {

    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    char line[LN_SIZE];
    char **tuple;
    process_t *new = NULL;
    list_t *load_list = new_list();
    while(fgets(line, LN_SIZE, fp)) {
        // trim the last \n
        line[strlen(line)-1] = '\0';
        tuple = tokenizer(line);
        // can i use 0123 directly? or define? or use sizeof() to calculate
        new = new_process(atoi(tuple[0]), atoi(tuple[1]),
        atoi(tuple[2]), atoi(tuple[3]));

        insert_at_foot(load_list, new);

    }
    fclose(fp);
    return load_list;
}
