#include <asm-generic/errno-base.h>
#include <bench/bench.h>
#include <merlin/simd.h>
#include <merlin/str8.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <immintrin.h>

#ifndef UNLIKELY
#define UNLIKELY(A) __builtin_expect((A), 0)
#endif

#ifndef LIKELY
#define LIKELY(A) __builtin_expect((A), 1)
#endif

static void lazy_error(int error);
static mrln_str8_t get_lorem(void);

typedef struct BENCH_findallpcome3_result_t BENCH_findallpcome3_result_t;
struct BENCH_findallpcome3_result_t {
  intptr_t *buffer;
  intptr_t capacity;
  intptr_t length;
  int error;
};

static void row32_search(const mrln_v8u32_unaligned_t addr[static 1],
                         BENCH_findallpcome3_result_t result[static 1],
                         intptr_t index, intptr_t offset,
                         mrln_v8u32_t row_selection_mask,
                         mrln_v8u32_t row_key) {
  mrln_v8u32_t row;
  row = mrln_v8u32_load_unaligned(addr);
  row = mrln_v8u32_and(row, row_selection_mask);
  row = mrln_v8u32_cmpeq(row, row_key);
  int mask = mrln_v8u32_mask(row);
  int index_offset = 0;
  while (mask) {
    const int ctz = __builtin_ctz(mask);
    index_offset += ctz;
    result->buffer[result->length++] = index + index_offset * 4 + offset;
    mask >>= ctz + 1;
    index_offset += 1;
  }
}

__attribute__((__always_inline__, __nonnull__(1))) static int
BENCH_findallpcome3_result_grow_to(
    BENCH_findallpcome3_result_t result[static 1], const intptr_t new_cap) {
  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static int intptr_cmp(const void *a, const void *b) {
  return *(intptr_t *)a - *(intptr_t *)b;
}

static BENCH_findallpcome3_result_t
BENCH_findallpcome3_find(const mrln_str8view_t haystack[static 1],
                         const mrln_str8view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (BENCH_findallpcome3_result_t){};
  }

  BENCH_findallpcome3_result_t result = {.capacity = 32, .error = ENOMEM};

  const uint32_t key = ((uint32_t)needle->buffer[2] << 16) |
                       ((uint32_t)needle->buffer[1] << 8) |
                       ((uint32_t)needle->buffer[0]);

  const intptr_t v8u32_end = (haystack->length - 3 - 32);
  {
    intptr_t index = 0;
    const mrln_v8u32_t row_key = mrln_v8u32_set1(key);
    const mrln_v8u32_t row_selection_mask = mrln_v8u32_set1(0xffffffu);
    mrln_v8u32_t row;
    mrln_v8u32_t row_data;
    uint32_t mask;
    int index_offset = 0;

    for (; index < v8u32_end;) {
      int err =
          BENCH_findallpcome3_result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }
      const intptr_t result_end = result.capacity - 32;
      for (; index < v8u32_end && result.length < result_end; index += 32) {
        row_data =
            mrln_v8u32_load_unaligned((void *)(haystack->buffer + index));
        row = mrln_v8u32_and(row_data, row_selection_mask);
        row = mrln_v8u32_cmpeq(row, row_key);
        mask = mrln_v8u32_mask(row);
        index_offset = 0;
        while (mask) {
          const int ctz = __builtin_ctz(mask);
          index_offset += ctz;
          result.buffer[result.length++] = index + index_offset * 4;
          mask >>= ctz + 1;
          index_offset += 1;
        }

        row_data = _mm256_bslli_epi128(row_data, 1);
        row_data[0] = (haystack->buffer + index)[1];
        row = mrln_v8u32_and(row_data, row_selection_mask);
        row = mrln_v8u32_cmpeq(row, row_key);
        mask = mrln_v8u32_mask(row);
        index_offset = 0;
        while (mask) {
          const int ctz = __builtin_ctz(mask);
          index_offset += ctz;
          result.buffer[result.length++] = index + index_offset * 4 + 1;
          mask >>= ctz + 1;
          index_offset += 1;
        }

        row_data = _mm256_bslli_epi128(row_data, 1);
        row_data[0] = (haystack->buffer + index)[2];
        row = mrln_v8u32_and(row_data, row_selection_mask);
        row = mrln_v8u32_cmpeq(row, row_key);
        mask = mrln_v8u32_mask(row);
        index_offset = 0;
        while (mask) {
          const int ctz = __builtin_ctz(mask);
          index_offset += ctz;
          result.buffer[result.length++] = index + index_offset * 4 + 2;
          mask >>= ctz + 1;
          index_offset += 1;
        }

        row_data = _mm256_bslli_epi128(row_data, 1);
        row_data[0] = (haystack->buffer + index)[3];
        row = mrln_v8u32_and(row_data, row_selection_mask);
        row = mrln_v8u32_cmpeq(row, row_key);
        mask = mrln_v8u32_mask(row);
        index_offset = 0;
        while (mask) {
          const int ctz = __builtin_ctz(mask);
          index_offset += ctz;
          result.buffer[result.length++] = index + index_offset * 4 + 3;
          mask >>= ctz + 1;
          index_offset += 1;
        }
      }
    }
  }

  if (result.length + 32 > result.capacity) {
    int err = BENCH_findallpcome3_result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  intptr_t index = v8u32_end;

  if (index + 16 + 4 < haystack->length) {
    const mrln_v4u32_t row_key = mrln_v4u32_set1(key);
    const mrln_v4u32_t row_selection_mask = mrln_v4u32_set1(0xffffffu);
    mrln_v4u32_t row;
    uint32_t mask;
    row = mrln_v4u32_load_unaligned((void *)(haystack->buffer + index));
    row = mrln_v4u32_and(row, row_selection_mask);
    row = mrln_v4u32_cmpeq(row, row_key);
    mask = mrln_v4u32_mask(row);
    {
      int index_offset = 0;
      while (mask) {
        const int ctz = __builtin_ctz(mask);
        index_offset += ctz;
        result.buffer[result.length++] = index + index_offset * 4;
        mask >>= ctz + 1;
        index_offset += 1;
      }
    }

    row = mrln_v4u32_load_unaligned((void *)(haystack->buffer + index + 1));
    row = mrln_v4u32_and(row, row_selection_mask);
    row = mrln_v4u32_cmpeq(row, row_key);
    mask = mrln_v4u32_mask(row);
    {
      int index_offset = 0;
      while (mask) {
        const int ctz = __builtin_ctz(mask);
        index_offset += ctz;
        result.buffer[result.length++] = index + index_offset * 4;
        mask >>= ctz + 1;
        index_offset += 1;
      }
    }

    row = mrln_v4u32_load_unaligned((void *)(haystack->buffer + index + 2));
    row = mrln_v4u32_and(row, row_selection_mask);
    row = mrln_v4u32_cmpeq(row, row_key);
    mask = mrln_v4u32_mask(row);
    {
      int index_offset = 0;
      while (mask) {
        const int ctz = __builtin_ctz(mask);
        index_offset += ctz;
        result.buffer[result.length++] = index + index_offset * 4;
        mask >>= ctz + 1;
        index_offset += 1;
      }
    }

    row = mrln_v4u32_load_unaligned((void *)(haystack->buffer + index + 3));
    row = mrln_v4u32_and(row, row_selection_mask);
    row = mrln_v4u32_cmpeq(row, row_key);
    mask = mrln_v4u32_mask(row);
    {
      int index_offset = 0;
      while (mask) {
        const int ctz = __builtin_ctz(mask);
        index_offset += ctz;
        result.buffer[result.length++] = index + index_offset * 4;
        mask >>= ctz + 1;
        index_offset += 1;
      }
    }
  }

  for (; index + 2 < haystack->length; ++index) {
    if (memcmp(haystack->buffer + index, needle->buffer, needle->length) == 0) {
      result.buffer[result.length++] = index;
    }
  }

  qsort(result.buffer, result.length, sizeof(*result.buffer), intptr_cmp);

  result.error = 0;
  return result;
}

static int
BENCH_findallpcome3_replace(mrln_str8_t s[static 1],
                            const mrln_str8view_t target[static 1],
                            const mrln_str8view_t replacement[static 1]) {

  mrln_str8view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_findallpcome3_result_t splits =
      BENCH_findallpcome3_find(&haystack, target);
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

static bench_timer_t
BENCH_findallpcome3_bench(mrln_str8view_t target[static 1],
                          mrln_str8view_t replace[static 1]) {
  mrln_str8_t str = get_lorem();

  bench_timer_t timer = {};

  bench_timer_start(&timer);
  int err = BENCH_findallpcome3_replace(&str, target, replace);
  bench_timer_end(&timer);

  lazy_error(err);

  mrln_str8_destroy(&str);

  return timer;
}
