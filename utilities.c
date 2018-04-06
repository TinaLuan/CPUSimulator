/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include "utilities.h"
#include "swap.h"

/* Given in the workshop
 * Tokenize the input string to an array of strings
 */
char **tokenizer(char *str){
  char *cur = str;
  char delimiter = ' ';
  int num_tokens = str_char_count(str, delimiter);
  int cur_num = 0;
  char **str_array = malloc((size_t)sizeof(char*) * (size_t)(num_tokens+2));
  assert(str_array);
  char *prev = str;
  while (*cur) {
    if (!*(cur+1)) {
      cur++;
      *cur = delimiter;
    }
    if (*cur == delimiter) {
      char *len = (char*)(cur-prev);
      char *token =  malloc(sizeof(char) * (size_t)(len+1));
      assert(token);
      m_strcpy_slice(token, prev, (int) len);
      str_array[cur_num] = token;
      prev = cur;
      prev++;
      cur_num++;
    }
    cur++;
  }
  str_array[num_tokens+1] = NULL;
  return str_array;
}

/* Given in the workshop */
int str_char_count(char *str, char cmp) {
  int count = 0;
  while (*str != '\0') {
    if (*str++ == cmp) {
      count++;
    }
  }
  return count;
}

/* Given in the workshop */
int m_strcpy_slice(char *dst, char *src, int n) {
    int i = 0;
    for (i = 0; i < n; i++) {
        if ((*dst++ = *src++) == '\0') {
          return -1;
        }
    }
    *dst = '\0';
    return 0;
}

/* Modified from the given code in lms
 * Read the command line and set up
 */
void
read_command(int argc, char *argv[], char *filename, algorithm_t *algorithm,
    int *mem_capacity, int *quantum) {

    char input;
    while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF) {
		switch ( input )
		{
			case 'f':
				if (optarg) {
                    strncpy(filename, optarg, strlen(optarg));
                    filename[strlen(optarg)] = '\0';
                }
                else {
					// exit if optarg unknown
                    fprintf(stderr, USAGE);
                    exit(1);
				}
 				break;
					// should not get here

			case 'a':
				if (strcmp(optarg, FIRST_FIT) == 0)
                    *algorithm = FIRST;
				else if (strcmp(optarg, BEST_FIT) == 0)
                    *algorithm = BEST;
				else if (strcmp(optarg, WORST_FIT) == 0)
                    *algorithm = WORST;
				else {
                    // exit if optarg unknown
                    fprintf(stderr, USAGE);
					exit(1);
				}
				break;

			case 'm':
				// set the value of size (int value) based on optarg
				if (optarg) *mem_capacity = atoi(optarg);
				else {
                    // exit if optarg unknown
                    fprintf(stderr, USAGE);
					exit(1);
				}
				break;

			case 'q':
				if (optarg) *quantum = atoi(optarg);
				else {
                    // exit if optarg unknown
                    fprintf(stderr, USAGE);
					exit(1);
				}
				break;

			default:

				break;
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
        fprintf(stdout,
        "time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%%\n",
        clock, new_pid, num_processes, num_holes,
        (int)(ceil((double)usage * 100 / memory->capacity)) );
    }
}
