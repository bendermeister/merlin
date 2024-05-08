#ifndef DDMAP_H
#define DDMAP_H

#ifdef __cplusplus
extern "C" {
#define restrict __restrict__
#endif // __cplusplus

#include "../../../src/util.h"
#include <merlin/aloctr.h>
#include <stdbool.h>

typedef void ddmap_hash_t;
typedef void ddmap_cmpeq_t;

typedef u64 (*ddmap_hash_f)(const void *) __attribute__((const, nonnull(1)));
typedef bool (*ddmap_iseq_f)(const void *, const void *)
    __attribute__((const, nonnull(1, 2)));

typedef struct ddmap_vt_t ddmap_vt_t;
struct ddmap_vt_t {
  const ddmap_iseq_f iseq;
  const ddmap_hash_f hash;
  const u16 keysz;
  const u16 valsz;
};

typedef struct ddmap_t ddmap_t;
struct ddmap_t {
  uint8_t *ctrl;
  i32 chnksz;
  i32 bufsz;
  i32 len;
  i32 tomb;
};

NODISCARD NONNULL(1, 2, 4) int ddmap(ddmap_t *t, const ddmap_vt_t *vt,
                                     const i32 cap, mrln_aloctr_t *a);
NONNULL(1, 2) void ddmap_destroy(ddmap_t *t, mrln_aloctr_t *a);

CONST_FUNC NONNULL(1, 2, 3) i32
    ddmap_find(const ddmap_t *t, const ddmap_vt_t *vt,
               const void *restrict key);

CONST_FUNC NONNULL(1, 2, 3) bool ddmap_contains(const ddmap_t *t,
                                                const ddmap_vt_t *vt,
                                                const void *restrict key);

CONST_FUNC NONNULL(1) bool ddmap_isset(const ddmap_t *t, const i32 i);

CONST_FUNC NONNULL(1, 2) void *ddmap_key(const ddmap_t *t, const ddmap_vt_t *vt,
                                         const i32 i);

CONST_FUNC NONNULL(1, 2) void *ddmap_val(const ddmap_t *t, const ddmap_vt_t *vt,
                                         const i32 i);

NONNULL(1, 2, 3, 4, 5)
NODISCARD int ddmap_insert(ddmap_t *t, const ddmap_vt_t *vt,
                           const void *restrict key, const void *restrict val,
                           mrln_aloctr_t *a);
NONNULL(1, 2, 3, 4, 5)
NODISCARD int ddmap_upsert(ddmap_t *t, const ddmap_vt_t *vt,
                           const void *restrict key, void *restrict val,
                           mrln_aloctr_t *a);

NONNULL(1, 2, 4)
NODISCARD int ddmap_reserve(ddmap_t *t, const ddmap_vt_t *vt, const i32 cap,
                            mrln_aloctr_t *a);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ddmap_H
