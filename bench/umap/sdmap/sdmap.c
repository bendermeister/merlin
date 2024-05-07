#include "sdmap.h"
#include <merlin/simd.h>
#include <stdint.h>
#include <string.h>

#define ISSET 0x80
#define TOMB 1

CONST_FUNC static u8 hash_to_ctrl(const u64 h) { return h >> 56 | ISSET; }

CONST_FUNC NONNULL(1) const uint8_t *keybufat(const sdmap_t *t, const i32 i) {
  ASSUME(t);
  return t->ctrl + t->bufsz + 32 + t->vt->keysz * i;
}

CONST_FUNC NONNULL(1) const uint8_t *valbufat(const sdmap_t *t, const i32 i) {
  ASSUME(t);
  return t->ctrl + t->bufsz + 32 + t->vt->keysz * t->bufsz + i * t->vt->valsz;
}

CONST_FUNC i32 cap_to_bufsz(const i32 bufsz) {
  let t0 = (bufsz * 8) / 7;
  let t1 = MAX(t0, 16);
  return 1 << (32 - __builtin_clz(t1));
}

CONST_FUNC i32 bufsz_to_chnksz(const i32 bufsz, const i16 keysz,
                               const i16 valsz) {
  return bufsz + 32 + bufsz * keysz + bufsz * valsz;
}

CONST_FUNC NONNULL(1, 2) static i32
    find(const sdmap_t *t, const void *key, u64 h) {
  let mod = t->bufsz - 1;
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  let zero = mrln_v32u8_set1(0);
  i32 i = h & (u64)mod;

  for (i32 o = 32;; ++o, i = (i + o) & mod) {
    let r = mrln_v32u8_load_unaligned((const void *)&t->ctrl[i]);
    let t0 = mrln_v32u8_cmpeq(r, ctrl);
    var m = mrln_v32u8_mask(t0);

    for (i32 j = 0; m; ++j, m >>= 1) {
      let ctz = __builtin_ctz(m);
      j += ctz;
      m >>= ctz;
      let l = (i + j) & mod;
      if (t->vt->iseq(key, keybufat(t, l))) {
        return l;
      }
    }

    let t1 = mrln_v32u8_cmpeq(r, zero);
    m = mrln_v32u8_mask(t1);
    if (m) {
      return (i + __builtin_ctz(m)) & mod;
    }
  }
}

CONST_FUNC NONNULL(1) static i32 find_empty(const sdmap_t *t, u64 h) {
  let mod = t->bufsz - 1;
  i32 i = h & (u64)mod;
  let z = mrln_v32u8_set1(0);

  for (i32 o = 32;; ++o, i = (i + o) & mod) {
    let r = mrln_v32u8_load_unaligned((const void *)&t->ctrl[i]);
    let t = mrln_v32u8_cmpeq(r, z);
    let m = mrln_v32u8_mask(t);
    if (m) {
      return (i + __builtin_ctz(m)) & mod;
    }
  }
}

NODISCARD NONNULL(1, 3) static int rehash_to(sdmap_t *t, const i32 bufsz,
                                             mrln_aloctr_t *a) {
  sdmap_t new = {.vt = t->vt};
  // TODO don't call constructor do some inline code which doesn't need to
  // recheck everything
  let err = sdmap(&new, bufsz, a);
  if (UNLIKELY(err)) {
    return err;
  }
  new.len = t->len;
  for (i32 i = 0; i < t->bufsz; ++i) {
    if (!(t->ctrl[i] & ISSET)) {
      continue;
    }
    let h = t->vt->hash(keybufat(t, i));
    let j = find_empty(&new, h);
    new.ctrl[j] = hash_to_ctrl(h);
    if (j < 32) {
      new.ctrl[j + new.bufsz] = hash_to_ctrl(h);
    }
    memcpy((void *)keybufat(&new, j), keybufat(t, i), t->vt->keysz);
    memcpy((void *)valbufat(&new, j), valbufat(t, i), t->vt->valsz);
  }
  sdmap_destroy(t, a);
  *t = new;
  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(sdmap_t *t, mrln_aloctr_t *a) {
  let should = t->len + t->tomb >= (t->bufsz * 7) / 8;
  if (should) {
    return rehash_to(t, t->bufsz * 2, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 3) int sdmap(sdmap_t *t, const i32 cap, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(a);

  t->bufsz = cap_to_bufsz(cap);
  let new_chnksz = bufsz_to_chnksz(t->bufsz, t->vt->keysz, t->vt->valsz);
  t->len = 0;
  t->tomb = 0;

  if (t->chnksz < new_chnksz) {
    intptr_t chnksz = t->chnksz;
    let err = mrln_alloc(a, (void **)&t->ctrl, &chnksz, 0, new_chnksz);
    if (UNLIKELY(err)) {
      return err;
    }
    ASSUME(chnksz < INT32_MAX);
    t->chnksz = (i32)chnksz;
  }
  memset(t->ctrl, 0, t->bufsz + 32);
  return 0;
}

NONNULL(1, 2) void sdmap_destroy(sdmap_t *t, mrln_aloctr_t *a) {
  intptr_t chnksz = t->chnksz;
  (void)mrln_alloc(a, (void **)&t->ctrl, &chnksz, 0, 0);
  *t = (sdmap_t){};
}

CONST_FUNC NONNULL(1, 2) i32 sdmap_find(const sdmap_t *t, const void *key) {
  let h = t->vt->hash(key);
  return find(t, key, h);
}

CONST_FUNC NONNULL(1, 2) bool sdmap_contains(const sdmap_t *t,
                                             const void *key) {
  let i = sdmap_find(t, key);
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1) bool sdmap_isset(const sdmap_t *t, const i32 i) {
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1) const void *sdmap_key(const sdmap_t *t, const i32 i) {
  return keybufat(t, i);
}

CONST_FUNC NONNULL(1) const void *sdmap_val(const sdmap_t *t, const i32 i) {
  return valbufat(t, i);
}

NONNULL(1, 2, 3, 4)
NODISCARD int sdmap_insert(sdmap_t *t, const void *key, const void *val,
                           mrln_aloctr_t *a) {
  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let h = t->vt->hash(key);
  let i = find(t, key, h);

  if (t->ctrl[i] & ISSET) {
    memcpy((void *)valbufat(t, i), val, t->vt->valsz);
    return 0;
  }

  t->len += 1;

  t->ctrl[i] = hash_to_ctrl(h);
  if (i < 32) {
    t->ctrl[i + t->bufsz] = hash_to_ctrl(h);
  }

  memcpy((void *)keybufat(t, i), key, t->vt->keysz);
  memcpy((void *)valbufat(t, i), val, t->vt->valsz);
  return 0;
}
NONNULL(1, 2, 3, 4)
NODISCARD int sdmap_upsert(sdmap_t *t, const void *key, void *val,
                           mrln_aloctr_t *a) {
  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let h = t->vt->hash(key);
  let i = find(t, key, h);
  if (!t->ctrl[i]) {
    t->len += 1;
    t->ctrl[i] = hash_to_ctrl(h);
    if (i < 32) {
      t->ctrl[i + t->bufsz] = t->ctrl[i];
    }
    memcpy((void *)keybufat(t, i), key, t->vt->keysz);
    memcpy((void *)valbufat(t, i), val, t->vt->valsz);
  }
  memcpy(val, valbufat(t, i), t->vt->valsz);
  return 0;
}

NONNULL(1, 3)
NODISCARD int sdmap_reserve(sdmap_t *t, const i32 cap, mrln_aloctr_t *a) {
  return rehash_to(t, cap_to_bufsz(cap), a);
}
