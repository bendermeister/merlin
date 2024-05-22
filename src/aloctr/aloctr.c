#include <merlin/aloctr.h>

#include "../util.h"

#include <stdlib.h>

static void free_wrapper(aloctr_t *a, void *p) {
  UNUSED(a);
  free(p);
}

static void *realloc_wrapper(aloctr_t *a, void *p, u64 size) {
  UNUSED(a);
  if (size == 0) {
    free(p);
    return NULL;
  }
  return realloc(p, size);
}

static void *malloc_wrapper(aloctr_t *a, u64 size) {
  UNUSED(a);
  if (size == 0) {
    return NULL;
  }
  return malloc(size);
}

static struct aloctr_layout_t layout = {0};
static aloctr_t *global = NULL;

aloctr_t *aloctr_global(void) {
  if (UNLIKELY(!global)) {
    layout = (struct aloctr_layout_t){
        .allocate = &malloc_wrapper,
        .reallocate = &realloc_wrapper,
        .deallocate = &free_wrapper,
    };
    global = &layout;
  }
  return global;
}
