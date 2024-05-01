#include <asm-generic/errno-base.h>
#include <bench/bench.h>
#include <merlin/simd.h>
#include <merlin/str8.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef UNLIKELY
#define UNLIKELY(A) __builtin_expect((A), 0)
#endif

#ifndef LIKELY
#define LIKELY(A) __builtin_expect((A), 1)
#endif

static void lazy_error(int error);
static mrln_str8_t get_lorem(void);

typedef struct BENCH_strstr_result_t BENCH_strstr_result_t;
struct BENCH_strstr_result_t {
  intptr_t *buffer;
  intptr_t capacity;
  intptr_t length;
  int error;
};

__attribute__((__always_inline__, __nonnull__(1))) static int
BENCH_strstr_result_grow_to(BENCH_strstr_result_t result[static 1],
                            const intptr_t new_cap) {
  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static BENCH_strstr_result_t
BENCH_strstr_find(const mrln_str8view_t haystack[static 1],
                  const mrln_str8view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (BENCH_strstr_result_t){};
  }
  BENCH_strstr_result_t result = {.capacity = 16, .error = ENOMEM};

  bool keep_searching = true;
  intptr_t index = 0;
  while (keep_searching) {
    int err = BENCH_strstr_result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    while (keep_searching && result.length < result_end) {
      const uint8_t *substring = (const uint8_t *)strstr(
          (const char *)haystack->buffer + index, (const char *)needle->buffer);
      keep_searching = substring != NULL;
      if (keep_searching) {
        index = substring - haystack->buffer;
        result.buffer[result.length++] = index;
        index += needle->length;
      }
    }
  }

  result.error = 0;
  return result;
}

static int BENCH_strstr_replace(mrln_str8_t s[static 1],
                                const mrln_str8view_t target[static 1],
                                const mrln_str8view_t replacement[static 1]) {

  (void)mrln_str8_concat_char(s, 0);

  mrln_str8view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_strstr_result_t splits = BENCH_strstr_find(&haystack, target);
  if (UNLIKELY(splits.error)) {
    free(splits.buffer);
    return splits.error;
  }

  mrln_str8_t str_new = {};
  str_new.capacity =
      s->length + splits.length * (replacement->length - target->length);
  str_new.buffer = malloc(str_new.capacity);
  if (UNLIKELY(!str_new.buffer)) {
    return ENOMEM;
  }

  const intptr_t splits_end = splits.length;
  intptr_t previous_index = 0;
  for (intptr_t i = 0; i < splits_end; ++i) {
    const intptr_t index = splits.buffer[i];

    __builtin_memcpy(str_new.buffer + str_new.length,
                     haystack.buffer + previous_index, index - previous_index);
    str_new.length += index - previous_index;

    __builtin_memcpy(str_new.buffer + str_new.length, replacement->buffer,
                     replacement->length);
    str_new.length += replacement->length;
    previous_index = index + target->length;
  }
  __builtin_memcpy(str_new.buffer + str_new.length,
                   haystack.buffer + previous_index,
                   haystack.length - previous_index);

  str_new.length += haystack.length - previous_index;

  free(splits.buffer);
  free(s->buffer);
  *s = str_new;
  return 0;
}

static bench_timer_t BENCH_strstr_bench(mrln_str8view_t target[static 1],
                                        mrln_str8view_t replace[static 1]) {
  mrln_str8_t str = get_lorem();

  bench_timer_t timer = {};

  bench_timer_start(&timer);
  int err = BENCH_strstr_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);

  return timer;
}
