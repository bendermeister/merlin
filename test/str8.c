#include <broken/unit.h>
#include <merlin/str8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void lazy_error(int err) {
  if (err) {
    fprintf(stderr, "ERROR: errcode: %s", strerror(err));
    exit(err);
  }
}

static test_t TEST_generic(void) {
  test_t test = TEST_MAKE();

  mrln_aloctr_t *a = mrln_aloctr_global();

  mrln_str8view_t v0 = mrln_str8view("Hello World");

  TEST_INT(&test, v0.length, 11, NULL);
  TEST_INT(&test, memcmp(v0.buffer, "Hello World", 11), 0, NULL);

  mrln_str8view_t v1 = mrln_str8view(v0);

  TEST_INT(&test, v1.length, 11, NULL);
  TEST_INT(&test, memcmp(v1.buffer, "Hello World", 11), 0, NULL);

  v1 = mrln_str8view(&v0);

  TEST_INT(&test, v1.length, 11, NULL);
  TEST_INT(&test, memcmp(v1.buffer, "Hello World", 11), 0, NULL);

  int err;
  mrln_str8_t s0, s1;
  err = mrln_str8(&s0, "Bye World", a);
  lazy_error(err);

  TEST_INT(&test, s0.length, 9, NULL);
  TEST_INT(&test, memcmp(s0.buffer, "Bye World", s0.length), 0, NULL);

  err = mrln_str8(&s1, &s0, a);
  lazy_error(err);

  TEST_INT(&test, s1.length, 9, NULL);
  TEST_INT(&test, memcmp(s1.buffer, "Bye World", s1.length), 0, NULL);
  TEST_BOOL(&test, s1.buffer == s0.buffer, 0, NULL);

  v0 = mrln_str8view(s0);
  TEST_INT(&test, v0.length, 9, NULL);
  TEST_INT(&test, memcmp(v0.buffer, "Bye World", v0.length), 0, NULL);
  TEST_BOOL(&test, v0.buffer == s0.buffer, 1, NULL);

  v0 = (mrln_str8view_t){};

  v0 = mrln_str8view(&s0);
  TEST_INT(&test, v0.length, 9, NULL);
  TEST_INT(&test, memcmp(v0.buffer, "Bye World", v0.length), 0, NULL);
  TEST_BOOL(&test, v0.buffer == s0.buffer, 1, NULL);

  mrln_str8_destroy(&s0, a);
  mrln_str8_destroy(&s1, a);

  return test;
}

static test_t TEST_concat(void) {
  test_t test = TEST_MAKE();

  mrln_str8_t str = {};

  mrln_aloctr_t *a = mrln_aloctr_global();

  mrln_str8view_t view = mrln_str8view("Hello World");
  int err = 0;

  err = mrln_str8_concat(&str, &view, a);
  lazy_error(err);

  TEST_INT(&test, memcmp(str.buffer, "Hello World", str.length), 0, NULL);

  err = mrln_str8_concat_char(&str, ' ', a);
  lazy_error(err);
  TEST_INT(&test, memcmp(str.buffer, "Hello World ", str.length), 0, NULL);

  err = mrln_str8_concat_u64(&str, 69, 0, 0, a);
  lazy_error(err);
  TEST_INT(&test, memcmp(str.buffer, "Hello World 69", str.length), 0, NULL);

  err = mrln_str8_concat_i64(&str, -1, 0, 0, a);
  lazy_error(err);

  mrln_str8_destroy(&str, a);
  return test;
}

static test_t TEST_replace(void) {
  test_t test = TEST_MAKE();

  mrln_aloctr_t *a = mrln_aloctr_global();

  int err = 0;
  mrln_str8_t str = {};
  err = mrln_str8(&str, "Hello World this is again a very nice #include string",
                  a);

  lazy_error(err);

  mrln_str8view_t target = mrln_str8view(" ");
  mrln_str8view_t replace = mrln_str8view("\t\t");

  err = mrln_str8_replace(&str, &target, &replace, a);
  lazy_error(err);

  TEST_INT(&test,
           memcmp(str.buffer,
                  "Hello\t\tWorld\t\tthis\t\tis\t\tagain\t\t"
                  "a\t\tvery\t\tnice\t\t#include\t\tstring",
                  str.length),
           0, NULL);

  err = mrln_str8_replace_n(&str, &replace, &target, 5, a);
  lazy_error(err);

  TEST_INT(&test,
           memcmp(str.buffer,
                  "Hello World this is again "
                  "a\t\tvery\t\tnice\t\t#include\t\tstring",
                  str.length),
           0, NULL);

  mrln_str8_destroy(&str, a);

  return test;
}

static test_t TEST_find(void) {
  test_t test = TEST_MAKE();
  mrln_str8view_t view = mrln_str8view(
      "Hello World this is some sentence written by the great Ben");
  mrln_str8view_t needle = mrln_str8view("great");

  size_t index = mrln_str8_find(&view, &needle);
  TEST_UINT(&test, index, 49, TEST_MAKE_STR("%.*s", 4, view.buffer + index));

  index = mrln_str8_find_char(&view, ' ');
  TEST_UINT(&test, index, 5, NULL);

  return test;
}

int main(void) {
  TEST_RUN(TEST_concat());
  TEST_RUN(TEST_find());
  TEST_RUN(TEST_replace());
  TEST_RUN(TEST_generic());
  return TEST_CLEANUP();
}
