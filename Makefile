# COMP30023 assignment1
# written by Tian Luan, April 2017
#

## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS =	-Wall -Wextra -std=gnu99


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		swap.c utilities.c list.c queue.c
OBJ =		swap.o utilities.o list.o queue.o
EXE = 		swap

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies

swap.o: swap.h utilities.h list.h queue.h
utilities.o: utilities.h list.h
list.o: list.h utilities.h
queue.o: queue.h
