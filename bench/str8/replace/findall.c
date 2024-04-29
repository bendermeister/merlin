#include <asm-generic/errno-base.h>
#include <bench/bench.h>
#include <merlin/simd.h>
#include <merlin/str8.h>
#include <stdlib.h>

#ifndef UNLIKELY
#define UNLIKELY(A) __builtin_expect((A), 0)
#endif

static void lazy_error(int error);
static merlin_str8_t get_lorem(void);

typedef struct BENCH_findall_result_t BENCH_findall_result_t;
struct BENCH_findall_result_t {
  intptr_t *buffer;
  uint32_t capacity;
  uint32_t length;
  int error;
};

__attribute__((__always_inline__, __nonnull__(1))) static int
BENCH_findall_result_grow_to(BENCH_findall_result_t result[static 1],
                             const uint32_t new_cap) {
  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static BENCH_findall_result_t
BENCH_findall_find(const merlin_str8_view_t haystack[static 1],
                   const merlin_str8_view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (BENCH_findall_result_t){};
  }

  const uint32_t last_char_offset = needle->length - 1;
  const uint8_t last_char = needle->buffer[last_char_offset];
  const uint8_t first_char = needle->buffer[0];

  BENCH_findall_result_t result = {.capacity = 16, .error = ENOMEM};

  intptr_t index = 0;
  bool keep_searching;

  // 256 bit linear find
  keep_searching = index + 32 + last_char_offset < haystack->length;
  if (keep_searching) {

    const merlin_v32u8_t row_last_char = merlin_v32u8_set1(last_char);
    const merlin_v32u8_t row_first_char = merlin_v32u8_set1(first_char);
    merlin_v32u8_t row_in;
    uint32_t mask;

    do {
      int err = BENCH_findall_result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }
      const uint32_t end = result.capacity;
      while (keep_searching && result.length < end) {
        row_in =
            merlin_v32u8_load_unaligned((void *)(haystack->buffer + index));
        row_in = merlin_v32u8_cmpeq(row_in, row_first_char);
        mask = merlin_v32u8_mask(row_in);

        row_in = merlin_v32u8_load_unaligned(
            (void *)(haystack->buffer + index + last_char_offset));
        row_in = merlin_v32u8_cmpeq(row_in, row_last_char);
        mask &= merlin_v32u8_mask(row_in);

        uint32_t index_offset = 0;
        while (mask) {
          const uint32_t ctz = __builtin_ctz(mask);
          index_offset += ctz;
          mask >>= ctz;

          if (__builtin_memcmp(haystack->buffer + index + index_offset,
                               needle->buffer, needle->length) == 0) {
            result.buffer[result.length] = index + index_offset;
            result.length += 1;
          }

          mask >>= 1;
          index_offset += 1;
        }
        index += 32;
        keep_searching = index + 32 + last_char_offset < haystack->length;
      }
    } while (keep_searching);

    index -= 32;
  }

  // 128 bit linear find
  keep_searching = index + 16 + last_char_offset < haystack->length;
  if (keep_searching) {

    const merlin_v16u8_t row_last_char = merlin_v16u8_set1(last_char);
    const merlin_v16u8_t row_first_char = merlin_v16u8_set1(first_char);
    merlin_v16u8_t row_in;
    uint32_t mask;

    if (UNLIKELY(result.length + 32 >= result.capacity)) {
      int err = BENCH_findall_result_grow_to(&result, result.capacity + 32);
      if (UNLIKELY(err)) {
        return result;
      }
    }

    row_in = merlin_v16u8_load_unaligned((void *)(haystack->buffer + index));
    row_in = merlin_v16u8_cmpeq(row_in, row_first_char);
    mask = merlin_v16u8_mask(row_in);

    row_in = merlin_v16u8_load_unaligned(
        (void *)(haystack->buffer + index + last_char_offset));
    row_in = merlin_v16u8_cmpeq(row_in, row_last_char);
    mask &= merlin_v16u8_mask(row_in);

    uint32_t index_offset = 0;
    while (mask) {
      const uint32_t ctz = __builtin_ctz(mask);
      index_offset += ctz;
      mask >>= ctz;

      if (__builtin_memcmp(haystack->buffer + index + index_offset,
                           needle->buffer, needle->length) == 0) {
        result.buffer[result.length] = index + index_offset;
        result.length += 1;
      }

      mask >>= 1;
      index_offset += 1;
    }
  }

  keep_searching = index + last_char_offset < haystack->length;

  while (keep_searching) {
    if (__builtin_memcmp(haystack->buffer + index, needle->buffer,
                         needle->length) == 0) {
      result.buffer[result.length] = index;
      result.length += 1;
    }
    index += 1;
    keep_searching = index + last_char_offset < haystack->length;
  }

  result.error = 0;
  return result;
}

static int
BENCH_findall_replace(merlin_str8_t s[static 1],
                      const merlin_str8_view_t target[static 1],
                      const merlin_str8_view_t replacement[static 1]) {

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  BENCH_findall_result_t splits = BENCH_findall_find(&haystack, target);
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

  const uint32_t splits_end = splits.length;
  uint32_t previous_index = 0;
  for (uint32_t i = 0; i < splits_end; ++i) {
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

static bench_timer_t BENCH_findall_bench(void) {
  merlin_str8_t str = get_lorem();

  bench_timer_t timer = {};

  merlin_str8_view_t target = merlin_str8_view_from_static_cstr("est");
  merlin_str8_view_t replacement =
      merlin_str8_view_from_static_cstr("bendermeister");

  bench_timer_start(&timer);
  int err = BENCH_findall_replace(&str, &target, &replacement);
  bench_timer_end(&timer);

  lazy_error(err);

  merlin_str8_destroy(&str);

  return timer;
}
