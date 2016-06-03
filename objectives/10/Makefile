#----------------------------------------------------------------------
# CS415/515 Assignment 3
#----------------------------------------------------------------------

FLAGS = -std=c99 -g -lm
CC = mpicc

all: extsort datagen verify qsort

extsort: extsort.c
	$(CC) $(FLAGS) -o extsort extsort.c

datagen: datagen.c
	$(CC) $(FLAGS) -o datagen datagen.c

verify: verify.c
	$(CC) $(FLAGS) -o verify verify.c

qsort: qsort.c
	$(CC) $(FLAGS) -o qsort qsort.c

%: %.c
	$(CC) $(FLAGS) -o $@ $<

clean:
	rm -f extsort datagen verify qsort
