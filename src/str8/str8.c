#include <asm-generic/errno-base.h>
#include <complex.h>
#include <ctype.h>
#include <merlin/str8.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define UNLIKELY(A) __builtin_expect((A), 0)
#else
#define UNLIKELY(A)
#endif

static int internal_realloc(merlin_str8_t s[static 1],
                            const intptr_t capacity) {
  void *buffer = realloc(s->buffer, capacity);
  if (UNLIKELY(!buffer)) {
    return ENOMEM;
  }
  s->buffer = buffer;
  s->capacity = capacity;
  return 0;
}

int merlin_str8_reserve(merlin_str8_t s[static 1], const intptr_t capacity) {
  if (s->capacity >= capacity) {
    return 0;
  }
  return internal_realloc(s, capacity);
}

static int internal_grow(merlin_str8_t s[static 1],
                         const intptr_t min_capacity) {
  if (s->length + min_capacity >= s->capacity) {
    const intptr_t capacity =
        min_capacity > s->capacity << 1 ? min_capacity : s->capacity << 1;
    return internal_realloc(s, capacity);
  }
  return 0;
}

int merlin_str8_shrink(merlin_str8_t s[static 1]) {
  return internal_realloc(s, s->length);
}

void merlin_str8_destroy(merlin_str8_t s[static 1]) {
  free(s->buffer);
  *s = (merlin_str8_t){};
}

merlin_str8_view_t merlin_str8_get_view(const merlin_str8_t s[static 1],
                                        const intptr_t index,
                                        const intptr_t length) {
  return (merlin_str8_view_t){.buffer = s->buffer + index, .length = length};
}

intptr_t merlin_str8_find(const merlin_str8_view_t haystack[static 1],
                          const merlin_str8_view_t needle[static 1]) {
  merlin_str8_view_t result = (merlin_str8_view_t){.buffer = haystack->buffer,
                                                   .length = needle->length};
  for (intptr_t i = 0; i < haystack->length - needle->length; ++i) {
    const bool f = merlin_str8_view_is_equal(needle, &result);
    if (f) {
      return i;
    }
    result.buffer += 1;
  }
  return haystack->length;
}

void merlin_str8_split_at_view(const merlin_str8_view_t s[static 1],
                               const merlin_str8_view_t delim[static 1],
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]) {
  const intptr_t index = merlin_str8_find(s, delim);
  if (index < s->length) {
    merlin_str8_split_at_index(s, index, lower, upper);
    upper->buffer += delim->length;
    upper->length -= delim->length;
  } else {
    *lower = (merlin_str8_view_t){};
    *upper = *s;
  }
}

intptr_t merlin_str8_find_char(const merlin_str8_view_t haystack[static 1],
                               const char needle) {

  for (intptr_t i = 0; i < haystack->length; ++i) {
    if (haystack->buffer[i] == needle) {
      return i;
    }
  }
  return haystack->length;
}

void merlin_str8_split_at_char(const merlin_str8_view_t s[static 1],
                               const uint8_t delim,
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]) {
  const intptr_t index = merlin_str8_find_char(s, delim);
  if (index < s->length) {
    merlin_str8_split_at_index(s, index, lower, upper);
    upper->length -= 1;
    upper->buffer += 1;
  } else {
    *lower = (merlin_str8_view_t){};
    *upper = *s;
  }
}

void merlin_str8_split_at_index(const merlin_str8_view_t s[static 1],
                                const intptr_t index,
                                merlin_str8_view_t lower[static 1],
                                merlin_str8_view_t upper[static 1]) {
  const intptr_t i = index < s->length ? index : s->length;
  *lower = (merlin_str8_view_t){
      .length = i,
      .buffer = s->buffer,
  };
  *upper =
      (merlin_str8_view_t){.length = s->length - i, .buffer = s->buffer + i};
}

int merlin_str8_insert(merlin_str8_t s[static 1], intptr_t index,
                       merlin_str8_view_t view[static 1]) {
  int err = internal_grow(s, s->length + view->length);
  if (err) {
    return err;
  }

  __builtin_memmove(&s->buffer[index + view->length], &s->buffer[index],
                    view->length);
  __builtin_memcpy(&s->buffer[index], view->buffer, view->length);
  s->length += view->length;
  return 0;
}

void merlin_str8_remove(merlin_str8_t s[static 1],
                        const merlin_str8_view_t view[static 1]) {
  const intptr_t index = view->buffer - s->buffer;
  __builtin_memmove(&s->buffer[index], &s->buffer[index + view->length],
                    s->length - index - view->length);
  s->length -= view->length;
}

int merlin_str8_copy(merlin_str8_t dest[static 1],
                     merlin_str8_t src[static 1]) {
  int err = internal_grow(dest, src->capacity);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src->buffer, src->length);
  dest->length = src->length;
  return 0;
}

int merlin_str8_from_view(merlin_str8_t s[static 1],
                          merlin_str8_view_t view[static 1]) {
  int err = internal_grow(s, view->length);
  if (err) {
    return err;
  }
  __builtin_memcpy(s->buffer, view->buffer, view->length);
  s->length = view->length;
  return 0;
}

int merlin_str8_concat(merlin_str8_t dest[static 1],
                       const merlin_str8_view_t src[static 1]) {
  int err = internal_grow(dest, dest->length + src->length);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer + dest->length, src->buffer, src->length);
  dest->length += src->length;
  return 0;
}

int merlin_str8_cstr(merlin_str8_t dest[static 1], const char src[static 1]) {
  const intptr_t length = strlen(src);
  int err = internal_grow(dest, length);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src, length);
  return 0;
}

int merlin_str8_ncstr(merlin_str8_t dest[static 1], const intptr_t length,
                      const char src[length]) {

  int err = internal_grow(dest, length);
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

int merlin_str8_concat_u64(merlin_str8_t dest[static 1], const uint64_t src,
                           const uint8_t padding_char,
                           const intptr_t padding_size) {
  const intptr_t fmt_length = concat_u64_helper(src);
  const intptr_t fmt_padding =
      padding_size > fmt_length ? padding_size - fmt_length : 0;

  int err = internal_grow(dest, dest->length + fmt_length + fmt_padding);
  if (err) {
    return err;
  }

  __builtin_memset(&dest->buffer[dest->length], padding_char, fmt_padding);
  dest->length += fmt_padding;

  __builtin_memcpy(&dest->buffer[dest->length], concat_buffer, fmt_length);
  dest->length += fmt_length;

  return 0;
}

int merlin_str8_concat_i64(merlin_str8_t dest[static 1], const int64_t src,
                           const uint8_t padding_char,
                           const intptr_t padding_size) {
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
      internal_grow(dest, dest->length + fmt_padding + fmt_length + isneg);
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
int merlin_str8_concat_f64(merlin_str8_t dest[static 1], const double src,
                           const uint8_t padding_char,
                           const intptr_t padding_size,
                           const uint64_t n_decimal);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @param(factory): factory used to allocate `dest`
 * @param(n_decimal): number of decimal digits formatted
 * @return: error
 */
int merlin_str8_concat_f32(merlin_str8_t dest[static 1], const float src,
                           const uint8_t padding_char,
                           const intptr_t padding_size,
                           const uint64_t n_decimal);

int merlin_str8_concat_char(merlin_str8_t dest[static 1], const char c) {
  int err = internal_grow(dest, dest->length + 1);
  if (err) {
    return err;
  }
  dest->buffer[dest->length] = c;
  dest->length += 1;
  return 0;
}

void merlin_str8_clear(merlin_str8_t dest[static 1]) { dest->length = 0; }

void merlin_st8_trim_left(merlin_str8_view_t s[static 1]) {
  // TODO(ben): this can be done with simd: mask all whitespace, get mask, mask
  // + 1, builtin_ctz from mask is number of whitespace
  while (s->length > 0 && isspace(*s->buffer)) {
    s->buffer += 1;
    s->length -= 1;
  }
}

void merlin_st8_trim_right(merlin_str8_view_t s[static 1]) {
  while (s->length > 0 && isspace(s->buffer[s->length - 1])) {
    s->length -= 1;
  }
}

void merlin_st8_trim(merlin_str8_view_t s[static 1]) {
  merlin_st8_trim_left(s);
  merlin_st8_trim_right(s);
}

void merlin_str8_view_cut_front(merlin_str8_view_t s[static 1]) {
  merlin_str8_view_cut_n_front(s, 1);
}

void merlin_str8_view_cut_n_front(merlin_str8_view_t s[static 1],
                                  const intptr_t n) {
  const intptr_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
  s->buffer += m;
}

void merlin_str8_view_cut_end(merlin_str8_view_t s[static 1]) {
  merlin_str8_view_cut_n_end(s, 1);
}

void merlin_str8_view_cut_n_end(merlin_str8_view_t s[static 1],
                                const intptr_t n) {
  const intptr_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
}

int merlin_str8_view_compare(const merlin_str8_view_t a[static 1],
                             const merlin_str8_view_t b[static 1]) {
  const intptr_t length = a->length < b->length ? a->length : b->length;
  int cmp = __builtin_memcmp(a->buffer, b->buffer, length);
  return cmp == 0 ? 0 : a->length < b->length ? -1 : 1;
}

bool merlin_str8_view_is_equal(const merlin_str8_view_t a[static 1],
                               const merlin_str8_view_t b[static 1]) {
  if (a->length != b->length) {
    return 0;
  }
  return __builtin_memcmp(a->buffer, b->buffer, a->length) == 0;
}

merlin_str8_pview_t
merlin_str8_view_get_pview(const merlin_str8_t s[static 1],
                           const merlin_str8_view_t v[static 1]) {
  return (merlin_str8_pview_t){
      .str = s, .index = v->buffer - s->buffer, .length = v->length};
}

merlin_str8_view_t
merlin_str8_pview_get_view(const merlin_str8_pview_t pview[static 1]) {
  return (merlin_str8_view_t){
      .buffer = pview->str->buffer + pview->index,
      .length = pview->length,
  };
}
