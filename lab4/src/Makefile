CC=gcc
CFLAGS=-I. -I../../lab3/src

LAB3_SRC=/workspaces/os_lab_2019/lab3/src

all : parallel_min_max process_memory sequential_min_max run_seq zombie_demo

parallel_min_max : utils.o find_min_max.o
	$(CC) -o parallel_min_max utils.o find_min_max.o $(LAB3_SRC)/parallel_min_max.c $(CFLAGS)

sequential_min_max : utils.o find_min_max.o
	$(CC) -o sequential_min_max utils.o find_min_max.o $(LAB3_SRC)/sequential_min_max.c $(CFLAGS)

run_seq : $(LAB3_SRC)/run_sequential.c
	$(CC) -o run_seq $(LAB3_SRC)/run_sequential.c $(CFLAGS)

zombie_demo : zombie_demo.c
	$(CC) -o zombie_demo $(LAB3_SRC)/zombie_demo.c $(CFLAGS)

process_memory : process_memory.c
	$(CC) -o process_memory $(LAB3_SRC)/process_memory.c $(CFLAGS)

utils.o : $(LAB3_SRC)/utils.c $(LAB3_SRC)/utils.h
	$(CC) -c $(LAB3_SRC)/utils.c $(CFLAGS) -o utils.o

find_min_max.o : $(LAB3_SRC)/find_min_max.c $(LAB3_SRC)/find_min_max.h
	$(CC) -c $(LAB3_SRC)/find_min_max.c $(CFLAGS) -o find_min_max.o

clean :
	rm -f *.o parallel_min_max sequential_min_max run_seq zombie_demo process_memory