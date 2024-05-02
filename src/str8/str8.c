#include <asm-generic/errno-base.h>
#include <complex.h>
#include <ctype.h>
#include <merlin/aloctr.h>
#include <merlin/str8.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"

int mrln_str8_reserve(mrln_str8_t s[static 1], const intptr_t capacity,
                      mrln_aloctr_t *a) {
  if (s->capacity >= capacity) {
    return 0;
  }
  return mrln_aloctr(a, (void **)&s->buffer, &s->capacity, capacity);
}

static int internal_grow(mrln_str8_t s[static 1], const intptr_t min_capacity,
                         mrln_aloctr_t *a) {
  if (s->length + min_capacity >= s->capacity) {
    return mrln_aloctr(a, (void **)&s->buffer, &s->capacity,
                       MAX(s->capacity << 1, min_capacity));
  }

  return 0;
}

int mrln_str8_shrink(mrln_str8_t s[static 1], mrln_aloctr_t *a) {
  return mrln_aloctr(a, (void **)&s->buffer, &s->capacity, s->length);
}

void mrln_str8_destroy(mrln_str8_t s[static 1], mrln_aloctr_t *a) {
  (void)mrln_aloctr(a, (void **)&s->buffer, &s->capacity, 0);
  s->length = 0;
}

intptr_t mrln_str8_find(const mrln_str8view_t haystack[static 1],
                        const mrln_str8view_t needle[static 1]) {
  mrln_str8view_t result =
      (mrln_str8view_t){.buffer = haystack->buffer, .length = needle->length};
  for (intptr_t i = 0; i < haystack->length - needle->length; ++i) {
    const bool f = mrln_str8view_is_equal(needle, &result);
    if (f) {
      return i;
    }
    result.buffer += 1;
  }
  return haystack->length;
}

void mrln_str8_split_at_view(const mrln_str8view_t s[static 1],
                             const mrln_str8view_t delim[static 1],
                             mrln_str8view_t lower[static 1],
                             mrln_str8view_t upper[static 1]) {
  const intptr_t index = mrln_str8_find(s, delim);
  if (index < s->length) {
    mrln_str8_split_at_index(s, index, lower, upper);
    upper->buffer += delim->length;
    upper->length -= delim->length;
  } else {
    *lower = (mrln_str8view_t){};
    *upper = *s;
  }
}

intptr_t mrln_str8_find_char(const mrln_str8view_t haystack[static 1],
                             const char needle) {

  for (intptr_t i = 0; i < haystack->length; ++i) {
    if (haystack->buffer[i] == needle) {
      return i;
    }
  }
  return haystack->length;
}

void mrln_str8_split_at_char(const mrln_str8view_t s[static 1],
                             const uint8_t delim,
                             mrln_str8view_t lower[static 1],
                             mrln_str8view_t upper[static 1]) {
  const intptr_t index = mrln_str8_find_char(s, delim);
  if (index < s->length) {
    mrln_str8_split_at_index(s, index, lower, upper);
    upper->length -= 1;
    upper->buffer += 1;
  } else {
    *lower = (mrln_str8view_t){};
    *upper = *s;
  }
}

void mrln_str8_split_at_index(const mrln_str8view_t s[static 1],
                              const intptr_t index,
                              mrln_str8view_t lower[static 1],
                              mrln_str8view_t upper[static 1]) {
  const intptr_t i = index < s->length ? index : s->length;
  *lower = (mrln_str8view_t){
      .length = i,
      .buffer = s->buffer,
  };
  *upper = (mrln_str8view_t){.length = s->length - i, .buffer = s->buffer + i};
}

int mrln_str8_insert(mrln_str8_t s[static 1], intptr_t index,
                     mrln_str8view_t view[static 1], mrln_aloctr_t *a) {
  int err = internal_grow(s, s->length + view->length, a);
  if (err) {
    return err;
  }

  __builtin_memmove(&s->buffer[index + view->length], &s->buffer[index],
                    view->length);
  __builtin_memcpy(&s->buffer[index], view->buffer, view->length);
  s->length += view->length;
  return 0;
}

void mrln_str8_remove(mrln_str8_t s[static 1],
                      const mrln_str8view_t view[static 1]) {
  const intptr_t index = view->buffer - s->buffer;
  __builtin_memmove(&s->buffer[index], &s->buffer[index + view->length],
                    s->length - index - view->length);
  s->length -= view->length;
}

int mrln_str8_copy(mrln_str8_t dest[static 1], mrln_str8_t src[static 1],
                   mrln_aloctr_t *a) {
  int err = internal_grow(dest, src->capacity, a);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src->buffer, src->length);
  dest->length = src->length;
  return 0;
}

int mrln_str8_from_str8(mrln_str8_t dest[static 1],
                        const mrln_str8_t src[static 1], mrln_aloctr_t *a) {
  int err = internal_grow(dest, src->length, a);
  if (UNLIKELY(err)) {
    return err;
  }
  memcpy(dest->buffer, src->buffer, src->length);
  dest->length = src->length;
  return 0;
}

int mrln_str8_from_cstr(mrln_str8_t dest[static 1], const char src[static 1],
                        mrln_aloctr_t *a) {
  const intptr_t src_length = strlen(src);
  int err = internal_grow(dest, src_length, a);
  if (UNLIKELY(err)) {
    return err;
  }
  memcpy(dest->buffer, src, src_length);
  dest->length = src_length;
  return 0;
}

int mrln_str8_from_view(mrln_str8_t s[static 1],
                        const mrln_str8view_t view[static 1],
                        mrln_aloctr_t *a) {
  int err = internal_grow(s, view->length, a);
  if (UNLIKELY(err)) {
    return err;
  }
  __builtin_memcpy(s->buffer, view->buffer, view->length);
  s->length = view->length;
  return 0;
}

int mrln_str8_concat(mrln_str8_t dest[static 1],
                     const mrln_str8view_t src[static 1], mrln_aloctr_t *a) {
  int err = internal_grow(dest, dest->length + src->length, a);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer + dest->length, src->buffer, src->length);
  dest->length += src->length;
  return 0;
}

int mrln_str8_cstr(mrln_str8_t dest[static 1], const char src[static 1],
                   mrln_aloctr_t *a) {
  const intptr_t length = strlen(src);
  int err = internal_grow(dest, length, a);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src, length);
  return 0;
}

int mrln_str8_ncstr(mrln_str8_t dest[static 1], const intptr_t length,
                    const char src[length], mrln_aloctr_t *a) {

  int err = internal_grow(dest, length, a);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src, length);
  return 0;
}

static _Thread_local char concat_buffer[128] = {0};
static _Thread_local intptr_t concat_buffer_length = 0;

static uint64_t concat_u64_helper_helper(const uint64_t src,
                                         const uint64_t reminder,
                                         const intptr_t index) {
  if (reminder < 10) {
    concat_buffer[index] = (src % 10) + '0';
    concat_buffer_length += 1;
    return src / 10;
  }
  const intptr_t new_src =
      concat_u64_helper_helper(src, reminder / 10, index + 1);
  concat_buffer[index] = new_src % 10 + '0';
  concat_buffer_length += 1;
  return new_src / 10;
}

static intptr_t concat_u64_helper(const uint64_t reminder) {
  concat_buffer_length = 0;
  (void)concat_u64_helper_helper(reminder, reminder, 0);
  return concat_buffer_length;
}

int mrln_str8_concat_u64(mrln_str8_t dest[static 1], const uint64_t src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, mrln_aloctr_t *a) {
  const intptr_t fmt_length = concat_u64_helper(src);
  const intptr_t fmt_padding =
      padding_size > fmt_length ? padding_size - fmt_length : 0;

  int err = internal_grow(dest, dest->length + fmt_length + fmt_padding, a);
  if (err) {
    return err;
  }

  __builtin_memset(&dest->buffer[dest->length], padding_char, fmt_padding);
  dest->length += fmt_padding;

  __builtin_memcpy(&dest->buffer[dest->length], concat_buffer, fmt_length);
  dest->length += fmt_length;

  return 0;
}

int mrln_str8_concat_i64(mrln_str8_t dest[static 1], const int64_t src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, mrln_aloctr_t *a) {
  int isneg = 0;
  uint64_t reminder = src;
  if (src < 0) {
    reminder = -1 * src;
    isneg = 1;
  }

  const intptr_t fmt_length = concat_u64_helper(reminder);

  intptr_t fmt_padding =
      padding_size > fmt_length ? padding_size - fmt_length : 0;

  int err =
      internal_grow(dest, dest->length + fmt_padding + fmt_length + isneg, a);
  if (err) {
    return err;
  }

  __builtin_memset(&dest->buffer[dest->length], padding_char, fmt_padding);
  dest->length += fmt_padding;

  __builtin_memcpy(&dest->buffer[dest->length], concat_buffer, fmt_length);
  dest->length += fmt_length;
  return 0;
}

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @param(factory): factory used to allocate `dest`
 * @param(n_decimal): number of decimal digits formatted
 * @return: error
 */
int mrln_str8_concat_f64(mrln_str8_t dest[static 1], const double src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, const uint64_t n_decimal,
                         mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @param(factory): factory used to allocate `dest`
 * @param(n_decimal): number of decimal digits formatted
 * @return: error
 */
int mrln_str8_concat_f32(mrln_str8_t dest[static 1], const float src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, const uint64_t n_decimal,
                         mrln_aloctr_t *a);

int mrln_str8_concat_char(mrln_str8_t dest[static 1], const char c,
                          mrln_aloctr_t *a) {
  int err = internal_grow(dest, dest->length + 1, a);
  if (err) {
    return err;
  }
  dest->buffer[dest->length] = c;
  dest->length += 1;
  return 0;
}

void mrln_str8_clear(mrln_str8_t dest[static 1]) { dest->length = 0; }

void mrln_st8_trim_left(mrln_str8view_t s[static 1]) {
  // TODO(ben): this can be done with simd: mask all whitespace, get mask, mask
  // + 1, builtin_ctz from mask is number of whitespace
  while (s->length > 0 && isspace(*s->buffer)) {
    s->buffer += 1;
    s->length -= 1;
  }
}

void mrln_st8_trim_right(mrln_str8view_t s[static 1]) {
  while (s->length > 0 && isspace(s->buffer[s->length - 1])) {
    s->length -= 1;
  }
}

void mrln_st8_trim(mrln_str8view_t s[static 1]) {
  mrln_st8_trim_left(s);
  mrln_st8_trim_right(s);
}

void mrln_str8view_cut_front(mrln_str8view_t s[static 1]) {
  mrln_str8view_cut_n_front(s, 1);
}

void mrln_str8view_cut_n_front(mrln_str8view_t s[static 1], const intptr_t n) {
  const intptr_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
  s->buffer += m;
}

void mrln_str8view_cut_end(mrln_str8view_t s[static 1]) {
  mrln_str8view_cut_n_end(s, 1);
}

void mrln_str8view_cut_n_end(mrln_str8view_t s[static 1], const intptr_t n) {
  const intptr_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
}

int mrln_str8view_compare(const mrln_str8view_t a[static 1],
                          const mrln_str8view_t b[static 1]) {
  const intptr_t length = a->length < b->length ? a->length : b->length;
  int cmp = __builtin_memcmp(a->buffer, b->buffer, length);
  return cmp == 0 ? 0 : a->length < b->length ? -1 : 1;
}

bool mrln_str8view_is_equal(const mrln_str8view_t a[static 1],
                            const mrln_str8view_t b[static 1]) {
  if (a->length != b->length) {
    return 0;
  }
  return __builtin_memcmp(a->buffer, b->buffer, a->length) == 0;
}
