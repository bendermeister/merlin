#include "../util.h"

#include <errno.h>
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
  ASSUME(bufsz >= 32);

  return sizeof(u8) * bufsz + 32 * sizeof(u8) + sizeof(uintptr_t) * bufsz +
         sizeof(uintptr_t) * bufsz;
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *keybuf(const mrln_umap_t *t) {
  ASSUME(t->ctrl);
  ASSUME(t->bufsz >= 32);

  return (uintptr_t *)(t->ctrl + 32 + t->bufsz);
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *valbuf(const mrln_umap_t *t) {
  ASSUME(t->bufsz >= 32);
  ASSUME(t->ctrl);

  return keybuf(t) + t->bufsz;
}

NONNULL(1)
CONST_FUNC NODISCARD static bool is_well_formed(const mrln_umap_t *t) {

  // either everything is corretly setup so all invariands hold
  if (t->ctrl) {
    let b0 = t->bufsz >= 32 && __builtin_popcountll(t->bufsz) == 1;
    let b2 = t->ctrl != NULL && t->key != NULL && t->val != NULL;
    let b5 = t->key == keybuf(t);
    let b6 = t->val == valbuf(t);
    let b7 = t->len >= 0 && t->tomb >= 0;
    return b0 && b2 && b5 && b6 && b7;
  }

  // or the map is empty initilized
  return t->bufsz == 0 && t->ctrl == 0 && t->key == NULL && t->tomb == 0 &&
         t->len == 0 && t->val == NULL && t->end == 0;
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
  ASSUME(is_well_formed(t));

  let mod = t->bufsz - 1;
  intptr_t i = h & mod;

  // __builtin_prefetch(t->_ctrl + i);

  let ctrl = v32u8_set1(hash_to_ctrl(h));
  let zero = v32u8_set1(0);

  for (intptr_t j = 32;; ++j, i = (i + j) & mod) {
    let row = v32u8_load_unaligned((const void *)&t->ctrl[i]);
    let t0 = v32u8_eq(row, ctrl);
    var m = v32u8_mask(t0);

    for (intptr_t k = 0; m; ++k, m >>= 1) {
      // __builtin_prefetch(t->key + i);
      let ctz = __builtin_ctz(m);
      m >>= ctz;
      k += ctz;
      let l = (i + k) & mod;
      if (t->key[l] == key) {
        return l;
      }
    }
    let t1 = v32u8_eq(row, zero);
    m = v32u8_mask(t1);
    if (m) {
      return (i + __builtin_ctz(m)) & mod;
    }
  }

  __builtin_unreachable();
}

NONNULL(1, 3)
NODISCARD static int rehash_to(mrln_umap_t *t, const intptr_t bufsz,
                               aloctr_t *a) {
  ASSUME(is_well_formed(t));

  mrln_umap_t new = {.bufsz = bufsz};
  new.ctrl = allocate(a, bufz_to_chnksz(new.bufsz));
  if (UNLIKELY(!new.ctrl)) {
    return errno;
  }

  new.key = keybuf(&new);
  new.val = valbuf(&new);
  mrln_umap_clear(&new);
  new.len = t->len;

  for (intptr_t i = 0; i < t->end; ++i) {
    if (!(t->ctrl[i] & ISSET)) {
      continue;
    }
    let j = mrln_umap_find(&new, t->key[i]);
    new.ctrl[j] = t->ctrl[i];
    if (j < 32) {
      new.ctrl[j + new.bufsz] = t->ctrl[i];
    }
    new.key[j] = t->key[i];
    new.val[j] = t->val[i];
  }

  mrln_umap_destroy(t, a);
  *t = new;

  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(mrln_umap_t *t, aloctr_t *a) {
  ASSUME(is_well_formed(t));

  let should = t->tomb + t->len >= (t->bufsz * 7) / 8;
  if (should) {
    const intptr_t bufsz = MAX(32, t->bufsz * 2);
    return rehash_to(t, bufsz, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 3) int mrln_umap(mrln_umap_t *t, const intptr_t capacity,
                                      aloctr_t *a) {
  ASSUME(is_well_formed(t));

  let bufsz = cap_to_bufz(capacity);
  if (bufsz <= t->bufsz) {
    mrln_umap_clear(t);
    return 0;
  }
  t->bufsz = bufsz;
  let chnksz = bufz_to_chnksz(t->bufsz);
  t->ctrl = allocate(a, chnksz);
  if (UNLIKELY(!t->ctrl)) {
    return errno;
  }
  t->key = keybuf(t);
  t->val = valbuf(t);
  mrln_umap_clear(t);
  return 0;
}

NONNULL(1, 3)
NODISCARD int mrln_umap_reserve(mrln_umap_t *t, const intptr_t capacity,
                                aloctr_t *a) {
  ASSUME(is_well_formed(t));

  let bufsz = cap_to_bufz(capacity);
  if (t->bufsz >= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int mrln_umap_shrink(mrln_umap_t *t, aloctr_t *a) {
  ASSUME(is_well_formed(t));

  if (!t->ctrl) {
    return 0;
  }

  let bufsz = cap_to_bufz(t->len);
  if (t->bufsz <= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int mrln_umap_rehash(mrln_umap_t *t, aloctr_t *a) {
  ASSUME(is_well_formed(t));

  return rehash_to(t, t->bufsz, a);
}

NONNULL(1)
NODISCARD CONST_FUNC bool mrln_umap_contains(const mrln_umap_t *t,
                                             const uint64_t key) {
  ASSUME(is_well_formed(t));

  if (t->len == 0) {
    return false;
  }

  let i = mrln_umap_find(t, key);
  return t->ctrl[i] & ISSET;
}

NONNULL(1, 4)
NODISCARD int mrln_umap_insert(mrln_umap_t *t, const uint64_t key, uint64_t val,
                               aloctr_t *a) {
  ASSUME(is_well_formed(t));

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let h = hash(key);
  let i = find(t, key, h);
  if (!t->ctrl[i]) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = t->ctrl[i];
    }
    t->key[i] = key;
  }
  t->val[i] = val;
  return 0;
}

NONNULL(1, 3, 4)
NODISCARD int mrln_umap_upsert(mrln_umap_t *t, const uint64_t key,
                               uint64_t *val, aloctr_t *a) {
  ASSUME(is_well_formed(t));

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }
  let h = hash(key);
  let i = find(t, key, h);

  if (!t->ctrl[i]) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = t->ctrl[i];
    }
    t->key[i] = key;
    t->val[i] = *val;
  }
  *val = t->val[i];
  return 0;
}

NONNULL(1, 3)
NODISCARD int mrln_umap_remove(mrln_umap_t *t, const uint64_t key,
                               aloctr_t *a) {
  ASSUME(is_well_formed(t));

  if (t->len == 0) {
    return 0;
  }

  let i = mrln_umap_find(t, key);
  if (!(t->ctrl[i] & ISSET)) {
    return 0;
  }
  t->ctrl[i] = TOMB;
  if (i < 32) {
    t->ctrl[i + t->bufsz] = TOMB;
  }
  t->tomb += 1;
  if (t->tomb >= t->bufsz / 4) {
    return rehash_to(t, t->bufsz, a);
  }
  return 0;
}

NONNULL(1)
void mrln_umap_clear(mrln_umap_t *t) {
  ASSUME(is_well_formed(t));

  if (t->ctrl) {
    memset(t->ctrl, 0, t->bufsz + 32);
    t->len = 0;
    t->tomb = 0;
  }
}

CONST_FUNC NONNULL(1) bool mrln_umap_isset(mrln_umap_t *t, intptr_t i) {
  ASSUME(is_well_formed(t));
  if (i < 0) {
    return false;
  }
  return t->ctrl[i] & ISSET;
}

NONNULL(1, 2, 3);
NODISCARD int mrln_umap_copy(mrln_umap_t *restrict dest,
                             const mrln_umap_t *restrict src, aloctr_t *a) {
  ASSUME(is_well_formed(src));

  if (dest->bufsz < src->bufsz) {
    dest->ctrl = allocate(a, bufz_to_chnksz(src->bufsz));
    if (UNLIKELY(!dest->ctrl)) {
      return errno;
    }
    dest->key = keybuf(dest);
    dest->val = valbuf(dest);
    dest->bufsz = src->bufsz;
  }
  mrln_umap_clear(dest);
  dest->len = src->len;
  memcpy(dest->ctrl, src->ctrl, bufz_to_chnksz(src->bufsz));
  return 0;
}

NONNULL(1, 2) void mrln_umap_destroy(mrln_umap_t *t, aloctr_t *a) {
  ASSUME(is_well_formed(t));

  deallocate(a, t->ctrl);
  *t = (mrln_umap_t){};
}

NONNULL(1)
CONST_FUNC NODISCARD intptr_t mrln_umap_find(const mrln_umap_t *t,
                                             const uintptr_t key) {
  ASSUME(is_well_formed(t));
  if (t->len == 0) {
    return -1;
  }

  let h = hash(key);
  return find(t, key, h);
}
