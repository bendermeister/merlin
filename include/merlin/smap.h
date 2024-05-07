#ifndef MERLIN_SMAP_H
#define MERLIN_SMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <merlin/aloctr.h>
#include <merlin/str8.h>
#include <stdint.h>

typedef struct mrln_smap_t mrln_smap_t;
struct mrln_smap_t {
  mrln_str8view_t *key;
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

__attribute__((nonnull(1, 2))) void mrln_smap_destroy(mrln_smap_t *t,
                                                      mrln_aloctr_t *a);

__attribute__((nonnull(1, 3), warn_unused_result("returns an error code"))) int
mrln_smap(mrln_smap_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 3), warn_unused_result("returns an error code"))) int
mrln_smap_reserve(mrln_smap_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 2), warn_unused_result("returns an error code"))) int
mrln_smap_shrink(mrln_smap_t *t, mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result, const)) intptr_t
mrln_smap_find(const mrln_smap_t *t, const mrln_str8view_t key);

__attribute__((nonnull(1), warn_unused_result, const)) bool
mrln_smap_contains(const mrln_smap_t *t, const mrln_str8view_t key);

__attribute__((nonnull(1, 4), warn_unused_result("returns an error code"))) int
mrln_smap_insert(mrln_smap_t *t, const mrln_str8view_t key, uintptr_t value,
                 mrln_aloctr_t *a);

__attribute__((nonnull(1, 3, 4),
               warn_unused_result("returns an error code"))) int
mrln_smap_upsert(mrln_smap_t *t, const mrln_str8view_t key, uintptr_t *value,
                 mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result("returns an error code"))) int
mrln_smap_remove(mrln_smap_t *t, const mrln_str8view_t key, mrln_aloctr_t *a);

__attribute__((nonnull(1))) void mrln_smap_clear(mrln_smap_t *t);

__attribute__((nonnull(1), const, warn_unused_result)) bool
mrln_smap_isset(mrln_smap_t *t, intptr_t i);

__attribute__((nonnull(1, 2, 3),
               warn_unused_result("returns an error code"))) int
mrln_smap_copy(mrln_smap_t *dest, const mrln_smap_t *src, mrln_aloctr_t *a);

#ifdef __cplusplus
}
#endif

#endif // MERLIN_SMAP_H
