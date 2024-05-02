#include <merlin/aloctr.h>

#include "../util.h"

#include <asm-generic/errno-base.h>
#include <stdlib.h>

__attribute__((__nonnull__(1, 2), __warn_unused_result__)) static int
global_aloctr_func(mrln_aloctr_t *a, void **chunk, isize *chunk_size,
                   const intptr_t size) {
  UNUSED(a);
  if (size == 0) {
    free(*chunk);
    *chunk = NULL;
    *chunk_size = 0;
    return 0;
  }

  void *new = realloc(*chunk, size);

  if (UNLIKELY(!new)) {
    return ENOMEM;
  }

  *chunk = new;
  *chunk_size = size;
  return 0;
}

static struct {
  mrln_aloctr_f *alloc;
} global_aloctr = {&global_aloctr_func};

mrln_aloctr_t *mrln_aloctr_global(void) { return &global_aloctr; }
