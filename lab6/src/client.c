#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>

struct Server {
  char ip[255];
  int port;
};

struct ThreadArgs {
    struct Server server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }
  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }
  if (errno != 0)
    return false;
  *val = i;
  return true;
}

void *ProcessServer(void *args) {
    struct ThreadArgs *t_args = (struct ThreadArgs *)args;
    uint64_t *result = malloc(sizeof(uint64_t));
    *result = 1; 

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) return result;

    struct hostent *hostname = gethostbyname(t_args->server.ip);
    if (hostname == NULL) {
        fprintf(stderr, "gethostbyname failed: %s\n", t_args->server.ip);
        close(sck);
        return result;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(t_args->server.port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Connection to %s:%d failed\n", t_args->server.ip, t_args->server.port);
        close(sck);
        return result;
    }

    uint64_t task[3] = {t_args->begin, t_args->end, t_args->mod};
    if (send(sck, task, sizeof(task), 0) < 0) {
        close(sck);
        return result;
    }

    if (recv(sck, result, sizeof(uint64_t), 0) < 0) {
        *result = 1;
    }

    close(sck);
    return (void *)result;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; 

  while (true) {
    static struct option options[] = {
        {"k", required_argument, 0, 0},
        {"mod", required_argument, 0, 0},
        {"servers", required_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0: ConvertStringToUI64(optarg, &k); break;
          case 1: ConvertStringToUI64(optarg, &mod); break;
          case 2: strncpy(servers, optarg, 254); break;
        }
        break;
      case '?': printf("Arguments error\n"); break;
      default: break;
    }
  }

  if (k == -1 || mod == -1 || strlen(servers) == 0) {
    fprintf(stderr, "Usage: %s --k 1000 --mod 5 --servers /path/to/file\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(servers, "r");
  if (!f) {
      perror("Failed to open servers file");
      return 1;
  }

  unsigned int servers_num = 0;
  char line[256];
  while (fgets(line, sizeof(line), f)) servers_num++;
  rewind(f);

  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  for (int i = 0; i < servers_num; i++) {
      fscanf(f, "%[^:]:%d\n", to[i].ip, &to[i].port);
  }
  fclose(f);

  pthread_t threads[servers_num];
  struct ThreadArgs args[servers_num];
  uint64_t chunk = k / servers_num;

  for (int i = 0; i < servers_num; i++) {
      args[i].server = to[i];
      args[i].mod = mod;
      args[i].begin = i * chunk + 1;
      args[i].end = (i == servers_num - 1) ? k : (i + 1) * chunk;

      if (pthread_create(&threads[i], NULL, ProcessServer, (void *)&args[i]) != 0) {
          perror("pthread_create failed");
          return 1;
      }
  }

  uint64_t total_answer = 1;
  for (int i = 0; i < servers_num; i++) {
      uint64_t *thread_result;
      pthread_join(threads[i], (void **)&thread_result);
      total_answer = MultModulo(total_answer, *thread_result, mod);
      free(thread_result);
  }

  printf("Final answer for %llu! mod %llu: %llu\n", k, mod, total_answer);

  free(to);
  return 0;
}