CC=gcc
CFLAGS=-I.
LDFLAGS=-lpthread

all: client server

libsquare.o: libsquare.c libsquare.h
	$(CC) $(CFLAGS) -c libsquare.c -o libsquare.o

client: client.c libsquare.o
	$(CC) $(CFLAGS) client.c libsquare.o -o client $(LDFLAGS)

server: server.c libsquare.o
	$(CC) $(CFLAGS) server.c libsquare.o -o server $(LDFLAGS)

clean:
	rm -f client server libsquare.o