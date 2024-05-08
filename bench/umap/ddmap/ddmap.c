#include "ddmap.h"
#include <merlin/simd.h>
#include <stdint.h>
#include <string.h>

#include "../../../src/util.h"

#define ISSET 0x80
#define TOMB 1

CONST_FUNC AINLINE static u8 hash_to_ctrl(const u64 h) {
  return h >> 56 | ISSET;
}

CONST_FUNC AINLINE NONNULL(1, 2) uint8_t *keybufat(const ddmap_t *t,
                                                   const ddmap_vt_t *vt,
                                                   const i32 i) {
  ASSUME(t);
  ASSUME(t->ctrl);
  ASSUME(t->bufsz);
  return t->ctrl + t->bufsz + 32 + vt->keysz * i;
}

CONST_FUNC AINLINE NONNULL(1, 2) uint8_t *valbufat(const ddmap_t *t,
                                                   const ddmap_vt_t *vt,
                                                   const i32 i) {
  ASSUME(t);
  ASSUME(vt);
  ASSUME(t->ctrl);
  ASSUME(t->bufsz);
  return t->ctrl + t->bufsz + 32 + vt->keysz * t->bufsz + i * vt->valsz;
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

CONST_FUNC NONNULL(1, 2, 3) static i32
    find(const ddmap_t *t, const ddmap_vt_t *vt, const void *restrict key,
         u64 h) {

  let mod = t->bufsz - 1;
  let ctrl = mrln_v32u8_set1(hash_to_ctrl(h));
  let zero = mrln_v32u8_set1(0);
  i32 i = h & (u64)mod;

  // TODO: do we need this for optimization
  let keybuf = keybufat(t, vt, 0);
  let keysz = vt->keysz;

  let iseq = vt->iseq;

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

CONST_FUNC AINLINE NONNULL(1) static i32 find_empty(const ddmap_t *t, u64 h) {
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

NODISCARD NONNULL(1, 2,
                  4) static int rehash_to(ddmap_t *t, const ddmap_vt_t *vt,
                                          const i32 bufsz, mrln_aloctr_t *a) {

  let hash = vt->hash;
  let ctrl = t->ctrl;
  let end = t->bufsz;

  ddmap_t new = {
      .bufsz = bufsz,
      .len = t->len,
  };

  {
    intptr_t chnksz = 0;
    let err = mrln_alloc(a, (void **)&new.ctrl, &chnksz, 0,
                         bufsz_to_chnksz(bufsz, vt->keysz, vt->valsz));
    if (UNLIKELY(err)) {
      return err;
    }
    memset(new.ctrl, 0, bufsz + 32);
  }

  let restrict keybuf = keybufat(t, vt, 0);
  let restrict valbuf = valbufat(t, vt, 0);
  i32 keyi = 0;
  i32 vali = 0;
  let keysz = vt->keysz;
  let valsz = vt->valsz;

  let restrict new_keybuf = (void *)keybufat(&new, vt, 0);
  let restrict new_valbuf = (void *)valbufat(&new, vt, 0);

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

NODISCARD NONNULL(1, 2) static int grow_if(ddmap_t *t, const ddmap_vt_t *vt,
                                           mrln_aloctr_t *a) {
  let should = t->len + t->tomb >= (t->bufsz * 7) / 8;
  if (should) {
    return rehash_to(t, vt, t->bufsz * 2, a);
  }
  return 0;
}

NODISCARD NONNULL(1, 2, 4) int ddmap(ddmap_t *t, const ddmap_vt_t *vt,
                                     const i32 cap, mrln_aloctr_t *a) {
  ASSUME(t);
  ASSUME(a);

  t->bufsz = cap_to_bufsz(cap);
  let new_chnksz = bufsz_to_chnksz(t->bufsz, vt->keysz, vt->valsz);
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

NONNULL(1, 2) void ddmap_destroy(ddmap_t *t, mrln_aloctr_t *a) {
  intptr_t chnksz = t->chnksz;
  (void)mrln_alloc(a, (void **)&t->ctrl, &chnksz, 0, 0);
  t->ctrl = NULL;
  t->chnksz = 0;
}

CONST_FUNC NONNULL(1, 2, 3) i32
    ddmap_find(const ddmap_t *t, const ddmap_vt_t *vt,
               const void *restrict key) {
  let h = vt->hash(key);
  return find(t, vt, key, h);
}

CONST_FUNC NONNULL(1, 2, 3) bool ddmap_contains(const ddmap_t *t,
                                                const ddmap_vt_t *vt,
                                                const void *restrict key) {
  let i = ddmap_find(t, vt, key);
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1) bool ddmap_isset(const ddmap_t *t, const i32 i) {
  return t->ctrl[i] & ISSET;
}

CONST_FUNC NONNULL(1, 2) void *ddmap_key(const ddmap_t *t, const ddmap_vt_t *vt,
                                         const i32 i) {
  return keybufat(t, vt, i);
}

CONST_FUNC NONNULL(1, 2) void *ddmap_val(const ddmap_t *t, const ddmap_vt_t *vt,
                                         const i32 i) {
  return valbufat(t, vt, i);
}

NONNULL(1, 2, 3, 4, 5)
NODISCARD int ddmap_insert(ddmap_t *t, const ddmap_vt_t *vt,
                           const void *restrict key, const void *restrict val,
                           mrln_aloctr_t *a) {
  let h = vt->hash(key);

  let err = grow_if(t, vt, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let i = find(t, vt, key, h);

  if (t->ctrl[i] & ISSET) {
    memcpy(valbufat(t, vt, i), val, vt->valsz);
    return 0;
  }

  t->len += 1;

  let hh = hash_to_ctrl(h);

  t->ctrl[i] = hh;
  if (i < 32) {
    t->ctrl[i + t->bufsz] = hh;
  }

  memcpy(keybufat(t, vt, i), key, vt->keysz);
  memcpy(valbufat(t, vt, i), val, vt->valsz);
  return 0;
}

NONNULL(1, 2, 3, 4, 5)
NODISCARD int ddmap_upsert(ddmap_t *t, const ddmap_vt_t *vt,
                           const void *restrict key, void *restrict val,
                           mrln_aloctr_t *a) {
  let h = vt->hash(key);

  let err = grow_if(t, vt, a);
  if (UNLIKELY(err)) {
    return err;
  }

  let i = find(t, vt, key, h);
  if (!t->ctrl[i]) {
    t->len += 1;
    let hh = hash_to_ctrl(h);
    t->ctrl[i] = hh;
    if (i < 32) {
      t->ctrl[i + t->bufsz] = hh;
    }
    memcpy(keybufat(t, vt, i), key, vt->keysz);
    memcpy(valbufat(t, vt, i), val, vt->valsz);
  }
  memcpy(val, valbufat(t, vt, i), vt->valsz);
  return 0;
}

NONNULL(1, 2, 4)
NODISCARD int ddmap_reserve(ddmap_t *t, const ddmap_vt_t *vt, const i32 cap,
                            mrln_aloctr_t *a) {
  return rehash_to(t, vt, cap_to_bufsz(cap), a);
}
