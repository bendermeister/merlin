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

static test_t TEST_concat(void) {
  test_t test = TEST_MAKE();

  merlin_str8_t str = {};
  merlin_str8_factory_t factory = {};

  merlin_str8_view_t view = merlin_str8_view_from_static_cstr("Hello World");
  int err = 0;

  err = merlin_str8_concat(&str, &view, &factory);
  lazy_error(err);

  TEST_INT(&test, memcmp(str.buffer, "Hello World", str.length), 0, NULL);

  err = merlin_str8_concat_char(&str, ' ', &factory);
  lazy_error(err);
  TEST_INT(&test, memcmp(str.buffer, "Hello World ", str.length), 0, NULL);

  err = merlin_str8_concat_u64(&str, 69, 0, 0, &factory);
  lazy_error(err);
  TEST_INT(&test, memcmp(str.buffer, "Hello World 69", str.length), 0, NULL);

  err = merlin_str8_concat_i64(&str, -1, 0, 0, &factory);
  lazy_error(err);

  merlin_str8_destroy(&str, &factory);
  merlin_str8_factory_destroy(&factory);
  return test;
}

static test_t TEST_replace(void) {
  test_t test = TEST_MAKE();

  merlin_str8_factory_t factory = {};
  merlin_str8_t str = {};
  int err = 0;

  err = merlin_str8_concat(
      &str,
      &merlin_str8_view_from_static_cstr(
          "Hello World this is again a very nice #include string"),
      &factory);
  lazy_error(err);

  merlin_str8_view_t target = merlin_str8_view_from_static_cstr(" ");
  merlin_str8_view_t replace = merlin_str8_view_from_static_cstr("\t\t");

  err = merlin_str8_replace(&str, &target, &replace, &factory);
  lazy_error(err);

  TEST_INT(&test,
           memcmp(str.buffer,
                  "Hello\t\tWorld\t\tthis\t\tis\t\tagain\t\t"
                  "a\t\tvery\t\tnice\t\t#include\t\tstring",
                  str.length),
           0, NULL);

  err = merlin_str8_replace_n(&str, &replace, &target, 5, &factory);
  lazy_error(err);

  TEST_INT(&test,
           memcmp(str.buffer,
                  "Hello World this is again "
                  "a\t\tvery\t\tnice\t\t#include\t\tstring",
                  str.length),
           0, NULL);

  merlin_str8_destroy(&str, &factory);
  merlin_str8_factory_destroy(&factory);

  return test;
}

static test_t TEST_find(void) {
  test_t test = TEST_MAKE();
  merlin_str8_view_t view = merlin_str8_view_from_static_cstr(
      "Hello World this is some sentence written by the great Ben");
  merlin_str8_view_t needle = merlin_str8_view_from_static_cstr("great");

  size_t index = merlin_str8_find(&view, &needle);
  TEST_UINT(&test, index, 49, TEST_MAKE_STR("%.*s", 4, view.buffer + index));

  index = merlin_str8_find_char(&view, ' ');
  TEST_UINT(&test, index, 5, NULL);

  return test;
}

int main(void) {
  TEST_RUN(TEST_concat());
  TEST_RUN(TEST_find());
  TEST_RUN(TEST_replace());
  return TEST_CLEANUP();
}
