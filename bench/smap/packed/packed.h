#ifndef PACKED_H
#define PACKED_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <merlin/aloctr.h>
#include <merlin/str8.h>

typedef struct packed_key_t packed_key_t;

typedef struct packed_t packed_t;
struct packed_t {
  uintptr_t *val;
  packed_key_t *key;
  uint8_t *ctrl;
  intptr_t chnksz;
  intptr_t bufsz;
  intptr_t len;
  intptr_t tomb;
} __attribute__((aligned(64)));

void packed_clear(packed_t *t);

__attribute__((nonnull(1, 2))) void packed_destroy(packed_t *t,
                                                   mrln_aloctr_t *a);

__attribute__((const, nonnull(1, 2), warn_unused_result)) intptr_t
packed_find(const packed_t *t, const mrln_str8view_t *key);

__attribute__((warn_unused_result, nonnull(1, 3))) int
packed(packed_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 3), warn_unused_result)) int
packed_reserve(packed_t *t, const intptr_t capacity, mrln_aloctr_t *a);

__attribute__((nonnull(1, 2), warn_unused_result)) int
packed_shrink(packed_t *t, mrln_aloctr_t *a);

__attribute__((nonnull(1, 2), warn_unused_result)) int
packed_rehash(packed_t *t, mrln_aloctr_t *a);

__attribute__((nonnull(1), warn_unused_result, const)) bool
packed_contains(const packed_t *t, const mrln_str8view_t *key);

__attribute__((nonnull(1, 4), warn_unused_result)) int
packed_insert(packed_t *t, const mrln_str8view_t *key, uint64_t val,
              mrln_aloctr_t *a);

__attribute__((nonnull(1, 3, 4), warn_unused_result)) int
packed_upsert(packed_t *t, const mrln_str8view_t *key, uint64_t *val,
              mrln_aloctr_t *a);

__attribute__((nonnull(1))) void packed_clear(packed_t *t);

__attribute__((const, nonnull(1), warn_unused_result)) bool
packed_isset(packed_t *t, intptr_t i);

__attribute__((nonnull(1, 2))) void packed_destroy(packed_t *t,
                                                   mrln_aloctr_t *a);

__attribute__((nonnull(1), const, warn_unused_result)) intptr_t
packed_find(const packed_t *t, const mrln_str8view_t *key);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PACKED_H
