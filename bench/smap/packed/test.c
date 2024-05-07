#include <broken/unit.h>
#include <merlin/smap.h>
#include <merlin/str8.h>
#include <string.h>

#include "packed.h"

static void lazy_error(int err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }
}

static test_t TEST_smap_long(void) {
  test_t test = TEST_MAKE();

  const intptr_t NUM = 100;
  const intptr_t max_word_len = 32;

  char *words = malloc(max_word_len * 2 * NUM);
  lazy_error(words == NULL);

  intptr_t words_len = 0;

  mrln_str8view_t *key = malloc(2 * NUM * sizeof(*key));
  lazy_error(key == NULL);

  for (intptr_t i = 0; i < NUM + NUM; ++i) {
    snprintf(words + words_len, max_word_len, "asdfasdfasdfasdf%ld", i);
    key[i] = mrln_str8view(words + words_len);
    words_len += key[i].length;
  }

  packed_t m = {};

  int err;

  mrln_aloctr_t *a = mrln_aloctr_global();

  err = packed(&m, 8, a);
  lazy_error(err);

  for (uintptr_t i = 0; i < NUM; ++i) {
    err = packed_insert(&m, &key[i], i, a);
    lazy_error(err);
  }

  TEST_INT(&test, m.len, NUM, NULL);

  for (uintptr_t i = 0; i < NUM; ++i) {
    const intptr_t j = packed_find(&m, &key[i]);
    const bool isset = packed_isset(&m, j);
    TEST_BOOL(&test, isset, 1, NULL);
    TEST_UINT(&test, m.val[j], i, NULL);
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = packed_upsert(&m, &key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, i, NULL);
  }

  for (uintptr_t i = NUM; i < NUM + NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = packed_upsert(&m, &key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, NUM + 69, NULL);
  }

  for (uintptr_t i = 0; i < NUM + NUM; ++i) {
    const bool c = packed_contains(&m, &key[i]);
    TEST_BOOL(&test, c, true, NULL);
  }

  TEST_INT(&test, m.len, NUM + NUM, NULL);

  packed_destroy(&m, a);

  free(words);
  free(key);

  return test;
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

  packed_t m = {};

  int err;

  mrln_aloctr_t *a = mrln_aloctr_global();

  err = packed(&m, 8, a);
  lazy_error(err);

  for (uintptr_t i = 0; i < NUM; ++i) {
    err = packed_insert(&m, &key[i], i, a);
    lazy_error(err);
  }

  TEST_INT(&test, m.len, NUM, NULL);

  for (uintptr_t i = 0; i < NUM; ++i) {
    const intptr_t j = packed_find(&m, &key[i]);
    const bool isset = packed_isset(&m, j);
    TEST_BOOL(&test, isset, 1, NULL);
    TEST_UINT(&test, m.val[j], i, NULL);
  }

  for (uintptr_t i = 0; i < NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = packed_upsert(&m, &key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, i, NULL);
  }

  for (uintptr_t i = NUM; i < NUM + NUM; ++i) {
    uintptr_t val = NUM + 69;
    err = packed_upsert(&m, &key[i], &val, a);
    lazy_error(err);
    TEST_UINT(&test, val, NUM + 69, NULL);
  }

  for (uintptr_t i = 0; i < NUM + NUM; ++i) {
    const bool c = packed_contains(&m, &key[i]);
    TEST_BOOL(&test, c, true, NULL);
  }

  TEST_INT(&test, m.len, NUM + NUM, NULL);

  packed_destroy(&m, a);

  free(words);
  free(key);

  return test;
}

int main(void) {
  TEST_RUN(TEST_smap());
  TEST_RUN(TEST_smap_long());
  return TEST_CLEANUP();
}
