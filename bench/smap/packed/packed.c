#include "packed.h"
#include "../../../src/util.h"
#include <merlin/simd.h>
#include <stddef.h>
#include <string.h>

struct packed_key_t {
  i64 len;
  const uint8_t *buf;
};

CONST_FUNC NODISCARD
NONNULL(1) static bool is_inplace(const packed_key_t *key) {
  return key->len & (1ull << 63);
}

CONST_FUNC NODISCARD NONNULL(1) static packed_key_t
    str8_inplace(const mrln_str8view_t *v) {
  packed_key_t key = {};
  key.len |= (1ull << 63);
  uint8_t *dest = (uint8_t *)&key;
  dest += 1;
  memcpy(dest, v->buffer, v->length);
  return key;
}

CONST_FUNC NODISCARD NONNULL(1) static packed_key_t
    str8_to_packed(const mrln_str8view_t *v) {
  return (packed_key_t){.len = v->length, .buf = v->buffer};
}

CONST_FUNC NODISCARD NONNULL(1) static i64 keylen(const packed_key_t *key) {
  let bitlen = (key->len >> 56) & 0x7F;
  return (key->len << 8) >> (64 - bitlen);
}

CONST_FUNC NODISCARD static intptr_t cap_to_bufsz(const intptr_t cap) {
  var t = (cap * 8) / 7;
  t = MAX(t, 16);
  return 1 << (sizeof(long long) * 8 - __builtin_clzll(t));
}

CONST_FUNC NODISCARD static i64 bufsz_to_chnksz(const i64 bufsz) {
  return bufsz * sizeof(uint8_t) + bufsz * sizeof(uintptr_t) +
         bufsz * sizeof(packed_key_t) + 32;
}

CONST_FUNC NODISCARD NONNULL(1) static packed_key_t *keybuf(const packed_t *t) {
  return (packed_key_t *)(t->ctrl + t->bufsz + 32);
}

CONST_FUNC NODISCARD NONNULL(1) static uintptr_t *valbuf(const packed_t *t) {
  return (uintptr_t *)(keybuf(t) + t->bufsz);
}

CONST_FUNC NODISCARD NONNULL(1) static bool is_well_formed(const packed_t *t) {
  // is empty initialised
  {
    let b0 = t->bufsz == 0;
    let b1 = t->ctrl == NULL;
    let b2 = t->key == NULL;
    let b3 = t->len == 0;
    let b4 = t->val == NULL;
    let b5 = t->chnksz == 0;
    let b6 = t->tomb == 0;
    if (b0 && b1 && b2 && b3 && b4 && b5 && b6) {
      return true;
    }
  }

  // check if initialised correctly
  {
    let b0 = t->bufsz >= 32 && __builtin_popcountll(t->bufsz) == 1;
    let b1 = t->chnksz >= bufsz_to_chnksz(t->bufsz);
    let b2 = t->ctrl != NULL && t->key != NULL && t->val != NULL;
    let b3 = (void *)t->key < (void *)(t->ctrl + t->chnksz);
    let b4 = (void *)t->val < (void *)(t->ctrl + t->chnksz);
    let b5 = t->key == keybuf(t);
    let b6 = t->val == valbuf(t);
    let b7 = t->len >= 0 && t->tomb >= 0;
    return b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7;
  }
}

#define ISSET ((u8)0x80)
#define TOMB 1

CONST_FUNC NODISCARD static u8 hash_to_ctrl(const u64 hash) {
  return (hash >> 56) | ISSET;
}

CONST_FUNC NODISCARD static u64 hash_inplace(const packed_key_t *s) {

  let len = 16;
  let iter = (uint8_t *)s;
  // fnv-1 hash with primes from wikipedia
  u64 h = 0xcbf29ce484222325ull;
  for (intptr_t i = 0; i < len; ++i) {
    h ^= iter[i];
    h *= 0x00000100000001B3;
  }
  return h;
}

CONST_FUNC NODISCARD static u64 hash_str(const packed_key_t *s) {

  let len = keylen(s);
  let iter = s->buf;
  // fnv-1 hash with primes from wikipedia
  u64 h = 0xcbf29ce484222325ull;
  for (intptr_t i = 0; i < len; ++i) {
    h ^= iter[i];
    h *= 0x00000100000001B3;
  }
  return h;
}

CONST_FUNC NODISCARD NONNULL(1) static intptr_t
    find_empty(const packed_t *t, const u64 h) {
  let mod = t->bufsz - 1;
  intptr_t i = h & mod;
  let zero = mrln_v32u8_set1(0);

  for (intptr_t o = 32;; ++o, i = (i + o) & mod) {
    let row = mrln_v32u8_load_unaligned((void *)(t->ctrl + i));
    let t = mrln_v32u8_cmpeq(row, zero);
    var m = mrln_v32u8_mask(t);

    if (m) {
      let ctz = __builtin_ctz(m);
      return (i + ctz) & mod;
    }
  }
}

CONST_FUNC NODISCARD NONNULL() static intptr_t
    find_inplace(const packed_t *t, const packed_key_t *key, const u64 h) {
  let mod = t->bufsz - 1;
  let zero = mrln_v32u8_set1(0);
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  intptr_t i = h & mod;

  for (intptr_t o = 32;; ++o, i = (i + o) & mod) {
    let r = mrln_v32u8_load_unaligned((void *)(t->ctrl + i));
    let t0 = mrln_v32u8_cmpeq(r, zero);
    let t1 = mrln_v32u8_cmpeq(r, ctrl);
    let t2 = mrln_v32u8_or(t0, t1);
    var m = mrln_v32u8_mask(t2);

    for (intptr_t j = 0; m; ++j, m >>= 1) {
      let ctz = __builtin_ctz(m);
      j += ctz;
      m >>= ctz;
      let l = (i + j) & mod;
      let c = t->ctrl[l] == 0 || __builtin_memcmp(&t->key[l], key, 16) == 0;
      if (c) {
        return l;
      }
    }
  }
}

CONST_FUNC NODISCARD NONNULL() static intptr_t
    find_str(const packed_t *t, const packed_key_t *key, const u64 h) {
  let mod = t->bufsz - 1;
  let zero = mrln_v32u8_set1(0);
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  intptr_t i = h & mod;

  for (intptr_t o = 32;; ++o, i = (i + o) & mod) {
    let r = mrln_v32u8_load_unaligned((void *)(t->ctrl + i));
    let t0 = mrln_v32u8_cmpeq(r, zero);
    let t1 = mrln_v32u8_cmpeq(r, ctrl);
    let t2 = mrln_v32u8_or(t0, t1);
    var m = mrln_v32u8_mask(t2);

    for (intptr_t j = 0; m; ++j, m >>= 1) {
      let ctz = __builtin_ctz(m);
      j += ctz;
      m >>= ctz;
      let l = (i + j) & mod;
      let c =
          t->ctrl[l] == 0 || (t->key[l].len == key->len &&
                              memcmp(t->key[l].buf, key->buf, key->len) == 0);
      if (c) {
        return l;
      }
    }
  }
}

NONNULL(1, 3)
NODISCARD static int rehash_to(packed_t *t, const intptr_t bufsz,
                               mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  packed_t new = {.bufsz = bufsz};
  let err = mrln_alloc(a, (void **)&new.ctrl, &new.chnksz, 32,
                       bufsz_to_chnksz(new.bufsz));
  if (UNLIKELY(err)) {
    return err;
  }

  new.key = keybuf(&new);
  new.val = valbuf(&new);
  packed_clear(&new);
  new.len = t->len;

  for (intptr_t i = 0; i < t->bufsz; ++i) {
    if (!(t->ctrl[i] & ISSET)) {
      continue;
    }
    u64 h;
    if (is_inplace(&t->key[i])) {
      h = hash_inplace(&t->key[i]);
    } else {
      h = hash_str(&t->key[i]);
    }
    let j = find_empty(&new, h);
    new.ctrl[j] = t->ctrl[i];
    if (j < 32) {
      new.ctrl[j + new.bufsz] = t->ctrl[i];
    }
    new.key[j] = t->key[i];
    new.val[j] = t->val[i];
  }

  packed_destroy(t, a);
  *t = new;

  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(packed_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let should = t->tomb + t->len >= (t->bufsz * 7) / 8;
  if (should) {
    const intptr_t bufsz = MAX(32, t->bufsz * 2);
    return rehash_to(t, bufsz, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 3) int packed(packed_t *t, const intptr_t capacity,
                                   mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let bufsz = cap_to_bufsz(capacity);
  if (bufsz <= t->bufsz) {
    packed_clear(t);
    return 0;
  }
  t->bufsz = bufsz;
  let chnksz = bufsz_to_chnksz(t->bufsz);
  let err = mrln_alloc(a, (void **)&t->ctrl, &t->chnksz, 32, chnksz);
  if (UNLIKELY(err)) {
    return err;
  }
  t->key = keybuf(t);
  t->val = valbuf(t);
  packed_clear(t);
  return 0;
}

NONNULL(1, 3)
NODISCARD int packed_reserve(packed_t *t, const intptr_t capacity,
                             mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let bufsz = cap_to_bufsz(capacity);
  if (t->bufsz >= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int packed_shrink(packed_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  if (!t->ctrl) {
    return 0;
  }

  let bufsz = cap_to_bufsz(t->len);
  if (t->bufsz <= bufsz) {
    return 0;
  }
  return rehash_to(t, bufsz, a);
}

NONNULL(1, 2) NODISCARD int packed_rehash(packed_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  return rehash_to(t, t->bufsz, a);
}

NONNULL(1)
NODISCARD CONST_FUNC bool packed_contains(const packed_t *t,
                                          const mrln_str8view_t *key) {
  ASSUME(t);
  ASSUME(is_well_formed(t));

  if (t->len == 0) {
    return false;
  }

  let i = packed_find(t, key);
  return t->ctrl[i] & ISSET;
}

NONNULL(1)
NODISCARD static int upsert_inplace(packed_t *t, const mrln_str8view_t *key,
                                    uint64_t *val) {
  let pkey = str8_inplace(key);
  let h = hash_inplace(&pkey);
  let i = find_inplace(t, &pkey, h);

  if (!(t->ctrl[i] & ISSET)) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hash_to_ctrl(h);
    }
    t->key[i] = pkey;
    t->val[i] = *val;
  }
  *val = t->val[i];
  return 0;
}

NONNULL(1)
NODISCARD static int upsert_str(packed_t *t, const mrln_str8view_t *key,
                                uint64_t *val) {
  let pkey = str8_to_packed(key);
  let h = hash_str(&pkey);
  let i = find_str(t, &pkey, h);

  if (!(t->ctrl[i] & ISSET)) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hash_to_ctrl(h);
    }
    t->key[i] = pkey;
    t->val[i] = *val;
  }
  *val = t->val[i];

  return 0;
}

NONNULL(1)
NODISCARD static int insert_inplace(packed_t *t, const mrln_str8view_t *key,
                                    uint64_t val) {
  let pkey = str8_inplace(key);
  let h = hash_inplace(&pkey);
  let i = find_inplace(t, &pkey, h);

  if (!(t->ctrl[i] & ISSET)) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hash_to_ctrl(h);
    }
    t->key[i] = pkey;
  }

  t->val[i] = val;
  return 0;
}

NONNULL(1)
NODISCARD static int insert_str(packed_t *t, const mrln_str8view_t *key,
                                uint64_t val) {
  let pkey = str8_to_packed(key);
  let h = hash_str(&pkey);
  let i = find_str(t, &pkey, h);

  if (!(t->ctrl[i] & ISSET)) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hash_to_ctrl(h);
    }
    t->key[i] = pkey;
  }

  t->val[i] = val;
  return 0;
}

NONNULL(1, 4)
NODISCARD int packed_insert(packed_t *t, const mrln_str8view_t *key,
                            uint64_t val, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  if (key->length < 16) {
    return insert_inplace(t, key, val);
  } else {
    return insert_str(t, key, val);
  }
}

NONNULL(1, 3, 4)
NODISCARD int packed_upsert(packed_t *t, const mrln_str8view_t *key,
                            uint64_t *val, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(val);
  ASSUME(a);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  if (key->length < 16) {
    return upsert_inplace(t, key, val);
  } else {
    return upsert_str(t, key, val);
  }
}

NONNULL(1, 3)
NODISCARD int packed_remove(packed_t *t, const uint64_t key, mrln_aloctr_t *a) {
  UNUSED(key);
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);
  return -1;
}

NONNULL(1)
void packed_clear(packed_t *t) {
  ASSUME(t);
  ASSUME(is_well_formed(t));

  if (t->ctrl) {
    memset(t->ctrl, 0, t->bufsz + 32);
    t->len = 0;
    t->tomb = 0;
  }
}

CONST_FUNC NONNULL(1) bool packed_isset(packed_t *t, intptr_t i) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  if (i < 0) {
    return false;
  }
  return t->ctrl[i] & ISSET;
}

NONNULL(1, 2, 3);
NODISCARD int packed_copy(packed_t *restrict dest, const packed_t *restrict src,
                          mrln_aloctr_t *a) {
  ASSUME(dest);
  ASSUME(src);
  ASSUME(is_well_formed(src));
  ASSUME(a);

  return -1;
}

NONNULL(1, 2) void packed_destroy(packed_t *t, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  ASSUME(a);

  (void)mrln_alloc(a, (void **)&t->ctrl, &t->chnksz, 32, 0);
  *t = (packed_t){};
}

NONNULL(1)
CONST_FUNC NODISCARD intptr_t packed_find(const packed_t *t,
                                          const mrln_str8view_t *key) {
  ASSUME(t);
  ASSUME(is_well_formed(t));
  if (t->len == 0) {
    return -1;
  }

  if (key->length < 16) {
    let pkey = str8_inplace(key);
    let h = hash_inplace(&pkey);
    return find_inplace(t, &pkey, h);
  } else {
    let pkey = str8_to_packed(key);
    let h = hash_str(&pkey);
    return find_str(t, &pkey, h);
  }
}
