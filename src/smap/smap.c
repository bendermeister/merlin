#include "../util.h"
#include <merlin/smap.h>
#include <xxhash.h>

CONST_FUNC NODISCARD u64 hash(const mrln_str8view_t *s) {}

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

__attribute__((nonnull(1))) bool mrln_smap_remove(mrln_smap_t *t,
                                                  const mrln_str8view_t key);

__attribute__((nonnull(1))) void mrln_smap_clear(mrln_smap_t *t);

__attribute__((nonnull(1), const, warn_unused_result)) bool
mrln_smap_isset(mrln_smap_t *t, intptr_t i);

__attribute__((nonnull(1, 2, 3),
               warn_unused_result("returns an error code"))) int
mrln_smap_copy(mrln_smap_t *restrict dest, const mrln_smap_t *restrict src,
               mrln_aloctr_t *a);
