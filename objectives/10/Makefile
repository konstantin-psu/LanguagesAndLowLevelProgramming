#----------------------------------------------------------------------
# CS415/515 Assignment 3
#----------------------------------------------------------------------

FLAGS = -std=c99 -g -lm
CC = mpicc
CHPL = chpl
SRCDIR=laplace-distrCompiledSourceCode

all: laplace-distr laplace-mpi

laplace-distr: laplace-distr.chpl
	mkdir -p $(SRCDIR)
	$(CHPL) -o laplace-distr --codegen laplace-distr.chpl --savec $(SRCDIR)

laplace-mpi: laplace-mpi.c
	$(CC) $(FLAGS) -o laplace-mpi laplace-mpi.c

mpirun: laplace-mpi
	mpiruc -n 4 ./laplace-mpi 	

chaplrun: laplace-distr
	./laplace-distr -nl 2

clean:
	rm -rf *_real  laplace-distr $(SRCDIR) laplace-mpi
