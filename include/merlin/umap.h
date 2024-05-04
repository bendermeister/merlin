#ifndef MERLIN_UMAP_H
#define MERLIN_UMAP_H

#include <merlin/aloctr.h>
#include <merlin/str8.h>
#include <stdint.h>

typedef struct mrln_umap_t mrln_umap_t;
struct mrln_umap_t {
  uintptr_t *key;
  uintptr_t *val;
  intptr_t len;
  union {
    intptr_t _bufsz;
    intptr_t end;
  };

  uint8_t *_ctrl;
  intptr_t _chnksz;
  int _tomb;
} __attribute__((aligned(64)));

__attribute__((nonnull(1, 3), warn_unused_result("returns an error code"))) int
mrln_umap(mrln_umap_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 3), warn_unused_result("returns an error code"))) int
mrln_umap_reserve(mrln_umap_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 2))) int mrln_umap_shrink(mrln_umap_t *t,
                                                    mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result("retuns an error code"))) int
mrln_umap_rehash(mrln_umap_t *t, mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result, const)) intptr_t
mrln_umap_find(const mrln_umap_t *t, const uint64_t key);

__attribute__((nonnull(1), warn_unused_result, const)) bool
mrln_umap_contains(const mrln_umap_t *t, const uint64_t key);

__attribute__((nonnull(1, 4), warn_unused_result("returns an error code"))) int
mrln_umap_insert(mrln_umap_t *t, const uint64_t key, uint64_t value,
                 mrln_aloctr_t *a);

__attribute__((nonnull(1, 3, 4),
               warn_unused_result("returns an error code"))) int
mrln_umap_upsert(mrln_umap_t *t, const uint64_t key, uint64_t *value,
                 mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result("returns an error code"))) int
mrln_umap_remove(mrln_umap_t *t, const uint64_t key, mrln_aloctr_t *a);

__attribute__((nonnull(1))) void mrln_umap_clear(mrln_umap_t *t);

__attribute__((nonnull(1), const)) bool mrln_umap_isset(mrln_umap_t *t,
                                                        intptr_t i);

__attribute__((nonnull(1, 2), warn_unused_result("returns an error code"))) int
mrln_umap_copy(mrln_umap_t *restrict dest, const mrln_umap_t *restrict src,
               mrln_aloctr_t *a);

__attribute__((nonnull(1, 2))) void mrln_umap_destroy(mrln_umap_t *t,
                                                      mrln_aloctr_t *a);

#endif // MERLIN_UMAP_H
