#include <broken/unit.h>
#include <merlin/smap.h>
#include <merlin/str8.h>
#include <string.h>

static void lazy_error(int err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }
}

static test_t TEST_smap(void) {
  test_t test = TEST_MAKE();

  const intptr_t NUM = 100;
  const intptr_t max_word_len = 4;

  char *words = malloc(max_word_len * 2 * NUM);
  lazy_error(words == NULL);

  intptr_t words_len = 0;

  mrln_str8view_t *key = malloc(2 * NUM * sizeof(*key));
  lazy_error(key == NULL);

  for (intptr_t i = 0; i < NUM + NUM; ++i) {
    snprintf(words + words_len, max_word_len, "%ld", i);
    key[i] = mrln_str8view(words + words_len);
    words_len += key[i].length;
  }

  mrln_smap_t m = {};

  int err;

  mrln_aloctr_t *a = mrln_aloctr_global();

  err = mrln_smap(&m, 8, a);
  lazy_error(err);

  for (uintptr_t i = 0; i < NUM; ++i) {
    err = mrln_smap_insert(&m, key[i], i, a);
    lazy_error(err);
  }

  TEST_INT(&test, m.len, NUM, NULL);

  for (uintptr_t i = 0; i < NUM; ++i) {
    const intptr_t j = mrln_smap_find(&m, key[i]);
    const bool isset = mrln_smap_isset(&m, j);
    TEST_BOOL(&test, isset, 1, NULL);
    TEST_UINT(&test, m.val[j], i, NULL);
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = mrln_smap_upsert(&m, key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, i, NULL);
  }

  for (uintptr_t i = NUM; i < NUM + NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = mrln_smap_upsert(&m, key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, NUM + 69, NULL);
  }

  for (uintptr_t i = 0; i < NUM + NUM; ++i) {
    const bool c = mrln_smap_contains(&m, key[i]);
    TEST_BOOL(&test, c, true, NULL);
  }

  TEST_INT(&test, m.len, NUM + NUM, NULL);

  mrln_smap_destroy(&m, a);

  free(words);
  free(key);

  return test;
}

int main(void) {
  TEST_RUN(TEST_smap());
  return TEST_CLEANUP();
}
