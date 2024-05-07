#include "dmap.h"
#include <merlin/simd.h>
#include <stdint.h>
#include <string.h>

#define ISSET 0x80
#define TOMB 1

CONST_FUNC AINLINE static u8 hash_to_ctrl(const u64 h) {
  return h >> 56 | ISSET;
}

CONST_FUNC AINLINE NONNULL(1) uint8_t *keybufat(const dmap_t *t, const i32 i) {
  ASSUME(t);
  ASSUME(t->ctrl);
  ASSUME(t->bufsz);
  return t->ctrl + t->bufsz + 32 + t->keysz * i;
}

CONST_FUNC AINLINE NONNULL(1) uint8_t *valbufat(const dmap_t *t, const i32 i) {
  ASSUME(t);
  ASSUME(t->ctrl);
  ASSUME(t->bufsz);
  return t->ctrl + t->bufsz + 32 + t->keysz * t->bufsz + i * t->valsz;
}

CONST_FUNC AINLINE i32 cap_to_bufsz(const i32 bufsz) {
  let t0 = (bufsz * 8) / 7;
  let t1 = MAX(t0, 16);
  return 1 << (32 - __builtin_clz(t1));
}

CONST_FUNC AINLINE i32 bufsz_to_chnksz(const i32 bufsz, const i16 keysz,
                                       const i16 valsz) {
  return bufsz + 32 + bufsz * keysz + bufsz * valsz;
}

CONST_FUNC NONNULL(1, 2) static i32
    find(const dmap_t *__attribute__((noescape)) t, const void *key, u64 h) {
  let mod = t->bufsz - 1;
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  let zero = mrln_v32u8_set1(0);
  i32 i = h & (u64)mod;

  // TODO: do we need this for optimization
  let keybuf = keybufat(t, 0);
  let keysz = t->keysz;

  let iseq = t->iseq;

  for (i32 o = 32;; ++o, i = (i + o) & mod) {
    let r = mrln_v32u8_load_unaligned((const void *)&t->ctrl[i]);
    let t0 = mrln_v32u8_cmpeq(r, ctrl);
    var m = mrln_v32u8_mask(t0);

    for (i32 j = 0; m; ++j, m >>= 1) {
      let ctz = __builtin_ctz(m);
      j += ctz;
      m >>= ctz;
      let l = (i + j) & mod;
      if (iseq(key, &keybuf[l * keysz])) {
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

CONST_FUNC AINLINE NONNULL(1) static i32 find_empty(const dmap_t *t, u64 h) {
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

NODISCARD NONNULL(1, 3) static int rehash_to(dmap_t *t, const i32 bufsz,
                                             mrln_aloctr_t *a) {

  let hash = t->hash;
  let ctrl = t->ctrl;
  let end = t->bufsz;

  dmap_t new = {
      .keysz = t->keysz,
      .valsz = t->valsz,

      .bufsz = bufsz,

      .hash = t->hash,
      .iseq = t->iseq,
      .len = t->len,
  };

  {
    intptr_t chnksz = 0;
    let err = mrln_alloc(a, (void **)&new.ctrl, &chnksz, 0,
                         bufsz_to_chnksz(bufsz, t->keysz, t->valsz));
    if (UNLIKELY(err)) {
      return err;
    }
    memset(new.ctrl, 0, bufsz + 32);
  }

  let restrict keybuf = keybufat(t, 0);
  let restrict valbuf = valbufat(t, 0);
  i32 keyi = 0;
  i32 vali = 0;
  let keysz = t->keysz;
  let valsz = t->valsz;

  let restrict new_keybuf = (void *)keybufat(&new, 0);
  let restrict new_valbuf = (void *)valbufat(&new, 0);

  for (i32 i = 0; i < end; ++i, keyi += keysz, vali += valsz) {
    let hh = ctrl[i];
    if (!(hh & ISSET)) {
      continue;
    }
    let h = hash(keybuf + keyi);
    let j = find_empty(&new, h);
    new.ctrl[j] = hh;
    if (j < 32) {
      new.ctrl[j + new.bufsz] = hh;
    }
    memcpy(&new_keybuf[j * keysz], keybuf + keyi, keysz);
    memcpy(&new_valbuf[j * valsz], valbuf + vali, valsz);
  }

  // dealloc old buffer
  {
    intptr_t chnksz = t->chnksz;
    (void)mrln_alloc(a, (void **)&t->ctrl, &chnksz, 0, 0);
    t->ctrl = new.ctrl;
    t->tomb = 0;
    t->bufsz = bufsz;
    t->chnksz = new.chnksz;
  }
  return 0;
}

NODISCARD NONNULL(1, 2) static int grow_if(dmap_t *t, mrln_aloctr_t *a) {
  let should = t->len + t->tomb >= (t->bufsz * 7) / 8;
  if (should) {
    return rehash_to(t, t->bufsz * 2, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 5) int dmap(dmap_t *t, const i32 cap, const u16 keysz,
                                 const u16 valsz, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(keysz);
  ASSUME(a);

  t->bufsz = cap_to_bufsz(cap);
  let new_chnksz = bufsz_to_chnksz(t->bufsz, keysz, valsz);
  t->len = 0;
  t->tomb = 0;
  t->keysz = keysz;
  t->valsz = valsz;

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

NONNULL(1, 2) void dmap_destroy(dmap_t *t, mrln_aloctr_t *a) {
  intptr_t chnksz = t->chnksz;
  (void)mrln_alloc(a, (void **)&t->ctrl, &chnksz, 0, 0);
  t->ctrl = NULL;
  t->chnksz = 0;
}

CONST_FUNC NONNULL(1, 2) i32 dmap_find(const dmap_t *t, const void *key) {
  let h = t->hash(key);
  return find(t, key, h);
}

CONST_FUNC NONNULL(1, 2) bool dmap_contains(const dmap_t *t, const void *key) {
  let i = dmap_find(t, key);
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1) bool dmap_isset(const dmap_t *t, const i32 i) {
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1) void *dmap_key(const dmap_t *t, const i32 i) {
  return keybufat(t, i);
}

CONST_FUNC NONNULL(1) void *dmap_val(const dmap_t *t, const i32 i) {
  return valbufat(t, i);
}

NONNULL(1, 2, 3, 4)
NODISCARD int dmap_insert(dmap_t *t, const void *key, const void *val,
                          mrln_aloctr_t *a) {
  let h = t->hash(key);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let i = find(t, key, h);

  if (t->ctrl[i] & ISSET) {
    memcpy(valbufat(t, i), val, t->valsz);
    return 0;
  }

  t->len += 1;

  let hh = hash_to_ctrl(h);

  t->ctrl[i] = hh;
  if (i < 32) {
    t->ctrl[i + t->bufsz] = hh;
  }

  memcpy(keybufat(t, i), key, t->keysz);
  memcpy(valbufat(t, i), val, t->valsz);
  return 0;
}

NONNULL(1, 2, 3, 4)
NODISCARD int dmap_upsert(dmap_t *t, const void *key, void *val,
                          mrln_aloctr_t *a) {
  let h = t->hash(key);

  let err = grow_if(t, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let i = find(t, key, h);
  if (!t->ctrl[i]) {
    t->len += 1;
    let hh = hash_to_ctrl(h);
    t->ctrl[i] = hh;
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hh;
    }
    memcpy(keybufat(t, i), key, t->keysz);
    memcpy(valbufat(t, i), val, t->valsz);
  }
  memcpy(val, valbufat(t, i), t->valsz);
  return 0;
}

NONNULL(1, 3)
NODISCARD int dmap_reserve(dmap_t *t, const i32 cap, mrln_aloctr_t *a) {
  return rehash_to(t, cap_to_bufsz(cap), a);
}
