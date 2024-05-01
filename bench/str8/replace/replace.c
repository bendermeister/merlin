#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <bench/bench.h>
#include <errno.h>
#include <merlin/simd.h>
#include <merlin/str8.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void lazy_error(int err) {
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(err);
  }
}

static mrln_str8_t lorem = {};

static void lorem_init(void) {

  FILE *file = fopen("bench/str8/loremipsum.txt", "r");
  int err = errno;
  lazy_error(err);
  (void)fseek(file, 0, SEEK_END);

  lorem.capacity = ftell(file);
  lorem.length = lorem.capacity;
  rewind(file);

  lorem.buffer = malloc(lorem.capacity);
  err = errno;
  lazy_error(err);

  // TODO(ben): does this always read in one go or do we need loop?
  fread(lorem.buffer, lorem.length, 1, file);
  lazy_error(ferror(file));

  fclose(file);
}

static mrln_str8_t get_lorem(void) {
  mrln_str8_t str;
  str.buffer = malloc(lorem.length);
  __builtin_memcpy(str.buffer, lorem.buffer, lorem.length);
  str.length = lorem.length;
  str.capacity = lorem.length;
  return str;
}

#define UNLIKELY(A) __builtin_expect((A), 0)

#include "combine.c"
#include "findall.c"
#include "findallpcome.c"
#include "findallpcome128.c"
#include "findallpcome3.c"
#include "pfindallpcome.c"
#include "strstr.c"

intptr_t BENCH_simd256_find(const mrln_str8view_t haystack[static 1],
                            const mrln_str8view_t needle[static 1]) {
  intptr_t i = 0;

  intptr_t last_char_offset = needle->length - 1;
  uint8_t first_char = needle->buffer[0];
  uint8_t last_char = needle->buffer[last_char_offset];
  mrln_str8view_t target = {.length = needle->length};

  // 256bit
  {
    const mrln_v32u8_t first_char_reg = mrln_v32u8_set1(first_char);
    const mrln_v32u8_t last_char_reg = mrln_v32u8_set1(last_char);

    for (; i + 32 + needle->length < haystack->length; i += 32) {
      mrln_v32u8_t first_data =
          mrln_v32u8_load_unaligned((void *)&haystack->buffer[i]);
      first_data = mrln_v32u8_cmpeq(first_char_reg, first_data);

      mrln_v32u8_t last_data = mrln_v32u8_load_unaligned(
          (void *)&haystack->buffer[i + last_char_offset]);
      last_data = mrln_v32u8_cmpeq(last_char_reg, last_data);

      first_data = mrln_v32u8_or(first_data, last_data);
      uint32_t mask = mrln_v32u8_mask(first_data);

      uint32_t index_offset = 0;
      while (mask) {
        int32_t ctz = __builtin_ctz(mask);
        index_offset += ctz;
        mask >>= ctz;
        target.buffer = haystack->buffer + i + index_offset;
        bool cmp = mrln_str8view_is_equal(&target, needle);
        if (cmp) {
          return i + index_offset;
        }
        index_offset += 1;
        mask >>= 1;
      }
    }
    i -= 32; // TODO: can this cause problems
  }

  // 128bit
  {
    const mrln_v16u8_t first_char_reg = mrln_v16u8_set1(first_char);
    const mrln_v16u8_t last_char_reg = mrln_v16u8_set1(last_char);

    for (; i + 16 + needle->length < haystack->length; i += 16) {
      mrln_v16u8_t first_data =
          mrln_v16u8_load_unaligned((void *)&haystack->buffer[i]);
      first_data = mrln_v16u8_cmpeq(first_char_reg, first_data);

      mrln_v16u8_t last_data = mrln_v16u8_load_unaligned(
          (void *)&haystack->buffer[i + last_char_offset]);
      last_data = mrln_v16u8_cmpeq(last_char_reg, last_data);

      first_data = mrln_v16u8_or(first_data, last_data);
      uint32_t mask = mrln_v16u8_mask(first_data);

      uint32_t index_offset = 0;
      while (mask) {
        int32_t ctz = __builtin_ctz(mask);
        index_offset += ctz;
        mask >>= ctz;
        target.buffer = haystack->buffer + i + index_offset;
        bool cmp = mrln_str8view_is_equal(&target, needle);
        if (cmp) {
          return i + index_offset;
        }
        index_offset += 1;
        mask >>= 1;
      }
    }
    i -= 16;
  }

  for (; i + needle->length < haystack->length; ++i) {
    target.buffer = haystack->buffer + i;
    bool cmp = mrln_str8view_is_equal(&target, needle);
    if (cmp) {
      return i;
    }
  }

  return haystack->length;
}

void BENCH_simd256_split(const mrln_str8view_t haystack[static 1],
                         const mrln_str8view_t needle[static 1],
                         mrln_str8view_t lower[static 1],
                         mrln_str8view_t upper[static 1]) {
  intptr_t index = BENCH_simd256_find(haystack, needle);
  if (UNLIKELY(index >= haystack->length)) {
    *upper = (mrln_str8view_t){};
    *lower = *haystack;
    return;
  }
  lower->buffer = haystack->buffer;
  lower->length = index;
  upper->buffer = haystack->buffer + index + needle->length;
  upper->length = haystack->length - index - needle->length;
}

int BENCH_simd256_replace(mrln_str8_t s[static 1],
                          const mrln_str8view_t target[static 1],
                          const mrln_str8view_t replacement[static 1]) {
  mrln_str8_t new = (mrln_str8_t){};

  int err = mrln_str8_reserve(&new, s->capacity);
  if (UNLIKELY(err)) {
    return err;
  }

  mrln_str8view_t lower = (mrln_str8view_t){};
  mrln_str8view_t upper = mrln_str8_get_view(s, 0, s->length);
  BENCH_simd256_split(&upper, target, &lower, &upper);
  while (lower.length > 0) {
    err |= mrln_str8_concat(&new, &lower);
    err |= mrln_str8_concat(&new, replacement);
    if (err) {
      return err;
    }
    BENCH_simd256_split(&upper, target, &lower, &upper);
  }
  err |= mrln_str8_concat(&new, &upper);
  if (err) {
    return err;
  }
  mrln_str8_destroy(s);
  *s = new;
  return 0;
}

int BENCH_simd256_presplit_replace(
    mrln_str8_t s[static 1], const mrln_str8view_t target[static 1],
    const mrln_str8view_t replacement[static 1]) {

  struct {
    uint32_t capacity;
    uint32_t length;
    intptr_t *buffer;
  } splits = {.capacity = 16};

  mrln_str8view_t haystack = {.buffer = s->buffer, .length = s->length};
  intptr_t index = 0;

  while (index < haystack.length) {
    splits.capacity <<= 1;
    splits.buffer =
        realloc(splits.buffer, splits.capacity * sizeof(*splits.buffer));
    if (UNLIKELY(!splits.buffer)) {
      return ENOMEM;
    }
    const uint32_t limit = splits.capacity;
    for (; splits.length < limit && index < haystack.length; ++splits.length) {
      index = BENCH_simd256_find(&haystack, target);
      splits.buffer[splits.length] = index;
      haystack.buffer += index + target->length;
      haystack.length -= (index + target->length);
    }
  }

  mrln_str8_t str_new = {};
  str_new.capacity =
      s->length + splits.length * (replacement->length - target->length);
  str_new.buffer = malloc(str_new.capacity);
  if (UNLIKELY(!str_new.buffer)) {
    return ENOMEM;
  }

  haystack = (mrln_str8view_t){
      .length = s->length,
      .buffer = s->buffer,
  };
  const uint32_t splits_end = splits.length - 1;
  for (uint32_t i = 0; i < splits_end; ++i) {
    const intptr_t index = splits.buffer[i];

    __builtin_memcpy(str_new.buffer + str_new.length, haystack.buffer, index);
    str_new.length += index;
    // TODO(ben): this can be removed if we work with the last index right?
    haystack.length -= (index + target->length);
    haystack.buffer += index + target->length;

    __builtin_memcpy(str_new.buffer + str_new.length, replacement->buffer,
                     replacement->length);
    str_new.length += replacement->length;
  }
  __builtin_memcpy(str_new.buffer + str_new.length, haystack.buffer,
                   haystack.length);

  free(splits.buffer);
  free(s->buffer);
  *s = str_new;

  return 0;
}

static bench_timer_t BENCH_simd256_presplit(mrln_str8view_t target[static 1],
                                            mrln_str8view_t replace[static 1]) {
  bench_timer_t timer = {};
  mrln_str8_t str = get_lorem();

  int err = 0;

  bench_timer_start(&timer);
  err = BENCH_simd256_presplit_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);
  return timer;
}

static bench_timer_t BENCH_simd256(mrln_str8view_t target[static 1],
                                   mrln_str8view_t replace[static 1]) {
  bench_timer_t timer = {};
  mrln_str8_t str = get_lorem();

  int err = 0;

  bench_timer_start(&timer);
  err = BENCH_simd256_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);
  return timer;
}

intptr_t BENCH_simd128_find(const mrln_str8view_t haystack[static 1],
                            const mrln_str8view_t needle[static 1]) {
  intptr_t i = 0;

  intptr_t last_char_offset = needle->length - 1;
  uint8_t first_char = needle->buffer[0];
  uint8_t last_char = needle->buffer[last_char_offset];

  const mrln_v16u8_t first_char_reg = mrln_v16u8_set1(first_char);
  const mrln_v16u8_t last_char_reg = mrln_v16u8_set1(last_char);

  mrln_str8view_t target = {.length = needle->length};
  for (; i + 16 + needle->length < haystack->length; i += 16) {
    mrln_v16u8_t first_data =
        mrln_v16u8_load_unaligned((void *)&haystack->buffer[i]);

    first_data = mrln_v16u8_cmpeq(first_char_reg, first_data);

    mrln_v16u8_t last_data = mrln_v16u8_load_unaligned(
        (void *)&haystack->buffer[i + last_char_offset]);

    last_data = mrln_v16u8_cmpeq(last_char_reg, last_data);

    first_data = mrln_v16u8_and(first_data, last_data);
    uint32_t mask = mrln_v16u8_mask(first_data);

    uint32_t index_offset = 0;
    while (mask) {
      int32_t ctz = __builtin_ctz(mask);
      index_offset += ctz;
      mask >>= ctz;
      target.buffer = haystack->buffer + i + index_offset;
      bool cmp = mrln_str8view_is_equal(&target, needle);
      if (cmp) {
        return i + index_offset;
      }
      index_offset += 1;
      mask >>= 1;
    }
  }
  i -= 16;
  for (; i + needle->length < haystack->length; ++i) {
    target.buffer = haystack->buffer + i;
    bool cmp = mrln_str8view_is_equal(&target, needle);
    if (cmp) {
      return i;
    }
  }

  return haystack->length;
}

void BENCH_simd128_split(const mrln_str8view_t haystack[static 1],
                         const mrln_str8view_t needle[static 1],
                         mrln_str8view_t lower[static 1],
                         mrln_str8view_t upper[static 1]) {
  intptr_t index = BENCH_simd128_find(haystack, needle);
  if (UNLIKELY(index >= haystack->length)) {
    *upper = (mrln_str8view_t){};
    *lower = *haystack;
    return;
  }
  lower->buffer = haystack->buffer;
  lower->length = index;
  upper->buffer = haystack->buffer + index + needle->length;
  upper->length = haystack->length - index - needle->length;
}

int BENCH_simd128_replace(mrln_str8_t s[static 1],
                          const mrln_str8view_t target[static 1],
                          const mrln_str8view_t replacement[static 1]) {
  mrln_str8_t new = (mrln_str8_t){};

  int err = mrln_str8_reserve(&new, s->capacity);
  if (UNLIKELY(err)) {
    return err;
  }

  mrln_str8view_t lower = (mrln_str8view_t){};
  mrln_str8view_t upper = mrln_str8_get_view(s, 0, s->length);
  BENCH_simd128_split(&upper, target, &lower, &upper);
  while (lower.length > 0) {
    err |= mrln_str8_concat(&new, &lower);
    err |= mrln_str8_concat(&new, replacement);
    if (err) {
      return err;
    }
    BENCH_simd128_split(&upper, target, &lower, &upper);
  }
  err |= mrln_str8_concat(&new, &upper);
  if (err) {
    return err;
  }
  mrln_str8_destroy(s);
  *s = new;
  return 0;
}

static bench_timer_t BENCH_simd128(mrln_str8view_t target[static 1],
                                   mrln_str8view_t replace[static 1]) {
  bench_timer_t timer = {};
  mrln_str8_t str = get_lorem();

  int err = 0;

  bench_timer_start(&timer);
  err = BENCH_simd128_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);
  return timer;
}

int BENCH_linear_replace(mrln_str8_t s[static 1],
                         const mrln_str8view_t target[static 1],
                         const mrln_str8view_t replacement[static 1]) {
  mrln_str8_t new = (mrln_str8_t){};

  int err = mrln_str8_reserve(&new, s->capacity);
  if (UNLIKELY(err)) {
    return err;
  }

  mrln_str8view_t lower = (mrln_str8view_t){};
  mrln_str8view_t upper = mrln_str8_get_view(s, 0, s->length);
  mrln_str8_split_at_view(&upper, target, &lower, &upper);
  while (lower.length > 0) {
    err |= mrln_str8_concat(&new, &lower);
    err |= mrln_str8_concat(&new, replacement);
    if (err) {
      return err;
    }
    mrln_str8_split_at_view(&upper, target, &lower, &upper);
  }
  err |= mrln_str8_concat(&new, &upper);
  if (err) {
    return err;
  }
  mrln_str8_destroy(s);
  *s = new;
  return 0;
}

static bench_timer_t BENCH_stack2(mrln_str8view_t target[static 1],
                                  mrln_str8view_t replace[static 1]) {
  bench_timer_t timer = {};
  mrln_str8_t str = get_lorem();

  int err = 0;

  bench_timer_start(&timer);
  err = mrln_str8_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);
  return timer;
}

static bench_timer_t linear(mrln_str8view_t target[static 1],
                            mrln_str8view_t replace[static 1]) {
  bench_timer_t timer = {};
  mrln_str8_t str = get_lorem();

  int err = 0;

  bench_timer_start(&timer);
  err = BENCH_linear_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);
  return timer;
}

int main(void) {
  lorem_init();

  mrln_str8view_t target = mrln_str8view("est");
  mrln_str8view_t replace = mrln_str8view("bendermeister");

  BENCH(linear(&target, &replace), 10000, 10);
  BENCH(BENCH_simd128(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256_presplit(&target, &replace), 10000, 10);
  BENCH(BENCH_findall_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_strstr_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome128_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_pfindallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome3_bench(&target, &replace), 10000, 0);
  BENCH(BENCH_combine_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_stack2(&target, &replace), 10000, 10);
  putc('\n', stdout);

  target = mrln_str8view("pellentesque");
  replace = mrln_str8view("bendermeister");

  BENCH(linear(&target, &replace), 10000, 10);
  BENCH(BENCH_simd128(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256_presplit(&target, &replace), 10000, 10);
  BENCH(BENCH_findall_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_strstr_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome128_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_pfindallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_combine_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_stack2(&target, &replace), 10000, 10);

  putc('\n', stdout);

  target = mrln_str8view("metus");
  replace = mrln_str8view("bendermeister");

  BENCH(linear(&target, &replace), 10000, 10);
  BENCH(BENCH_simd128(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256(&target, &replace), 10000, 10);
  BENCH(BENCH_simd256_presplit(&target, &replace), 10000, 10);
  BENCH(BENCH_findall_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_strstr_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_findallpcome128_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_pfindallpcome_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_combine_bench(&target, &replace), 10000, 10);
  BENCH(BENCH_stack2(&target, &replace), 10000, 10);

  free(lorem.buffer);
  return 0;
}
