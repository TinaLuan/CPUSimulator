#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "utilities.h"
#include "process.h"

#define LN_SIZE 30 // right?

/* Extern declarations: */
extern  int     optind;
extern  char    *optarg;

typedef struct {
    int *pid_arr;
    int num;
} queue_t;



void read_command(int argc, char *argv[], char *filename, char *algorithm,
    int *mem_capacity, int *quantum);
void store_all_processes(char *filename, list_t *load_list);
void event_3(list_t *disk, list_t *memory, queue_t *queque, int clock,
    int *running_id, int *used_quantum);
int swap(list_t *disk, list_t *memory, int clock, queue_t *queue);
void schedule(queue_t *queque, int new_pid, int to_clear_dead, int running_id);
int swap_out(list_t *memory, list_t *disk, int clock);
queue_t *new_queque(int size);
void enqueque(queue_t *queque, int pid);
int dequeque(queue_t *queque);
void output_line(int clock, int new_pid, list_t *memory);

bool first_fit(list_t *memory, process_t *new);
bool best_fit(list_t *memory, process_t *new);
bool worst_fit(list_t *memory, process_t *new);

void status(int clock, list_t *memory, list_t *disk, queue_t *queue, int running_id);

int
main(int argc, char *argv[]) {
    char filename[20];
    char algorithm[10];
    int mem_capacity = 0;
    int quantum = 0;
    read_command(argc, argv, filename, algorithm, &mem_capacity, &quantum);

    list_t *load_list = new_list();
    store_all_processes(filename, load_list);

    list_t *disk = new_list();
    list_t *memory = new_list();
    memory->capacity = mem_capacity;

    queue_t *queue = new_queque(load_list->num);
    // the maximum possible size should be all num of all processes

    int used_quantum = 0;
    int clock = 0;
    bool terminate = false;

    while (!terminate) {
    //while (clock <= 117) {

        // when it is the creation time, load the process(es) to disk
        while (load_list->num != 0 && clock == load_list->head->crt_time) {
            process_t *new = remove_at_head(load_list);
            new->arrv_time = clock;
            insert_at_foot(disk, new);
        }

        // event 1
        if (memory->num == 0 && disk->num != 0) {
            int new_pid = swap(disk, memory, clock, queue);
            schedule(queue, new_pid, 0, -1); // deal with -1
            output_line(clock, new_pid, memory);
        }

        // should be in function
        int running_id = queue->pid_arr[0];
        process_t *running = get_one(memory, running_id);

        // event 3
        if (running->job_time == 0) {
            remove_one(memory, running, 1);

            int new_pid = swap(disk, memory, clock, queue);
            free(running);
            running = NULL;

            schedule(queue, new_pid, 1, running_id);
            //queue->num--;
            used_quantum = 0;
            running_id = queue->pid_arr[0];
            running = get_one(memory, running_id);
            output_line(clock, new_pid, memory);
        }

        // event 2
        if (used_quantum == quantum) {
            int new_pid = swap(disk, memory, clock, queue);
            schedule(queue, new_pid, 0, running_id);
            used_quantum = 0;
            running_id = queue->pid_arr[0];
            running = get_one(memory, running_id);
            output_line(clock, new_pid, memory);
        }

        // the condition is correct???
        // Definitely put this block after the three event
        if (memory->num == 0 && disk->num == 0 && load_list->num == 0 ) {
            terminate = true;
            fprintf(stdout, "time %d, simulation finished.\n", clock);
        }

        //status(clock, memory, disk, queue, running_id);


        clock++;
        used_quantum++;

        if (running)
            running->job_time--;
    }

    //free(alorithm);
    return 0;
}


void
event_3(list_t *disk, list_t *memory, queue_t *queque, int clock,
    int *running_id, int *used_quantum) {

    process_t *running = get_one(memory, running_id);
    bool to_update_hole = true;
    remove_one(memory, running, to_update_hole);
    int new_pid = swap(disk, memory, clock, queue);
    free(running);
    running = NULL;

    schedule(queue, new_pid, true, running_id);
    *used_quantum = 0;
    running_id = queue->pid_arr[0];
    //running = get_one(memory, running_id);
    output_line(clock, new_pid, memory);
}


int
swap(list_t *disk, list_t *memory, int clock, queue_t *queque) {
    process_t *longest = get_longest(disk, clock);
    // need to check whether disk was empty
    // if emtpy, longest is null and swapping won't happen
    if (longest) {
        remove_one(disk, longest, 0);
        longest->arrv_time = clock;
        longest->hole = 0;
        while (worst_fit(memory, longest) == 0){
            int out_id = swap_out(memory, disk, clock);
            // be in a func, and swap() shouldn't have arg queue
            if (queque->num > 0) {
                int i, j;
                for (i = 0; i < queque->num; i++) {
                    if (queque->pid_arr[i] == out_id) {
                        for (j = i+1; j < queque->num; j++){
                            queque->pid_arr[j-1] = queque->pid_arr[j];
                        }
                        queque->num--;
                        break;
                    }
                }
            }

        }
        return longest->pid;
    }
    return -1;
}

int
swap_out(list_t *memory, list_t *disk, int clock) {
    process_t *longest = get_longest(memory, clock);
    remove_one(memory, longest, 1);
    longest->arrv_time = clock;
    longest->hole = 0;
    insert_at_foot(disk, longest);

    return longest->pid;
}

void
schedule(queue_t *queque, int new_pid, int to_clear_dead, int running_id) {
    // new swapped-in process will be schedule in the RRQ
    if (new_pid >= 0) enqueque(queque, new_pid);

    // Put the process which was running but just expired to the end
    // The process which has been finished will disappear in the queue
    if (running_id == queque->pid_arr[0]) { // should have a peek function ?
        int expired_pid = dequeque(queque);
        if (!to_clear_dead)
            enqueque(queque, expired_pid);
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

void
read_command(int argc, char *argv[], char *filename, char *algorithm,
    int *mem_capacity, int *quantum) {

    char input;
    while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF) {
		switch ( input )
		{
			case 'f':
				if (optarg) {
                    strncpy(filename, optarg, strlen(optarg));
                    filename[strlen(optarg)+1] = '\0';
                //open(filename, "r");
                }
                else {
					// exit if optarg unknown
                    fprintf(stderr, "Usage: -f <filename> -a <algorithm>");
                    fprintf(stderr, "-m <memory size> -q <quantum>\n");
                    exit(1);
				}
 				break;
					// should not get here
            // Change????
            // if  == first || best || worst , then strcpy ();
			case 'a':
				if (strcmp(optarg, "first") == 0)
                    strcpy(algorithm, "first");
				else if (strcmp(optarg, "best") == 0)
                    strcpy(algorithm, "best");
				else if (strcmp(optarg, "worst") == 0)
                    strcpy(algorithm, "worst");
				else {
                    fprintf(stderr, "Usage: -f <filename> -a <algorithm>");
                    fprintf(stderr, "-m <memory size> -q <quantum>\n");
					exit(1);
				}
				break;

			case 'm':
				// set the value of size (int value) based on optarg
				if (optarg) *mem_capacity = atoi(optarg);
				else {
					fprintf(stderr, "Usage: -f <filename> -a <algorithm>");
                    fprintf(stderr, "-m <memory size> -q <quantum>\n");
					exit(1);
				}
				break;

			case 'q':
				if (optarg) *quantum = atoi(optarg);
				else {
                    fprintf(stderr, "Usage: -f <filename> -a <algorithm>");
                    fprintf(stderr, "-m <memory size> -q <quantum>\n");
					exit(1);

				}
				break;

			default:
				// do something
                // ?????????????????
				break;
		}
	}
}

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
    //list_t *load_list = new_list();
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
    //return load_list;
}

void
status(int clock, list_t *memory, list_t *disk, queue_t *queue, int running_id) {
    if (clock == 116) {
        printf("clock: %d\n", clock);
        printf("\nrun: %d, %d\n", running_id, get_one(memory, running_id)->job_time);

        printf("\ndisk: %d\n", disk->num);
        printf("id\thole\n");
        process_t *each = disk->head;
        while (each) {
            printf("%d\t%d\n", each->pid, each->hole);
            each = each->next;
        }

        printf("\nmemory: %d\n", memory->num);
        printf("id\thole\n");
        each = memory->head;
        while (each) {
            printf("%d\t%d\n", each->pid, each->hole);
            each = each->next;
        }

        // queue
        printf("\nquque: %d\n", queue->num);
        printf("id\tjob\n");
        for (int i = 0; i < queue->num; i++) {
            printf("%d\t%d\n", queue->pid_arr[i], get_one(memory, queue->pid_arr[i])->job_time);
        }
    }
}

void
output_line(int clock, int new_pid, list_t *memory) {
    // valid pid indicates the swapping did happen
    if (new_pid >= 0) {
        process_t *curr = memory->head;
        int num_processes = 0, usage = 0, holes_except_last = 0, num_holes = 0;
        while (curr) {
            num_processes++;
            usage += curr->size;
            holes_except_last += curr->hole;
            if (curr->hole != 0) num_holes++;
            curr = curr->next;
        }
        int last_hole = memory->capacity - usage - holes_except_last;
        if (last_hole > 0) {
            num_holes++;
        }
        fprintf(stdout, "time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%%\n",
        clock, new_pid, num_processes, num_holes, (int)((double)usage / (memory->capacity) * 100));
    }
}
