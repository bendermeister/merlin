#include "../util.h"

#include <merlin/aloctr.h>
#include <merlin/simd.h>
#include <merlin/umap.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

CONST_FUNC NODISCARD static intptr_t cap_to_bufz(const intptr_t cap) {
  var t = (cap * 8) / 7;
  t = MAX(t, 16);
  return 1 << (sizeof(long long) * 8 - __builtin_clzll(t));
}

CONST_FUNC NODISCARD static intptr_t bufz_to_chnksz(const intptr_t bufsz) {
  return sizeof(u8) * bufsz + 32 * sizeof(u8) + sizeof(uintptr_t) * bufsz +
         sizeof(uintptr_t) * bufsz;
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *keybuf(const mrln_umap_t *t) {
  return (uintptr_t *)(t->_ctrl + 32 + t->_bufsz);
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *valbuf(const mrln_umap_t *t) {
  return keybuf(t) + t->_bufsz;
}

#define ISSET ((u8)0x80)
#define TOMB 1

CONST_FUNC NODISCARD static u8 hash_to_ctrl(const u64 hash) {
  return (hash >> 56) | ISSET;
}

CONST_FUNC NODISCARD static u64 hash(uintptr_t x) {
  x ^= x >> 30;
  x *= 0xbf58476d1ce4e5b9U;
  x ^= x >> 27;
  x *= 0x94d049bb133111ebU;
  x ^= x >> 31;
  return x;
}

CONST_FUNC NODISCARD NONNULL(1) static intptr_t
    find(const mrln_umap_t *t, const uintptr_t key, const u64 h) {
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  let zero = mrln_v32u8_set1(0);
  let mod = t->_bufsz - 1;
  intptr_t i = h & mod;

  for (intptr_t j = 32;; ++j, i = (i + j) & mod) {
    let row = mrln_v32u8_load_unaligned((const void *)&t->_ctrl[i]);
    let t0 = mrln_v32u8_cmpeq(row, ctrl);
    let t1 = mrln_v32u8_cmpeq(row, zero);
    let t2 = mrln_v32u8_or(t0, t1);
    var m = mrln_v32u8_mask(t2);

    for (intptr_t k = 0; m; ++k, m >>= 1) {
      let ctz = __builtin_ctz(m);
      m >>= ctz;
      k += ctz;
      let l = (i + k) & mod;
      if (!t->_ctrl[l] || t->key[l] == key) {
        return l;
      }
    }
  }

  __builtin_unreachable();
}

NONNULL(1, 3)
NODISCARD static int rehash_to(mrln_umap_t *t, const intptr_t bufsz,
                               mrln_aloctr_t *a) {
  mrln_umap_t new = {._bufsz = bufsz};
  let err = mrln_alloc(a, (void **)&new._ctrl, &new._chnksz, 32,
                       bufz_to_chnksz(new._bufsz));
  if (UNLIKELY(err)) {
    return err;
  }
  new.key = keybuf(&new);
  new.val = valbuf(&new);
  mrln_umap_clear(&new);
  new.len = t->len;

  for (intptr_t i = 0; i < t->end; ++i) {
    if (!(t->_ctrl[i] & ISSET)) {
      continue;
    }
    let j = mrln_umap_find(&new, t->key[i]);
    new._ctrl[j] = t->_ctrl[i];
    if (j < 32) {
      new._ctrl[j + new._bufsz] = t->_ctrl[i];
    }
    new.key[j] = t->key[i];
    new.val[j] = t->val[i];
  }

  mrln_umap_destroy(t, a);
  *t = new;

  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(mrln_umap_t *t, mrln_aloctr_t *a) {
  let should = t->_tomb + t->len >= (t->_bufsz * 7) / 8;
  if (should) {
    const intptr_t bufsz = MAX(32, t->_bufsz * 2);
    return rehash_to(t, bufsz, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 3) int mrln_umap(mrln_umap_t *t, const intptr_t capacity,
                                      mrln_aloctr_t *a) {
  let bufsz = cap_to_bufz(capacity);
  if (bufsz <= t->_bufsz) {
    mrln_umap_clear(t);
    return 0;
  }
  t->_bufsz = bufsz;
  let chnksz = bufz_to_chnksz(t->_bufsz);
  let err = mrln_alloc(a, (void **)&t->_ctrl, &t->_chnksz, 32, chnksz);
  if (UNLIKELY(err)) {
    return err;
  }
  t->key = keybuf(t);
  t->val = valbuf(t);
  mrln_umap_clear(t);
  return 0;
}

NONNULL(1, 3)
NODISCARD int mrln_umap_reserve(mrln_umap_t *t, const intptr_t capacity,
                                mrln_aloctr_t *a) {
  let bufsz = cap_to_bufz(capacity);
  if (t->_bufsz >= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

__attribute__((nonnull(1, 2))) int mrln_umap_shrink(mrln_umap_t *t,
                                                    mrln_aloctr_t *a) {
  let bufsz = cap_to_bufz(t->len);
  if (t->_bufsz <= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int mrln_umap_rehash(mrln_umap_t *t, mrln_aloctr_t *a) {
  return rehash_to(t, t->_bufsz, a);
}

NONNULL(1)
NODISCARD CONST_FUNC bool mrln_umap_contains(const mrln_umap_t *t,
                                             const uint64_t key) {
  let i = mrln_umap_find(t, key);
  return t->_ctrl[i] & ISSET;
}

NONNULL(1, 4)
NODISCARD int mrln_umap_insert(mrln_umap_t *t, const uint64_t key, uint64_t val,
                               mrln_aloctr_t *a) {
  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }
  let h = hash(key);
  let i = find(t, key, h);

  if (!t->_ctrl[i]) {
    t->len += 1;
    t->_ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->_ctrl[i + t->_bufsz] = t->_ctrl[i];
    }
    t->key[i] = key;
  }
  t->val[i] = val;
  return 0;
}

NONNULL(1, 3, 4)
NODISCARD int mrln_umap_upsert(mrln_umap_t *t, const uint64_t key,
                               uint64_t *val, mrln_aloctr_t *a) {
  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }
  let h = hash(key);
  let i = find(t, key, h);

  if (!t->_ctrl[i]) {
    t->len += 1;
    t->_ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->_ctrl[i + t->_bufsz] = t->_ctrl[i];
    }
    t->key[i] = key;
    t->val[i] = *val;
  }
  *val = t->val[i];
  return 0;
}

NONNULL(1, 3)
NODISCARD int mrln_umap_remove(mrln_umap_t *t, const uint64_t key,
                               mrln_aloctr_t *a) {

  let i = mrln_umap_find(t, key);
  if (!(t->_ctrl[i] & ISSET)) {
    return 0;
  }
  t->_ctrl[i] = TOMB;
  if (i < 32) {
    t->_ctrl[i + t->_bufsz] = TOMB;
  }
  t->_tomb += 1;
  if (t->_tomb >= t->_bufsz / 4) {
    return rehash_to(t, t->_bufsz, a);
  }
  return 0;
}

__attribute__((nonnull(1))) void mrln_umap_clear(mrln_umap_t *t) {
  t->len = 0;
  t->_tomb = 0;
  memset(t->_ctrl, 0, t->_bufsz + 32);
}

__attribute__((nonnull(1), const)) bool mrln_umap_isset(mrln_umap_t *t,
                                                        intptr_t i) {
  return t->_ctrl[i] & ISSET;
}

__attribute__((nonnull(1, 2, 3),
               warn_unused_result("returns an error code"))) int
mrln_umap_copy(mrln_umap_t *restrict dest, const mrln_umap_t *restrict src,
               mrln_aloctr_t *a) {
  if (dest->_bufsz < src->_bufsz) {
    let err =
        mrln_alloc(a, (void **)&dest->_ctrl, &dest->_chnksz, 32, src->_chnksz);
    if (UNLIKELY(err)) {
      return err;
    }
    dest->key = keybuf(dest);
    dest->val = valbuf(dest);
    dest->_bufsz = src->_bufsz;
  }
  mrln_umap_clear(dest);
  dest->len = src->len;
  memcpy(dest->_ctrl, src->_ctrl, src->_chnksz);
  return 0;
}

NONNULL(1, 2) void mrln_umap_destroy(mrln_umap_t *t, mrln_aloctr_t *a) {
  (void)mrln_alloc(a, (void **)&t->_ctrl, &t->_chnksz, 32, 0);
  *t = (mrln_umap_t){};
}

NONNULL(1)
CONST_FUNC NODISCARD intptr_t mrln_umap_find(const mrln_umap_t *t,
                                             const uintptr_t key) {
  let h = hash(key);
  return find(t, key, h);
}
