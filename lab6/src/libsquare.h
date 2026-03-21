#ifndef LIBSQUARE_H
#define LIBSQUARE_H

#include <stdint.h>

struct Server {
  char ip[255];
  int port;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

#endif