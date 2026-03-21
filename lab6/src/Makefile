CC=gcc
CFLAGS=-I.
LDFLAGS=-lpthread

all: client server

client: client.c
	$(CC) $(CFLAGS) client.c -o client $(LDFLAGS)

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LDFLAGS)

clean:
	rm -f client server
