#include <merlin/aloctr.h>

#include "../util.h"

#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>

NONNULL(1, 2) NODISCARD static int free_wrapper(void **chnk, intptr_t *chnksz) {
  free(*chnk);
  *chnk = NULL;
  *chnksz = 0;
  return 0;
}

NONNULL(1, 2)
NODISCARD static int realloc_wrapper(void **chnk, intptr_t *chnksz,
                                     const intptr_t new_chnksz) {
  var p = realloc(*chnk, new_chnksz);
  if (p == NULL) {
    return ENOMEM;
  }
  *chnk = p;
  *chnksz = new_chnksz;
  return 0;
}

NONNULL(1, 2)
NODISCARD
static int aligned_alloc_wrapper(void **chnk, intptr_t *chnksz, int align,
                                 const intptr_t new_chnksz) {
  var p = aligned_alloc(align, new_chnksz);
  if (!p) {
    return ENOMEM;
  }
  if (*chnk) {
    memcpy(p, *chnk, *chnksz);
  }
  *chnk = p;
  *chnksz = new_chnksz;
  return 0;
}

NONNULL(1, 2, 3)
NODISCARD static int global_aloctr_func(mrln_aloctr_t *a, void **chunk,
                                        isize *chunk_size, const intptr_t align,
                                        const intptr_t size) {
  UNUSED(a);
  if (size == 0) {
    return free_wrapper(chunk, chunk_size);
  }

  if (align <= 16) {
    return realloc_wrapper(chunk, chunk_size, size);
  }

  return aligned_alloc_wrapper(chunk, chunk_size, align, size);
}

static struct {
  mrln_aloctr_f *alloc;
} global_aloctr = {&global_aloctr_func};

mrln_aloctr_t *mrln_aloctr_global(void) { return &global_aloctr; }
