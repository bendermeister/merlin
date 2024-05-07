#ifndef DMAP_H
#define DMAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "../../../src/util.h"
#include <merlin/aloctr.h>
#include <stdbool.h>

typedef void dmap_hash_t;
typedef void dmap_cmpeq_t;

typedef u64 (*dmap_hash_f)(const void *) __attribute__((const, nonnull(1)));
typedef bool (*dmap_iseq_f)(const void *, const void *)
    __attribute__((const, nonnull(1, 2)));

typedef struct dmap_t dmap_t;
struct dmap_t {
  const dmap_iseq_f iseq;
  const dmap_hash_f hash;
  uint8_t *ctrl;
  i32 chnksz;
  i32 bufsz;
  i32 len;
  i32 tomb;
  u16 keysz;
  u16 valsz;
};

NODISCARD NONNULL(1, 5) int dmap(dmap_t *t, const i32 cap, const u16 keysz,
                                 const u16 valsz, mrln_aloctr_t *a);
NONNULL(1, 2) void dmap_destroy(dmap_t *t, mrln_aloctr_t *a);

CONST_FUNC NONNULL(1, 2) i32 dmap_find(const dmap_t *t, const void *key);
CONST_FUNC NONNULL(1, 2) bool dmap_contains(const dmap_t *t, const void *key);
CONST_FUNC NONNULL(1) bool dmap_isset(const dmap_t *t, const i32 i);
CONST_FUNC NONNULL(1) void *dmap_key(const dmap_t *t, const i32 i);
CONST_FUNC NONNULL(1) void *dmap_val(const dmap_t *t, const i32 i);

NONNULL(1, 2, 3, 4)
NODISCARD int dmap_insert(dmap_t *t, const void *key, const void *val,
                          mrln_aloctr_t *a);
NONNULL(1, 2, 3, 4)
NODISCARD int dmap_upsert(dmap_t *t, const void *key, void *val,
                          mrln_aloctr_t *a);

NONNULL(1, 3)
NODISCARD int dmap_reserve(dmap_t *t, const i32 cap, mrln_aloctr_t *a);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DMAP_H
