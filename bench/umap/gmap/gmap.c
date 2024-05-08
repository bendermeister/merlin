#include "gmap.h"
#include <asm-generic/errno-base.h>
#include <merlin/simd.h>
#include <stdlib.h>
#include <string.h>

#define GROUP_BASE 16

static u32 group_index(const gmap_group_t *g, u32 hash) {
  return hash & (g->sz - 1);
}

static u32 group_mask(const gmap_group_t *g, u32 hash) {
  let index = group_index(g, hash);

  let z = mrln_v8u32_set1(0);
  let h = mrln_v8u32_set1(hash);

  let r0 = mrln_v8u32_load_unaligned((const void *)&g->hash[index]);
  let t0 = mrln_v8u32_cmpeq(r0, z);
  let t1 = mrln_v8u32_cmpeq(r0, h);
  let t2 = mrln_v8u32_or(t0, t1);

  let r1 = mrln_v8u32_load_unaligned((const void *)&g->hash[index + 8]);
  let t3 = mrln_v8u32_cmpeq(r1, z);
  let t4 = mrln_v8u32_cmpeq(r1, h);
  let t5 = mrln_v8u32_or(t3, t4);

  var mask = mrln_v8u32_mask(t5) << 8;
  mask |= mrln_v8u32_mask(t2);
  return mask;
}

gmap_iter_t gmap_iter(gmap_t *t, u32 hash) {
  let hh = hash | 0x80000000u;

  if (!t->group) {
    return (gmap_iter_t){
        .group = &t->group,
        .hash = hh,
    };
  }

  gmap_iter_t iter = {
      .group = &t->group,
      .hash = hh,
      .index = group_index(t->group, hh) - 1,
      .mask = group_mask(t->group, hh),
  };
  return gmap_next(iter);
}

gmap_iter_t gmap_next(gmap_iter_t iter) {
  iter.index += 1;

  if (iter.mask) {
    let ctz = __builtin_ctz(iter.mask);
    iter.mask >>= ctz + 1;
    iter.index += ctz;
    iter.isset = (*iter.group)->hash[iter.index] & 0x80000000u;
    return iter;
  }

  iter.group = &(*iter.group)->next;
  if (!*iter.group) {
    iter.isset = 0;
    return iter;
  }

  iter.index = group_index(*iter.group, iter.hash) - 1;
  iter.mask = group_mask(*iter.group, iter.hash);
  return gmap_next(iter);
}

static int group_insert(gmap_t *t, gmap_iter_t *iter) {
  t->max_group_sz = MAX((intptr_t)GROUP_BASE, t->max_group_sz * 2);
  *iter->group = malloc(sizeof(**iter->group));
  if (UNLIKELY(!*iter->group)) {
    return ENOMEM;
  }
  var g = *iter->group;
  g->sz = t->max_group_sz;
  g->next = NULL;
  g->hash = malloc((t->max_group_sz + GROUP_BASE) * sizeof(*g->hash));
  g->key = malloc((t->max_group_sz + GROUP_BASE) * sizeof(*g->key));
  g->val = malloc((t->max_group_sz + GROUP_BASE) * sizeof(*g->val));
  if (UNLIKELY(!g->hash || !g->val || !g->key)) {
    free(g->hash);
    free(g->key);
    free(g->val);
    free(g);
    *iter->group = NULL;
    return ENOMEM;
  }

  memset(g->hash, 0, (g->sz + GROUP_BASE) * sizeof(*g->hash));

  iter->index = group_index(g, iter->hash);
  g->hash[iter->index] = iter->hash;
  iter->isset = 1;
  return 0;
}

int gmap_insert(gmap_t *t, gmap_iter_t *iter) {
  if (!*iter->group) {
    return group_insert(t, iter);
  }

  var g = *iter->group;
  g->hash[iter->index] = iter->hash;
  iter->isset = 1;
  return 0;
}

static void group_destroy(gmap_group_t *g) {
  if (!g) {
    return;
  }
  group_destroy(g->next);
  free(g->hash);
  free(g->key);
  free(g->val);
  free(g);
}

void gmap_destroy(gmap_t *t) { group_destroy(t->group); }

uintptr_t *gmap_key(gmap_iter_t iter) {
  let group = *iter.group;
  return &group->key[iter.index];
}

uintptr_t *gmap_val(gmap_iter_t iter) {
  let group = *iter.group;
  return &group->val[iter.index];
}
