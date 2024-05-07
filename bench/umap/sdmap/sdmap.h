#ifndef sdmap_H
#define sdmap_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "../../../src/util.h"
#include <merlin/aloctr.h>
#include <stdbool.h>

typedef void sdmap_hash_t;
typedef void sdmap_cmpeq_t;

typedef struct sdmap_vtble_t sdmap_vtble_t;
struct sdmap_vtble_t {
  u64 (*hash)(const void *);
  bool (*iseq)(const void *, const void *);
  u16 keysz;
  u16 valsz;
};

typedef struct sdmap_t sdmap_t;
struct sdmap_t {
  const sdmap_vtble_t *vt;
  i32 chnksz;
  i32 bufsz;
  i32 len;
  i32 tomb;
  uint8_t *ctrl;
};

NODISCARD NONNULL(1, 3) int sdmap(sdmap_t *t, const i32 cap, mrln_aloctr_t *a);
NONNULL(1, 2) void sdmap_destroy(sdmap_t *t, mrln_aloctr_t *a);

CONST_FUNC NONNULL(1, 2) i32 sdmap_find(const sdmap_t *t, const void *key);
CONST_FUNC NONNULL(1, 2) bool sdmap_contains(const sdmap_t *t, const void *key);
CONST_FUNC NONNULL(1) bool sdmap_isset(const sdmap_t *t, const i32 i);
CONST_FUNC NONNULL(1) const void *sdmap_key(const sdmap_t *t, const i32 i);
CONST_FUNC NONNULL(1) const void *sdmap_val(const sdmap_t *t, const i32 i);

NONNULL(1, 2, 3, 4)
NODISCARD int sdmap_insert(sdmap_t *t, const void *key, const void *val,
                           mrln_aloctr_t *a);
NONNULL(1, 2, 3, 4)
NODISCARD int sdmap_upsert(sdmap_t *t, const void *key, void *val,
                           mrln_aloctr_t *a);

NONNULL(1, 3)
NODISCARD int sdmap_reserve(sdmap_t *t, const i32 cap, mrln_aloctr_t *a);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // sdmap_H
