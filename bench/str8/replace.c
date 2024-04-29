#include <bench/bench.h>
#include <errno.h>
#include <merlin/str8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void lazy_error(int err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(err);
  }
}

static merlin_str8_t get_lorem(void) {
  FILE *file = fopen("bench/str8/loremipsum.txt", "r");
  int err = errno;
  lazy_error(err);
  (void)fseek(file, 0, SEEK_END);

  merlin_str8_t str = {};
  str.capacity = ftell(file);
  str.length = str.capacity;
  rewind(file);

  str.buffer = malloc(str.capacity);
  err = errno;
  lazy_error(err);

  // TODO(ben): does this always read in one go or do we need loop?
  fread(str.buffer, str.length, 1, file);
  lazy_error(ferror(file));

  fclose(file);

  return str;
}

static bench_timer_t linear(void) {
  bench_timer_t timer = {};
  merlin_str8_t str = get_lorem();
  merlin_str8_view_t target = merlin_str8_view_from_static_cstr("est");
  merlin_str8_view_t replace =
      merlin_str8_view_from_static_cstr("bendermeister");

  int err = 0;

  bench_timer_start(&timer);
  err = merlin_str8_replace_n(&str, &target, &replace, -1);
  bench_timer_end(&timer);

  lazy_error(err);

  merlin_str8_destroy(&str);
  return timer;
}

int main(void) {
  BENCH(linear(), 1000, 10);
  return 0;
}
