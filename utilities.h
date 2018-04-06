/* COMP30023 assignment1
 * written by Tian Luan, April 2017
 */
#ifndef UTILITIES_H
#define UTILITIES_H

#include "list.h"
#include "swap.h"

#define USAGE     "Usage: -f <name> -a <algorithm> -m <size> -q <quantum>\n"
#define FIRST_FIT "first"
#define BEST_FIT  "best"
#define WORST_FIT "worst"

/* Extern declarations: */
extern  int     optind;
extern  char    *optarg;

// These three are given in workshops
char **tokenizer(char *str);
int str_char_count(char *str, char cmp);
int m_strcpy_slice(char *dst, char *src, int n);

void read_command(int argc, char *argv[], char *filename,algorithm_t *algorithm,
    int *mem_capacity, int *quantum);
void output_line(int clock, int new_pid, list_t *memory);

#endif
