#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "libsquare.h"

int main(int argc, char **argv) {
  int tnum = -1; 
  int port = -1;

  while (true) {
    static struct option options[] = {
        {"port", required_argument, 0, 0},
        {"tnum", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0: port = atoi(optarg); break;
          case 1: tnum = atoi(optarg); break;
        }
        break;
      case '?': printf("Arguments error\n"); break;
      default: break;
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Usage: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Socket failed");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    return 1;
  }

  if (listen(server_fd, 5) < 0) {
    perror("Listen failed");
    return 1;
  }

  printf("Server listening on port %d...\n", port);

  while (true) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);

    if (client_fd < 0) {
      perror("Accept failed");
      continue;
    }

    uint64_t task[3];
    int bytes_read = recv(client_fd, task, sizeof(task), 0);
    
    if (bytes_read > 0) {
        uint64_t begin = task[0];
        uint64_t end = task[1];
        uint64_t mod = task[2];

        printf("Calculating: %llu to %llu mod %llu\n", begin, end, mod);

        uint64_t result = 1;
        for (uint64_t i = begin; i <= end; i++) {
            result = MultModulo(result, i, mod);
        }

        send(client_fd, &result, sizeof(uint64_t), 0);
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}