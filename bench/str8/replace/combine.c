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
static merlin_str8_t get_lorem(void);

typedef struct BENCH_combine_result_t BENCH_combine_result_t;
struct BENCH_combine_result_t {
  intptr_t *buffer;
  intptr_t capacity;
  intptr_t length;
  int error;
};

__attribute__((__always_inline__, __nonnull__(1))) static int
BENCH_combine_result_grow_to(BENCH_combine_result_t result[static 1],
                             const intptr_t new_cap) {
  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static BENCH_combine_result_t
BENCH_combine_find_2(const merlin_str8_view_t haystack[static 1],
                     const merlin_str8_view_t needle[static 1]) {

  BENCH_combine_result_t result = {.capacity = 16, .error = ENOMEM};

  intptr_t index = 2;
  const intptr_t end = haystack->length - needle->length + 1;

  const uint16_t hash = ((uint16_t)needle->buffer[0] << 8) | needle->buffer[1];
  uint16_t rolling_hash =
      ((uint16_t)haystack->buffer[0] << 8) | haystack->buffer[1];

  for (; index < end;) {
    int err = BENCH_combine_result_grow_to(&result, result.capacity * 2);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    for (; index < end && result.length < result_end; ++index) {
      result.buffer[result.length] = index - 2;
      result.length += rolling_hash == hash;
      rolling_hash <<= 8;
      rolling_hash |= haystack->buffer[index];
    }
  }

  result.error = 0;
  return result;
}

static BENCH_combine_result_t
BENCH_combine_find_3(const merlin_str8_view_t haystack[static 1],
                     const merlin_str8_view_t needle[static 1]) {

  BENCH_combine_result_t result = {.capacity = 16, .error = ENOMEM};

  const intptr_t v32u8_end = ((haystack->length - 2) / 32) * 32;
  intptr_t index = 0;

  // 256
  {
    merlin_v32u8_t r0, r1, r2, m0, m1, m2, t;
    m0 = merlin_v32u8_set1(needle->buffer[0]);
    m1 = merlin_v32u8_set1(needle->buffer[1]);
    m2 = merlin_v32u8_set1(needle->buffer[2]);
    for (; index < v32u8_end;) {
      int err = BENCH_combine_result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }
      const intptr_t result_end = result.capacity;
      for (; index < v32u8_end && result.length < result_end; index += 32) {
        r0 =
            merlin_v32u8_load_unaligned((void *)(haystack->buffer + index + 0));
        r0 = merlin_v32u8_cmpeq(r0, m0);

        r1 =
            merlin_v32u8_load_unaligned((void *)(haystack->buffer + index + 1));
        r1 = merlin_v32u8_cmpeq(r1, m1);

        r2 =
            merlin_v32u8_load_unaligned((void *)(haystack->buffer + index + 2));
        r2 = merlin_v32u8_cmpeq(r2, m2);

        t = merlin_v32u8_and(r0, r1);
        t = merlin_v32u8_and(t, r2);
        uint32_t mask = merlin_v32u8_mask(t);
        intptr_t index_offset = 0;
        while (mask) {
          intptr_t ctz = __builtin_ctz(mask);
          index_offset += ctz;
          mask >>= ctz;
          result.buffer[result.length++] = index + index_offset;
          index_offset += 1;
          mask >>= 1;
        }
      }
    }
  }

  if (result.length + 11 > result.capacity) {
    int err = BENCH_combine_result_grow_to(&result, result.capacity + 11);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length; ++index) {
    result.buffer[result.length] = index;
    result.length +=
        __builtin_memcmp(haystack->buffer + index, needle->buffer, 3) == 0;
  }

  result.error = 0;
  return result;
}

static BENCH_combine_result_t
BENCH_combine_find_4(const merlin_str8_view_t haystack[static 1],
                     const merlin_str8_view_t needle[static 1]) {

  BENCH_combine_result_t result = {.capacity = 16, .error = ENOMEM};

  intptr_t index = 4;
  const intptr_t end = haystack->length - needle->length + 1;

  const uint32_t hash = ((uint32_t)needle->buffer[0] << 24) |
                        ((uint32_t)needle->buffer[1] << 16) |
                        ((uint32_t)needle->buffer[2] << 8) |
                        ((uint32_t)needle->buffer[3] << 0);

  uint32_t rolling_hash = ((uint32_t)haystack->buffer[0] << 24) |
                          ((uint32_t)haystack->buffer[1] << 16) |
                          ((uint32_t)haystack->buffer[2] << 8) |
                          ((uint32_t)haystack->buffer[3] << 0);

  for (; index < end;) {
    int err = BENCH_combine_result_grow_to(&result, result.capacity * 2);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    for (; index < end && result.length < result_end; ++index) {
      result.buffer[result.length] = index - 3;
      result.length += rolling_hash == hash;
      rolling_hash <<= 8;
      rolling_hash |= haystack->buffer[index];
    }
  }

  result.error = 0;
  return result;
}

static BENCH_combine_result_t
BENCH_combine_find_long_needle(const merlin_str8_view_t haystack[static 1],
                               const merlin_str8_view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (BENCH_combine_result_t){};
  }

  BENCH_combine_result_t result = {.error = ENOMEM, .capacity = 16};

  const intptr_t v32u8_end = ((haystack->length - needle->length) / 32) * 32;

  intptr_t index = 0;
  // 256
  {
    // r = row, b = begin, m = mask, e = end, o = offset
    const intptr_t rb0o = 0;
    const intptr_t rb1o = 1;
    const intptr_t re0o = needle->length - 2;
    const intptr_t re1o = needle->length - 1;
    merlin_v32u8_t rb0, rb1, re0, re1, mb0, mb1, me0, me1, t;
    mb0 = merlin_v32u8_set1(needle->buffer[rb0o]);
    mb1 = merlin_v32u8_set1(needle->buffer[rb1o]);
    me0 = merlin_v32u8_set1(needle->buffer[re0o]);
    me1 = merlin_v32u8_set1(needle->buffer[re1o]);

    while (index < v32u8_end) {
      int err = BENCH_combine_result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }

      const intptr_t result_end = result.capacity;
      for (; index < v32u8_end && result.length < result_end; index += 32) {
        rb0 = merlin_v32u8_load_unaligned((void *)haystack->buffer + index +
                                          rb0o);
        rb0 = merlin_v32u8_cmpeq(rb0, mb0);

        rb1 = merlin_v32u8_load_unaligned((void *)haystack->buffer + index +
                                          rb1o);
        rb1 = merlin_v32u8_cmpeq(rb1, mb1);

        re0 = merlin_v32u8_load_unaligned((void *)haystack->buffer + index +
                                          re0o);
        re0 = merlin_v32u8_cmpeq(re0, me0);

        re1 = merlin_v32u8_load_unaligned((void *)haystack->buffer + index +
                                          re1o);
        re1 = merlin_v32u8_cmpeq(re1, me1);

        t = merlin_v32u8_and(rb0, rb1);
        t = merlin_v32u8_and(t, re0);
        t = merlin_v32u8_and(t, re1);

        uint32_t mask = merlin_v32u8_mask(t);
        intptr_t index_offset = 0;
        while (mask) {
          intptr_t ctz = __builtin_ctz(mask);
          mask >>= ctz;
          index_offset += ctz;
          result.buffer[result.length++] = index + index_offset;
          index_offset += 1;
          mask >>= 1;
        }
      }
    }
  }

  intptr_t new_length = 0;
  const intptr_t result_end = result.length;
  for (intptr_t i = 0; i < result_end; ++i) {
    result.buffer[new_length] = result.buffer[i];
    new_length += memcmp(haystack->buffer + result.buffer[i], needle->buffer,
                         needle->length) == 0;
  }
  result.length = new_length;

  if (result.length + 32 > result.capacity) {
    int err = BENCH_combine_result_grow_to(&result, result.capacity + 32);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length - needle->length; ++index) {
    result.buffer[result.length] = index;
    result.length +=
        haystack->buffer[index] == needle->buffer[0] &&
        haystack->buffer[needle->length - 1] ==
            needle->buffer[needle->length - 1] &&
        memcmp(haystack->buffer + index, needle->buffer, needle->length) == 0;
  }

  result.error = 0;
  return result;
}

static BENCH_combine_result_t
BENCH_combine_find(const merlin_str8_view_t haystack[static 1],
                   const merlin_str8_view_t needle[static 1]) {
  switch (needle->length) {
  case 0:
  case 1:
    __builtin_unreachable();
  case 2:
    return BENCH_combine_find_2(haystack, needle);
  case 3:
    return BENCH_combine_find_3(haystack, needle);
  case 4:
    return BENCH_combine_find_4(haystack, needle);
  default:
    return BENCH_combine_find_long_needle(haystack, needle);
  }
}

static int BENCH_combine_replace_inplace_char(merlin_str8_t s[static 1],
                                              const uint8_t target,
                                              const uint8_t replacement) {
  const intptr_t end = s->length;

  const intptr_t v32u8_end = (end / 32) * 32;
  const merlin_v32u8_t v32_target_mask = merlin_v32u8_set1(target);
  const merlin_v32u8_t v32_replace_mask = merlin_v32u8_set1(replacement);

  intptr_t index = 0;
  for (; index < v32u8_end; index += 32) {
    merlin_v32u8_t data =
        merlin_v32u8_load_unaligned((void *)(s->buffer + index));
    merlin_v32u8_t mask = merlin_v32u8_cmpeq(data, v32_target_mask);
    merlin_v32u8_t rmask = merlin_v32u8_and(mask, v32_replace_mask);
    mask = merlin_v32u8_not(mask);
    data = merlin_v32u8_and(data, mask);
    data = merlin_v32u8_or(data, rmask);
    merlin_v32u8_store_unaligned((void *)(s->buffer + index), data);
  }

  if ((end - v32u8_end) / 16) {

    const merlin_v16u8_t v16_target_mask = merlin_v16u8_set1(target);
    const merlin_v16u8_t v16_replace_mask = merlin_v16u8_set1(replacement);

    merlin_v16u8_t data =
        merlin_v16u8_load_unaligned((void *)(s->buffer + index));
    merlin_v16u8_t mask = merlin_v16u8_cmpeq(data, v16_target_mask);
    merlin_v16u8_t rmask = merlin_v16u8_and(mask, v16_replace_mask);
    mask = merlin_v16u8_not(mask);
    data = merlin_v16u8_and(data, mask);
    data = merlin_v16u8_or(data, rmask);
    merlin_v16u8_store_unaligned((void *)(s->buffer + index), data);

    index += 16;
  }

  for (; index < end; ++index) {
    const char c = s->buffer[index];
    if (c == target) {
      s->buffer[index] = replacement;
    }
  }

  return 0;
}

static int
BENCH_combine_replace_inplace(merlin_str8_t s[static 1],
                              const merlin_str8_view_t target[static 1],
                              const merlin_str8_view_t replacement[static 1]) {
  if (target->length == 1) {
    return BENCH_combine_replace_inplace_char(s, target->buffer[0],
                                              replacement->buffer[0]);
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_combine_result_t splits = BENCH_combine_find(&haystack, target);
  if (UNLIKELY(splits.error)) {
    free(splits.buffer);
    return splits.error;
  }

  const intptr_t splits_end = splits.length;
  for (intptr_t i = 0; i < splits_end; ++i) {
    const intptr_t index = splits.buffer[i];
    memcpy(s->buffer + index, replacement->buffer, replacement->length);
  }
  free(splits.buffer);
  return 0;
}

static int
BENCH_combine_replace(merlin_str8_t s[static 1],
                      const merlin_str8_view_t target[static 1],
                      const merlin_str8_view_t replacement[static 1]) {

  if (target->length == replacement->length) {
    return BENCH_combine_replace_inplace(s, target, replacement);
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_combine_result_t splits = BENCH_combine_find(&haystack, target);
  if (UNLIKELY(splits.error)) {
    free(splits.buffer);
    return splits.error;
  }

  merlin_str8_t str_new = {};
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

static bench_timer_t BENCH_combine_bench(merlin_str8_view_t target[static 1],
                                         merlin_str8_view_t replace[static 1]) {
  merlin_str8_t str = get_lorem();

  bench_timer_t timer = {};

  bench_timer_start(&timer);
  int err = BENCH_combine_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  merlin_str8_destroy(&str);

  return timer;
}
