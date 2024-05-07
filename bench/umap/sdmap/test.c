#include "sdmap.h"
#include <broken/unit.h>
#include <string.h>

static void lazy_error(uintptr_t err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }
}

static bool uptr_iseq(const void *a, const void *b) {
  return *(uintptr_t *)a == *(uintptr_t *)b;
}

CONST_FUNC NODISCARD static u64 uptr_hash(const void *p) {
  uintptr_t x = *(uintptr_t *)p;
  x ^= x >> 30;
  x *= 0xbf58476d1ce4e5b9U;
  x ^= x >> 27;
  x *= 0x94d049bb133111ebU;
  x ^= x >> 31;
  return x;
}

static const sdmap_vtble_t vt = {
    .hash = uptr_hash,
    .iseq = uptr_iseq,
    .keysz = 8,
    .valsz = 8,
};

static test_t TEST_map(void) {
  test_t test = TEST_MAKE();

  mrln_aloctr_t *a = mrln_aloctr_global();
  uintptr_t err;

  const uintptr_t NUM = 75;

  sdmap_t m = {.vt = &vt};

  err = sdmap(&m, 8, a);
  lazy_error(err);

  for (uintptr_t i = 0; i < NUM; ++i) {
    err = sdmap_insert(&m, &i, &i, a);
    lazy_error(err);
  }

  TEST_INT(&test, m.len, NUM, NULL);

  for (uintptr_t i = 0; i < NUM; ++i) {
    const intptr_t j = sdmap_find(&m, &i);
    const bool isset = sdmap_isset(&m, j);
    TEST_BOOL(&test, isset, 1, NULL);
    TEST_UINT(&test, *(uintptr_t *)sdmap_val(&m, j), i, NULL);
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = sdmap_upsert(&m, &i, &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, i, NULL);
  }

  for (uintptr_t i = NUM; i < NUM + NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = sdmap_upsert(&m, &i, &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, NUM + 69, TEST_MAKE_STR("i: %lu", i));
  }

  for (uintptr_t i = 0; i < NUM + NUM; ++i) {
    const bool c = sdmap_contains(&m, &i);
    TEST_BOOL(&test, c, true, NULL);
  }

  TEST_INT(&test, m.len, NUM + NUM, NULL);

  sdmap_destroy(&m, a);

  return test;
}

int main(void) {
  TEST_RUN(TEST_map());
  return TEST_CLEANUP();
}
