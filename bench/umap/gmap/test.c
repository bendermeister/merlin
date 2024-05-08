#include "gmap.h"
#include <broken/unit.h>
#include <string.h>

static void lazy_error(uintptr_t err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }
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

static test_t TEST_map(void) {
  test_t test = TEST_MAKE();

  uintptr_t err;

  const uintptr_t NUM = 75;

  gmap_t m = {};

  for (uintptr_t i = 0; i < NUM; ++i) {
    gmap_iter_t iter = gmap_iter(&m, uptr_hash(&i));

    for (; iter.isset; iter = gmap_next(iter)) {
    }

    err = gmap_insert(&m, &iter);
    lazy_error(err);

    *(uintptr_t *)gmap_key(iter) = i;
    *(uintptr_t *)gmap_val(iter) = i;
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    gmap_iter_t iter = gmap_iter(&m, uptr_hash(&i));
    for (; iter.isset && *gmap_key(iter) != i; iter = gmap_next(iter)) {
    }

    TEST_BOOL(&test, iter.isset, true, TEST_MAKE_STR("i: %ld", i));

    const intptr_t k = *(uintptr_t *)gmap_key(iter);
    const intptr_t v = *(uintptr_t *)gmap_key(iter);
    TEST_UINT(&test, k, i, TEST_MAKE_STR("i: %ld", i));
    TEST_UINT(&test, v, i, TEST_MAKE_STR("i: %ld", i));
  }

  gmap_destroy(&m);

  return test;
}

int main(void) {
  TEST_RUN(TEST_map());
  return TEST_CLEANUP();
}
