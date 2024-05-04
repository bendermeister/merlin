#include "../util.h"
#include <merlin/simd.h>
#include <merlin/smap.h>
#include <merlin/str8.h>
#include <stddef.h>
#include <string.h>

CONST_FUNC NODISCARD u64 hash(const mrln_str8view_t *s) {
  // fnv-1 hash with primes from wikipedia
  u64 h = 0xcbf29ce484222325ull;
  for (intptr_t i = 0; i < s->length; ++i) {
    h ^= s->buffer[i];
    h *= 0x00000100000001B3;
  }
  return h;
}

CONST_FUNC NODISCARD static intptr_t cap_to_bufz(const intptr_t cap) {
  var t = (cap * 8) / 7;
  t = MAX(t, 16);
  return 1 << (sizeof(long long) * 8 - __builtin_clzll(t));
}

CONST_FUNC NODISCARD static intptr_t bufz_to_chnksz(const intptr_t bufsz) {
  ASSUME(bufsz >= 32);

  return sizeof(u8) * bufsz + 32 * sizeof(u8) +
         sizeof(mrln_str8view_t) * bufsz + sizeof(uintptr_t) * bufsz;
}

CONST_FUNC NODISCARD
NONNULL(1) static mrln_str8view_t *keybuf(const mrln_smap_t *t) {
  ASSUME(t);
  ASSUME(t->_ctrl);
  ASSUME(t->_bufsz >= 32);

  return (mrln_str8view_t *)(t->_ctrl + 32 + t->_bufsz);
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *valbuf(const mrln_smap_t *t) {
  ASSUME(t);
  ASSUME(t->_bufsz >= 32);
  ASSUME(t->_ctrl);

  return (uintptr_t *)(keybuf(t) + t->_bufsz);
}

NONNULL(1)
CONST_FUNC NODISCARD static bool is_well_formed(const mrln_smap_t *t) {
  ASSUME(t);

  // either everything is corretly setup so all invariands hold
  if (t->_chnksz) {
    let b0 = t->_bufsz >= 32 && __builtin_popcountll(t->_bufsz) == 1;
    let b1 = t->_chnksz >= bufz_to_chnksz(t->_bufsz);
    let b2 = t->_ctrl != NULL && t->key != NULL && t->val != NULL;
    let b3 = (void *)t->key < (void *)(t->_ctrl + t->_chnksz);
    let b4 = (void *)t->val < (void *)(t->_ctrl + t->_chnksz);
    let b5 = t->key == keybuf(t);
    let b6 = t->val == valbuf(t);
    let b7 = t->len >= 0 && t->_tomb >= 0;
    return b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7;
  }

  // or the map is empty initilized
  return t->_bufsz == 0 && t->_chnksz == 0 && t->_ctrl == 0 && t->key == NULL &&
         t->_tomb == 0 && t->len == 0 && t->val == NULL && t->end == 0;
}

#define ISSET ((u8)0x80)
#define TOMB 1

CONST_FUNC NODISCARD static u8 hash_to_ctrl(const u64 hash) {
  return (hash >> 56) | ISSET;
}

CONST_FUNC NODISCARD NONNULL(1) static intptr_t
    find(const mrln_smap_t *t, const mrln_str8view_t key, const u64 h) {
  ASSUME(t);
  ASSUME(is_well_formed(t));

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
      if (!t->_ctrl[l] || mrln_str8view_is_equal(&t->key[l], &key)) {
        return l;
      }
    }
  }

  __builtin_unreachable();
}

NONNULL(1, 3)
NODISCARD static int rehash_to(mrln_smap_t *t, const intptr_t bufsz,
                               mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  mrln_smap_t new = {._bufsz = bufsz};
  let err = mrln_alloc(a, (void **)&new._ctrl, &new._chnksz, 32,
                       bufz_to_chnksz(new._bufsz));
  if (UNLIKELY(err)) {
    return err;
  }

  new.key = keybuf(&new);
  new.val = valbuf(&new);
  mrln_smap_clear(&new);
  new.len = t->len;

  for (intptr_t i = 0; i < t->end; ++i) {
    if (!(t->_ctrl[i] & ISSET)) {
      continue;
    }
    let j = mrln_smap_find(&new, t->key[i]);
    new._ctrl[j] = t->_ctrl[i];
    if (j < 32) {
      new._ctrl[j + new._bufsz] = t->_ctrl[i];
    }
    new.key[j] = t->key[i];
    new.val[j] = t->val[i];
  }

  mrln_smap_destroy(t, a);
  *t = new;

  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(mrln_smap_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let should = t->_tomb + t->len >= (t->_bufsz * 7) / 8;
  if (should) {
    const intptr_t bufsz = MAX(32, t->_bufsz * 2);
    return rehash_to(t, bufsz, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 3) int mrln_smap(mrln_smap_t *t, const intptr_t capacity,
                                      mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let bufsz = cap_to_bufz(capacity);
  if (bufsz <= t->_bufsz) {
    mrln_smap_clear(t);
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
  mrln_smap_clear(t);
  return 0;
}

NONNULL(1, 3)
NODISCARD int mrln_smap_reserve(mrln_smap_t *t, const intptr_t capacity,
                                mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let bufsz = cap_to_bufz(capacity);
  if (t->_bufsz >= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int mrln_smap_shrink(mrln_smap_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  if (!t->_ctrl) {
    return 0;
  }

  let bufsz = cap_to_bufz(t->len);
  if (t->_bufsz <= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int mrln_smap_rehash(mrln_smap_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  return rehash_to(t, t->_bufsz, a);
}

NONNULL(1)
NODISCARD CONST_FUNC bool mrln_smap_contains(const mrln_smap_t *t,
                                             const mrln_str8view_t key) {
  ASSUME(t);
  ASSUME(is_well_formed(t));

  if (t->len == 0) {
    return false;
  }

  let i = mrln_smap_find(t, key);
  return t->_ctrl[i] & ISSET;
}

NONNULL(1, 4)
NODISCARD int mrln_smap_insert(mrln_smap_t *t, const mrln_str8view_t key,
                               uint64_t val, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let h = hash(&key);
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
NODISCARD int mrln_smap_upsert(mrln_smap_t *t, const mrln_str8view_t key,
                               uint64_t *val, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(val);
  ASSUME(a);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }
  let h = hash(&key);
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
NODISCARD int mrln_smap_remove(mrln_smap_t *t, const mrln_str8view_t key,
                               mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  if (t->len == 0) {
    return 0;
  }

  let i = mrln_smap_find(t, key);
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

NONNULL(1)
void mrln_smap_clear(mrln_smap_t *t) {
  ASSUME(t);
  ASSUME(is_well_formed(t));

  if (t->_ctrl) {
    memset(t->_ctrl, 0, t->_bufsz + 32);
    t->len = 0;
    t->_tomb = 0;
  }
}

CONST_FUNC NONNULL(1) bool mrln_smap_isset(mrln_smap_t *t, intptr_t i) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  if (i < 0) {
    return false;
  }
  return t->_ctrl[i] & ISSET;
}

NONNULL(1, 2, 3);
NODISCARD int mrln_smap_copy(mrln_smap_t *restrict dest,
                             const mrln_smap_t *restrict src,
                             mrln_aloctr_t *a) {
  ASSUME(dest);
  ASSUME(src);
  ASSUME(is_well_formed(src));
  ASSUME(a);

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
  mrln_smap_clear(dest);
  dest->len = src->len;
  memcpy(dest->_ctrl, src->_ctrl, src->_chnksz);
  return 0;
}

NONNULL(1, 2) void mrln_smap_destroy(mrln_smap_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  (void)mrln_alloc(a, (void **)&t->_ctrl, &t->_chnksz, 32, 0);
  *t = (mrln_smap_t){};
}

NONNULL(1)
CONST_FUNC NODISCARD intptr_t mrln_smap_find(const mrln_smap_t *t,
                                             const mrln_str8view_t key) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  if (t->len == 0) {
    return -1;
  }

  let h = hash(&key);
  return find(t, key, h);
}
