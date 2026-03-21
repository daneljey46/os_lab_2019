CC=gcc
CFLAGS=-I. -I../../lab3/src
LAB3_SRC=../../lab3/src

all: parallel_sum

libsum.a: sum_lib.o
	ar rcs libsum.a sum_lib.o

sum_lib.o: sum_lib.c sum_lib.h
	$(CC) -c sum_lib.c $(CFLAGS)

parallel_sum: parallel_sum.c utils.o libsum.a
	$(CC) -o parallel_sum parallel_sum.c utils.o -L. -lsum -lpthread $(CFLAGS)

utils.o: $(LAB3_SRC)/utils.c $(LAB3_SRC)/utils.h
	$(CC) -c $(LAB3_SRC)/utils.c $(CFLAGS) -o utils.o

clean:
	rm -f *.o *.a parallel_sum