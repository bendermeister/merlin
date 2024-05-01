#include "../util.h"

#include <merlin/simd.h>
#include <merlin/str8.h>

#include <asm-generic/errno-base.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct result_t result_t;
struct result_t {
  intptr_t *buffer;
  intptr_t capacity;
  intptr_t length;
  int error;
};

/**
 * # main algorithm for replacing things
 * - find all occrences of the target/needle in the haystack
 * - calculate needed buffer size for a new string with the replacements
 * - allocate this new string
 * - iterate over needle positions while memcpying into the new string
 *
 * ## caveats
 * - if target.length == replacement.length replace in place
 * - if target.length == replacement.length == 1, replace with SIMD in place
 *
 * # algorithm for finding needles
 * - iterate over the string with 5 offsets:
 *   - offset 0: 0
 *   - offset 1: 1
 *   - offset 2: needle.length / 2
 *   - offset 3: needle.length - 2
 *   - offset 4: needle.length - 1
 * - check if the chars at the given offset match
 *   the chars in the needle at the given offset
 * - if this is the case: store them in a buffer for later
 * - after finding all possible positions iterate over them and check with
 *   memcmp
 *
 * ## caveats
 * - if needle->length <= 5 call a seperate function that doesn't need to check
 *   with memcmp
 */

__attribute__((__always_inline__, __nonnull__(1))) static int
result_grow_to(result_t result[static 1], const intptr_t new_cap) {
  ASSUME(result != NULL);
  ASSUME(result->capacity < new_cap);

  void *buffer = realloc(result->buffer, new_cap * sizeof(*result->buffer));

  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }

  result->buffer = buffer;
  result->capacity = new_cap;
  return 0;
}

static result_t find_2(const merlin_str8_view_t haystack[static 1],
                       const merlin_str8_view_t needle[static 1]) {
  ASSUME(haystack != NULL);
  ASSUME(needle != NULL);
  ASSUME(haystack->length > needle->length);
  ASSUME(needle->length == 2);

  result_t result = { .error = ENOMEM, .capacity = 16 };

  const intptr_t v32u8_end = ((haystack->length - 1) / 32) * 32;
  intptr_t index = 0;

  merlin_v32u8_t r0, r1, m0, m1, t;
  m0 = merlin_v32u8_set1(needle->buffer[0]);
  m1 = merlin_v32u8_set1(needle->buffer[1]);
  while (index < v32u8_end) {
    int err = result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    for (; index < v32u8_end && result.length < result_end; index += 32) {
      r0 = merlin_v32u8_load_unaligned((void*)(haystack->buffer + 0 + index));
      r0 = merlin_v32u8_cmpeq(r0, m0);
      r1 = merlin_v32u8_load_unaligned((void*)(haystack->buffer + 1 + index));
      r1 = merlin_v32u8_cmpeq(r1, m1);
      t  = merlin_v32u8_and(r0, r1);
      uint32_t mask = merlin_v32u8_mask(t);
      intptr_t index_offset = 0;
      while (mask) {
        int ctz = __builtin_ctz(mask);
        index_offset += ctz;
        mask >>= ctz;
        result.buffer[result.length++] = index + index_offset;
        mask >>= 1;
        index_offset += 1;
      }
    }
  }

  if (result.length + 32 > result.capacity) {
    int err = result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length - 1; ++index) {
    result.buffer[result.length] = index;
    result.length += __builtin_memcmp(haystack->buffer + index, needle->buffer, 2) == 0;
  }

  result.error = 0;
  return result;
}

static result_t find_3(const merlin_str8_view_t haystack[static 1],
                       const merlin_str8_view_t needle[static 1]) {

  result_t result = {.capacity = 16, .error = ENOMEM};

  const intptr_t v32u8_end = ((haystack->length - 2) / 32) * 32;
  intptr_t index = 0;

  // 256
  {
    merlin_v32u8_t r0, r1, r2, m0, m1, m2, t;
    m0 = merlin_v32u8_set1(needle->buffer[0]);
    m1 = merlin_v32u8_set1(needle->buffer[1]);
    m2 = merlin_v32u8_set1(needle->buffer[2]);
    for (; index < v32u8_end;) {
      int err = result_grow_to(&result, result.capacity << 1);
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

  if (result.length + 32 > result.capacity) {
    int err = result_grow_to(&result, result.capacity + 32);
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

static result_t find_1(const merlin_str8_view_t haystack[static 1],
                       const merlin_str8_view_t needle[static 1]) {
  if (haystack->length < 1) {
    return (result_t){};
  }
  result_t result = {.capacity = 16, .error = ENOMEM};

  const intptr_t v32u8_end = (haystack->length / 32) * 32;

  const merlin_v32u8_t m0 = merlin_v32u8_set1(needle->buffer[0]);
  merlin_v32u8_t r0;

  intptr_t index = 0;

  while (index < v32u8_end) {
    int err = result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    for (; index < v32u8_end && result.length < result_end; index += 32) {
      r0 = merlin_v32u8_load_unaligned((void *)(haystack->buffer + index));
      r0 = merlin_v32u8_cmpeq(r0, m0);
      uint32_t mask = merlin_v32u8_mask(r0);
      intptr_t index_offset = 0;
      while (mask) {
        intptr_t ctz = __builtin_ctz(mask);
        index_offset += ctz;
        mask >>= ctz;
        result.buffer[result.length++] = index + index_offset;
        mask >>= 1;
        index_offset += 1;
      }
    }
  }

  index = v32u8_end;

  if (result.length + 32 > result.capacity) {
    int err = result_grow_to(&result, result.capacity + 32);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length; ++index) {
    result.buffer[result.length] = index;
    result.length += haystack->buffer[index] == needle->buffer[0];
  }

  result.error = 0;
  return result;
}

static result_t find_4(const merlin_str8_view_t haystack[static 1],
                       const merlin_str8_view_t needle[static 1]) {

  result_t result = {.capacity = 16, .error = ENOMEM};

  const intptr_t v32u8_end = ((haystack->length - 3) / 32) * 32;

  merlin_v32u8_t r0, r1, r2, r3, m0, m1, m2, m3, t;
  m0 = merlin_v32u8_set1(needle->buffer[0]);
  m1 = merlin_v32u8_set1(needle->buffer[1]);
  m2 = merlin_v32u8_set1(needle->buffer[2]);
  m3 = merlin_v32u8_set1(needle->buffer[3]);
  const uint8_t *in = haystack->buffer;
  intptr_t index = 0;

  while (index < v32u8_end) {
    int err = result_grow_to(&result, result.capacity << 1);
    if (UNLIKELY(err)) {
      return result;
    }
    const intptr_t result_end = result.capacity;
    for (; index < v32u8_end && result.length < result_end; index += 32) {
      r0 = merlin_v32u8_load_unaligned((void *)(in + index + 0));
      r0 = merlin_v32u8_cmpeq(r0, m0);

      r1 = merlin_v32u8_load_unaligned((void *)(in + index + 1));
      r1 = merlin_v32u8_cmpeq(r1, m1);

      r2 = merlin_v32u8_load_unaligned((void *)(in + index + 2));
      r2 = merlin_v32u8_cmpeq(r2, m2);

      r3 = merlin_v32u8_load_unaligned((void *)(in + index + 3));
      r3 = merlin_v32u8_cmpeq(r3, m3);

      t = merlin_v32u8_and(r0, r1);
      t = merlin_v32u8_and(t, r2);
      t = merlin_v32u8_and(t, r3);

      uint32_t mask = merlin_v32u8_mask(t);
      uintptr_t index_offset = 0;
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

  index = v32u8_end;

  if (result.length + 32 > result.capacity) {
    int err = result_grow_to(&result, result.capacity + 32);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length - 4; ++index) {
    result.buffer[result.length] = index;
    result.length +=
        __builtin_memcmp(haystack->buffer + index, needle->buffer, 4) == 0;
  }

  result.error = 0;
  return result;
}

static result_t find_long(const merlin_str8_view_t haystack[static 1],
                          const merlin_str8_view_t needle[static 1]) {
  if (needle->length > haystack->length) {
    return (result_t){};
  }

  result_t result = {.error = ENOMEM, .capacity = 16};

  const intptr_t v32u8_end = ((haystack->length - (needle->length - 1)) / 32) * 32;

  intptr_t index = 0;
  // 256
  {
    // r = row, b = begin, m = mask, e = end, o = offset
    const intptr_t r0o = 0;
    const intptr_t r1o = 1;
    const intptr_t r2o = needle->length - 2;
    const intptr_t r3o = needle->length - 1;
    const intptr_t r4o = needle->length / 2;
    merlin_v32u8_t r0, r1, r2, r3, r4, m0, m1, m2, m3, m4, t;
    m0 = merlin_v32u8_set1(needle->buffer[r0o]);
    m1 = merlin_v32u8_set1(needle->buffer[r1o]);
    m2 = merlin_v32u8_set1(needle->buffer[r0o]);
    m3 = merlin_v32u8_set1(needle->buffer[r1o]);
    m4 = merlin_v32u8_set1(needle->buffer[r4o]);

    while (index < v32u8_end) {
      int err = result_grow_to(&result, result.capacity << 1);
      if (UNLIKELY(err)) {
        return result;
      }

      const intptr_t result_end = result.capacity;
      for (; index < v32u8_end && result.length < result_end; index += 32) {
        r0 =
            merlin_v32u8_load_unaligned((void *)haystack->buffer + index + r0o);
        r0 = merlin_v32u8_cmpeq(r0, m0);

        r1 =
            merlin_v32u8_load_unaligned((void *)haystack->buffer + index + r1o);
        r1 = merlin_v32u8_cmpeq(r1, m1);

        r2 =
            merlin_v32u8_load_unaligned((void *)haystack->buffer + index + r2o);
        r2 = merlin_v32u8_cmpeq(r2, m2);

        r3 =
            merlin_v32u8_load_unaligned((void *)haystack->buffer + index + r3o);
        r3 = merlin_v32u8_cmpeq(r3, m3);

        r4 =
            merlin_v32u8_load_unaligned((void *)haystack->buffer + index + r4o);
        r4 = merlin_v32u8_cmpeq(r4, m4);

        t = merlin_v32u8_and(r0, r1);
        t = merlin_v32u8_and(t, r2);
        t = merlin_v32u8_and(t, r3);
        t = merlin_v32u8_and(t, r4);

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
    int err = result_grow_to(&result, result.capacity + 32);
    if (UNLIKELY(err)) {
      return result;
    }
  }

  for (; index < haystack->length - (needle->length - 1); ++index) {
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

static result_t find(const merlin_str8_view_t haystack[static 1],
                     const merlin_str8_view_t needle[static 1]) {
  switch (needle->length) {
  case 0:
  case 1:
    return find_1(haystack, needle);
  case 2:
    return find_2(haystack, needle);
  case 3:
    return find_3(haystack, needle);
  case 4:
    return find_4(haystack, needle);
  default:
    return find_long(haystack, needle);
  }
}

static int replace_inplace_1(merlin_str8_t s[static 1], const uint8_t target,
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

static int replace_inplace_1_n(merlin_str8_t s[static 1], const uint8_t target,
                               const uint8_t replacement, intptr_t n) {
  const intptr_t end = s->length;

  const intptr_t v32u8_end = (end / 32) * 32;
  const merlin_v32u8_t v32_target_mask = merlin_v32u8_set1(target);
  const merlin_v32u8_t v32_replace_mask = merlin_v32u8_set1(replacement);

  intptr_t index = 0;
  for (; index < v32u8_end && n - 32 > 0; index += 32) {
    merlin_v32u8_t data =
        merlin_v32u8_load_unaligned((void *)(s->buffer + index));
    merlin_v32u8_t mask = merlin_v32u8_cmpeq(data, v32_target_mask);
    uint32_t set = merlin_v32u8_mask(mask);
    if (set) {
      n -= __builtin_popcount(set);
      merlin_v32u8_t rmask = merlin_v32u8_and(mask, v32_replace_mask);
      mask = merlin_v32u8_not(mask);
      data = merlin_v32u8_and(data, mask);
      data = merlin_v32u8_or(data, rmask);
      merlin_v32u8_store_unaligned((void *)(s->buffer + index), data);
    }
  }

  for (; index < end && n > 0; ++index) {
    const char c = s->buffer[index];
    if (c == target) {
      s->buffer[index] = replacement;
      n -= 1;
    }
  }

  return 0;
}

static int replace_inplace_n(merlin_str8_t s[static 1],
                             const merlin_str8_view_t target[static 1],
                             const merlin_str8_view_t replacement[static 1],
                             const intptr_t n) {
  if (target->length == 1) {
    return replace_inplace_1_n(s, target->buffer[0], replacement->buffer[0], n);
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  result_t splits = find(&haystack, target);
  if (UNLIKELY(splits.error)) {
    free(splits.buffer);
    return splits.error;
  }

  const intptr_t splits_end = splits.length;
  for (intptr_t i = 0; i < splits_end && i < n; ++i) {
    const intptr_t index = splits.buffer[i];
    memcpy(s->buffer + index, replacement->buffer, replacement->length);
  }
  free(splits.buffer);
  return 0;
}

static int replace_inplace(merlin_str8_t s[static 1],
                           const merlin_str8_view_t target[static 1],
                           const merlin_str8_view_t replacement[static 1]) {
  if (target->length == 1) {
    return replace_inplace_1(s, target->buffer[0], replacement->buffer[0]);
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  result_t splits = find(&haystack, target);
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

int merlin_str8_replace(merlin_str8_t s[static 1],
                        const merlin_str8_view_t target[static 1],
                        const merlin_str8_view_t replacement[static 1]) {
  if (target->length == replacement->length) {
    return replace_inplace(s, target, replacement);
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  result_t splits = find(&haystack, target);
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

int merlin_str8_replace_n(merlin_str8_t s[static 1],
                          const merlin_str8_view_t target[static 1],
                          const merlin_str8_view_t replacement[static 1],
                          intptr_t n) {
  if (target->length == replacement->length) {
    return replace_inplace_n(s, target, replacement, n);
  }
  if (n < 0) {
    return merlin_str8_replace(s, target, replacement);
  }
  if (n == 0) {
    return 0;
  }

  merlin_str8_view_t haystack = {.buffer = s->buffer, .length = s->length};
  result_t splits = find(&haystack, target);
  if (UNLIKELY(splits.error)) {
    free(splits.buffer);
    return splits.error;
  }

  merlin_str8_t str_new = {};
  str_new.capacity = s->length + (splits.length < n ? splits.length : n) *
                                     (replacement->length - target->length);
  str_new.buffer = malloc(str_new.capacity);
  if (UNLIKELY(!str_new.buffer)) {
    return ENOMEM;
  }

  // TODO(ben): this fails if keys exists back to back and share char at pos
  // str[len - 1] and pos str[0]

  const intptr_t splits_end = splits.length;
  intptr_t previous_index = 0;
  for (intptr_t i = 0; i < splits_end && i < n; ++i) {
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
