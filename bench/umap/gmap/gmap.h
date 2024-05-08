#ifndef GMAP_H
#define GMAP_H

#include "../../../src/util.h"
#include <merlin/aloctr.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct gmap_group_t gmap_group_t;
struct gmap_group_t {
  gmap_group_t *next;
  u32 *hash;
  uintptr_t *key;
  uintptr_t *val;
  intptr_t sz;
};

typedef struct gmap_t gmap_t;
struct gmap_t {
  gmap_group_t *group;
  u32 max_group_sz;
};

typedef struct gmap_iter_t gmap_iter_t;
struct gmap_iter_t {
  gmap_group_t **group;
  u32 hash;
  u32 mask;
  u32 index;
  bool isset;
};

gmap_iter_t gmap_iter(gmap_t *t, u32 hash);
gmap_iter_t gmap_next(gmap_iter_t iter);
int gmap_insert(gmap_t *t, gmap_iter_t *iter);

uintptr_t *gmap_key(gmap_iter_t iter);
uintptr_t *gmap_val(gmap_iter_t iter);

void gmap_destroy(gmap_t *t);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GMAP_H
