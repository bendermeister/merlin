#ifndef MERLIN_VIEW_H
#define MERLIN_VIEW_H

#include <stddef.h>
#include <stdint.h>

typedef struct merlin_view_t merlin_view_t;
struct merlin_view_t {
  size_t size;
  uint8_t *buffer;
};

#endif // MELRIN_VIEW_H
