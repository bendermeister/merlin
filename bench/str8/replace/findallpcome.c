#include <asm-generic/errno-base.h>
#include <bench/bench.h>
#include <merlin/simd.h>
#include <merlin/str8.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef UNLIKELY
#define UNLIKELY(A) __builtin_expect((A), 0)
#endif

#ifndef LIKELY
#define LIKELY(A) __builtin_expect((A), 1)
#endif

static void lazy_error(int error);
static merlin_str8_t get_lorem(void);

typedef struct BENCH_findallpcome_result_t BENCH_findallpcome_result_t;
struct BENCH_findallpcome_result_t {
  intptr_t *buffer;
  intptr_t capacity;
  intptr_t length;
  int error;
};

__attribute__((__always_inline__, __nonnull__(1))) static int
BENCH_findallpcome_result_grow_to(BENCH_findallpcome_result_t result[static 1],
                                  const intptr_t new_cap) {
  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static BENCH_findallpcome_result_t
BENCH_findallpcome_find(const merlin_str8_view_t haystack[static 1],
                        const merlin_str8_view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (BENCH_findallpcome_result_t){};
  }

  const intptr_t last_char_offset = needle->length - 1;
  const uint8_t last_char = needle->buffer[last_char_offset];
  const uint8_t first_char = needle->buffer[0];

  BENCH_findallpcome_result_t result = {.capacity = 16, .error = ENOMEM};

  const intptr_t v32u8_iter_end =
      (haystack->length - 32 - last_char_offset) / 32;
  const intptr_t v16u8_index_offset = v32u8_iter_end * 32;

  const intptr_t v16u8_iter_end =
      (haystack->length - 16 - last_char_offset - (v32u8_iter_end * 32)) / 16;
  const intptr_t v1u8_index_offset = v16u8_iter_end * 16;

  const intptr_t v1u8_iter_end = haystack->length - last_char_offset -
                                 (v32u8_iter_end * 32) - (v16u8_iter_end * 16);

  {
    intptr_t index = 0;
    const merlin_v32u8_t row_first_char = merlin_v32u8_set1(first_char);
    const merlin_v32u8_t row_last_char = merlin_v32u8_set1(last_char);
    const merlin_v32u8_unaligned_t *first_buffer =
        (const merlin_v32u8_unaligned_t *)haystack->buffer;
    const merlin_v32u8_unaligned_t *last_buffer =
        (const merlin_v32u8_unaligned_t *)(haystack->buffer + last_char_offset);
    merlin_v32u8_t row_in;
    intptr_t i = 0;
    intptr_t mask;
    for (; i < v32u8_iter_end;) {
      int err =
          BENCH_findallpcome_result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }
      const intptr_t result_end = result.capacity;
      for (; (bool)(i < v32u8_iter_end) & (bool)(result.length < result_end);
           ++i) {
        row_in = merlin_v32u8_load_unaligned(first_buffer + i);
        row_in = merlin_v32u8_cmpeq(row_in, row_first_char);
        mask = merlin_v32u8_mask(row_in);

        row_in = merlin_v32u8_load_unaligned(last_buffer + i);
        row_in = merlin_v32u8_cmpeq(row_in, row_last_char);
        mask &= merlin_v32u8_mask(row_in);

        // const intptr_t mask_end = __builtin_popcount(mask);
        intptr_t index_offset = 0;
        while (mask) {
          const intptr_t ctz = __builtin_ctz(mask);
          index_offset += ctz;
          mask >>= ctz;

          const intptr_t real_index = index + index_offset;
          if (__builtin_memcmp(haystack->buffer + real_index, needle->buffer,
                               needle->length) == 0) {
            result.buffer[result.length++] = real_index;
          }
          mask >>= 1;
          index_offset += 1;
        }
        index += 32;
      }
    }
  }

  if (result.length + 32 > result.capacity) {
    int err = BENCH_findallpcome_result_grow_to(&result, result.length + 32);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  // 128 bit
  if (v16u8_iter_end) {
    intptr_t index = v1u8_index_offset;
    const merlin_v16u8_t row_first_char = merlin_v16u8_set1(first_char);
    const merlin_v16u8_t row_last_char = merlin_v16u8_set1(last_char);
    const merlin_v16u8_unaligned_t *first_buffer =
        (const merlin_v16u8_unaligned_t *)(haystack->buffer +
                                           v16u8_index_offset);
    const merlin_v16u8_unaligned_t *last_buffer =
        (const merlin_v16u8_unaligned_t *)(haystack->buffer +
                                           v16u8_index_offset +
                                           last_char_offset);
    merlin_v16u8_t row_in;
    intptr_t i = 0;
    intptr_t mask;
    row_in = merlin_v16u8_load_unaligned(first_buffer + i);
    row_in = merlin_v16u8_cmpeq(row_in, row_first_char);
    mask = merlin_v16u8_mask(row_in);

    row_in = merlin_v16u8_load_unaligned(last_buffer + i);
    row_in = merlin_v16u8_cmpeq(row_in, row_last_char);
    mask &= merlin_v16u8_mask(row_in);

    const intptr_t mask_end = __builtin_popcount(mask);
    intptr_t index_offset = 0;
    for (intptr_t j = 0; j < mask_end; ++j) {
      const intptr_t ctz = __builtin_ctz(mask);
      index_offset += ctz;
      mask >>= ctz;

      const intptr_t real_index = index + index_offset;
      if (__builtin_memcmp(haystack->buffer + real_index, needle->buffer,
                           needle->length) == 0) {
        result.buffer[result.length++] = real_index;
      }
      mask >>= 1;
      index_offset += 1;
    }
  }

  // 8 bit
  {
    intptr_t index = v16u8_index_offset;
    const uint8_t *buffer = haystack->buffer + v1u8_index_offset;
    for (intptr_t i = 0; i < v1u8_iter_end; ++i) {
      if (__builtin_memcmp(buffer + i, needle->buffer, needle->length) == 0) {
        result.buffer[result.length++] = index + i;
      }
    }
  }

  result.error = 0;
  return result;
}

static int
BENCH_findallpcome_replace(merlin_str8_t s[static 1],
                           const merlin_str8_view_t target[static 1],
                           const merlin_str8_view_t replacement[static 1]) {

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_findallpcome_result_t splits =
      BENCH_findallpcome_find(&haystack, target);
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

static bench_timer_t
BENCH_findallpcome_bench(merlin_str8_view_t target[static 1],
                         merlin_str8_view_t replace[static 1]) {
  merlin_str8_t str = get_lorem();

  bench_timer_t timer = {};

  bench_timer_start(&timer);
  int err = BENCH_findallpcome_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  merlin_str8_destroy(&str);

  return timer;
}
