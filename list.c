/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "list.h"

/* Create a new process */
process_t *
new_process(int crt_time, int pid, int size, int job_time) {
    process_t *new = (process_t *)malloc(sizeof(process_t));
    assert(new);
    new->crt_time = crt_time;
    new->pid = pid;
    new->size = size;
    new->job_time = job_time;
    new->arrv_time = crt_time;
    new->hole = 0;
    new->next = NULL;
    return new;
}


/* Create a new linked list */
list_t *
new_list() {
	list_t *list = (list_t*)malloc(sizeof(list_t));
	assert(list);
	list->head = NULL;
    list->foot = NULL;
    list->num = 0;
	return list;
}

/* From Foundation of algorithm lecture slides */
void
free_list(list_t *list) {
	process_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

/* Modified from Foundation of Algorithm lecture slides */
void
insert_at_head(list_t *list, process_t *new) {
	new->next = list->head;
	list->head = new;
	if (list->foot==NULL) {
		// this is the first insertion into the list
		list->foot = new;
	}
    list->num++;
}

/* Modified from Foundation of Algorithm lecture slides */
void
insert_at_foot(list_t *list, process_t *new) {
	new->next = NULL;
	if (list->foot==NULL) {
		// this is the first insertion into the list
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
    list->num++;
}


/* Remove the first process in linked list */
process_t *
remove_at_head(list_t *list) {
    process_t *old_head = list->head;
    // not empty
    if (list->num != 0) {
        // num > 1
        if (old_head->next != NULL) {
            list->head = old_head->next;
        // has only one node
        } else {
            list->head = list->foot = NULL;
        }
        list->num--;
    }
    return old_head;
}

/* Find the process which waited for the longes time */
process_t *
get_longest(list_t *list, int clock) {
    int longest_time = -1;
    process_t *result = NULL;
    process_t *curr = list->head;

    // Go throught the list
    // Find out the process with the longest waiting time and the smallest pid
    while (curr) {
        int curr_wait_time = clock - curr->arrv_time;
        // Found a longer waiting time, update result
        if (curr_wait_time > longest_time) {
            longest_time = curr_wait_time;
            result = curr;

        // Found an equal waiting time, pick up the one with smallest pid
        } else if (curr_wait_time == longest_time) {
            if (curr->pid < result->pid) {
                result = curr;
            }
        }
        curr = curr->next;
    }
    return result;
}

/* Remove the given process (in memory, update the hole)*/
process_t *
remove_one(list_t *list, process_t *one, bool to_update_hole) {
    if (one == list->head) {
        one = remove_at_head(list);
        if (to_update_hole) {
            if (one->next) one->next->hole += (one->size + one->hole);
        }
        return one;
    }
    // result is not the head
    process_t *prev = list->head;
    while (prev->next != NULL) {
        // found the result
        if (prev->next == one) {
            prev->next = one->next;
            // result is the foot
            if (one->next == NULL) {
                list->foot = prev;
            }
            list->num--;
            if (to_update_hole) {
                if (one->next) one->next->hole += (one->size + one->hole);
            }
            return one;
        // keep going
        } else {
            prev = prev->next;
        }
    }
    // Found nothing
    return NULL;
}

/* Find a process according to its pid */
process_t *
get_one(list_t *list, int pid) {
    process_t *curr = list->head;
    while (curr) {
        if (curr->pid == pid) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
