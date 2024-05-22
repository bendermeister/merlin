#ifndef MRLN_S8_H
#define MRLN_S8_H

#include <merlin/core.h>

typedef struct s8_t s8_t;
struct s8_t {
  char *buf;
  i64 cap;
  i64 len;
};

#endif // MRLN_S8_H
