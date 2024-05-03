#include <broken/unit.h>
#include <merlin/umap.h>
#include <string.h>

static void lazy_error(uintptr_t err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }
}

static test_t TEST_map(void) {
  test_t test = TEST_MAKE();

  mrln_aloctr_t *a = mrln_aloctr_global();
  uintptr_t err;

  const uintptr_t NUM = 100;

  mrln_umap_t m = {};

  err = mrln_umap(&m, 8, a);
  lazy_error(err);

  for (uintptr_t i = 0; i < NUM; ++i) {
    err = mrln_umap_insert(&m, i, i, a);
    lazy_error(err);
  }

  TEST_INT(&test, m.len, NUM, NULL);

  for (uintptr_t i = 0; i < NUM; ++i) {
    const intptr_t j = mrln_umap_find(&m, i);
    const bool isset = mrln_umap_isset(&m, j);
    TEST_BOOL(&test, isset, 1, NULL);
    TEST_UINT(&test, m.val[j], i, NULL);
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = mrln_umap_upsert(&m, i, &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, i, NULL);
  }

  for (uintptr_t i = NUM; i < NUM + NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = mrln_umap_upsert(&m, i, &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, NUM + 69, NULL);
  }

  mrln_umap_destroy(&m, a);

  return test;
}

int main(void) {
  TEST_RUN(TEST_map());
  return TEST_CLEANUP();
}
