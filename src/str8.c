#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <merlin/str8.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static size_t next_32(const size_t n) { return ((n - 1) | 31) + 1; }

static void factory_dealloc(merlin_str8_t s[static 1],
                            merlin_str8_factory_t factory[static 1]) {
  const size_t index = s->capacity / 32;
  *(uint8_t **)s->buffer = factory->buffer[index];
  factory->buffer[index] = s->buffer;
}

static int factory_realloc(merlin_str8_t s[static 1], size_t capacity,
                           merlin_str8_factory_t factory[static 1]) {
  capacity = next_32(capacity);
  if (factory->capacity <= capacity) {
    void *buffer =
        realloc(factory->buffer, capacity * sizeof(*factory->buffer));
    if (!buffer) {
      return ENOMEM;
    }
    factory->capacity = capacity;
    factory->buffer = buffer;
  }
  const size_t index = capacity / 32;
  if (!factory->buffer[index]) {
    factory->buffer[index] = aligned_alloc(32, capacity);
    if (!factory->buffer[index]) {
      return ENOMEM;
    }
    __builtin_memset(factory->buffer[index], 0, sizeof(uint8_t **));
  }

  uint8_t *buffer = factory->buffer[index];
  factory->buffer[index] = *(uint8_t **)buffer;
  __builtin_memcpy(buffer, s->buffer, s->length);
  factory_dealloc(s, factory);
  s->capacity = capacity;
  s->buffer = buffer;
  return 0;
}

int merlin_str8_reserve(merlin_str8_t s[static 1], const size_t capacity,
                        merlin_str8_factory_t factory[static 1]) {
  if (s->capacity >= capacity) {
    return 0;
  }

  return factory_realloc(s, capacity, factory);
}

int merlin_str8_shrink(merlin_str8_t s[static 1],
                       merlin_str8_factory_t factory[static 1]) {
  return factory_realloc(s, s->length, factory);
}

void merlin_str8_destroy(merlin_str8_t s[static 1],
                         merlin_str8_factory_t factory[static 1]) {
  factory_dealloc(s, factory);
  *s = (merlin_str8_t){};
}

merlin_str8_view_t merlin_str8_get_view(const merlin_str8_t s[static 1],
                                        const size_t index,
                                        const size_t length) {
  return (merlin_str8_view_t){.index = index, .length = length};
}

size_t merlin_str8_find(const merlin_str8_view_t haystack[static 1],
                        const merlin_str8_view_t needle[static 1]) {
  merlin_str8_view_t result =
      (merlin_str8_view_t){.index = 0, .length = needle->length};

  for (size_t i = 0; i < haystack->length - needle->length; ++i) {
    int c = merlin_str8_view_compare(needle, &result);
    if (c == 0) {
      return i;
    }
    result.index += 1;
  }
  return haystack->length;
}

void merlin_str8_split_at_view(const merlin_str8_view_t s[static 1],
                               const merlin_str8_view_t delim[static 1],
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]) {
  const size_t index = merlin_str8_find(s, delim);
  if (index < s->length) {
    merlin_str8_split_at_index(s, index, lower, upper);
    upper->index += delim->length;
    upper->length -= delim->length;
  } else {
    *lower = (merlin_str8_view_t){};
    *upper = *s;
  }
}

size_t merlin_str8_find_char(const merlin_str8_view_t haystack[static 1],
                             const char needle) {

  for (size_t i = 0; i < haystack->length; ++i) {
    if (haystack->str->buffer[haystack->index] == needle) {
      return i;
    }
  }
  return haystack->length;
}

void merlin_str8_split_at_char(const merlin_str8_view_t s[static 1],
                               const uint8_t delim,
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]) {
  const size_t index = merlin_str8_find_char(s, delim);
  if (index < s->length) {
    merlin_str8_split_at_index(s, index, lower, upper);
    upper->length -= 1;
    upper->index += 1;
  } else {
    *lower = (merlin_str8_view_t){};
    *upper = *s;
  }
}

void merlin_str8_split_at_index(const merlin_str8_view_t s[static 1],
                                const size_t index,
                                merlin_str8_view_t lower[static 1],
                                merlin_str8_view_t upper[static 1]) {
  const size_t i = index < s->length ? index : s->length;
  *lower = (merlin_str8_view_t){
      .length = s->length - i,
      .index = 0,
  };
  *upper =
      (merlin_str8_view_t){.length = s->length - lower->length, .index = i};
}

int merlin_str8_replace_view(merlin_str8_t s[static 1],
                             const merlin_str8_view_t target[static 1],
                             const merlin_str8_view_t replacement[static 1],
                             merlin_str8_factory_t factory[static 1]) {
  merlin_str8_t new = (merlin_str8_t){};

  merlin_str8_view_t lower = (merlin_str8_view_t){};
  merlin_str8_view_t upper = merlin_str8_get_view(s, 0, s->length);
  merlin_str8_split_at_view(&upper, target, &lower, &upper);
  int err = 0;
  while (lower.length > 0) {
    err |= merlin_str8_concat(&new, &lower, factory);
    err |= merlin_str8_concat(&new, replacement, factory);
    err |= merlin_str8_concat(&new, &upper, factory);
    if (err) {
      return err;
    }
  }
  err |= merlin_str8_concat(&new, replacement, factory);
  err |= merlin_str8_concat(&new, &upper, factory);
  if (err) {
    return err;
  }
  factory_dealloc(s, factory);
  *s = new;
  return 0;
}

int merlin_str8_replace_n_view(merlin_str8_t s[static 1],
                               const merlin_str8_view_t target[static 1],
                               const merlin_str8_view_t replacement[static 1],
                               const size_t n,
                               merlin_str8_factory_t factory[static 1]) {
  // TODO(ben): own function for target.length == replacement.length
  merlin_str8_t new = (merlin_str8_t){};

  merlin_str8_view_t lower = (merlin_str8_view_t){};
  merlin_str8_view_t upper = merlin_str8_get_view(s, 0, s->length);
  merlin_str8_split_at_view(&upper, target, &lower, &upper);
  int err = 0;
  for (size_t i = 0; i < n && lower.length > 0; ++i) {
    err |= merlin_str8_concat(&new, &lower, factory);
    err |= merlin_str8_concat(&new, replacement, factory);
    err |= merlin_str8_concat(&new, &upper, factory);
    if (err) {
      return err;
    }
  }
  err |= merlin_str8_concat(&new, &lower, factory);
  err |= merlin_str8_concat(&new, replacement, factory);
  err |= merlin_str8_concat(&new, &upper, factory);
  if (err) {
    return err;
  }
  factory_dealloc(s, factory);
  *s = new;
  return 0;
}

int merlin_str8_insert(merlin_str8_t s[static 1], size_t index,
                       merlin_str8_view_t view[static 1],
                       merlin_str8_factory_t factory[static 1]) {
  int err = merlin_str8_reserve(s, s->length + view->length, factory);
  if (err) {
    return err;
  }

  __builtin_memmove(&s->buffer[index + view->length], &s->buffer[index],
                    view->length);
  __builtin_memcpy(&s->buffer[index], &view->str->buffer[view->index],
                   view->length);
  s->length += view->length;
  return 0;
}

void merlin_str8_remove(merlin_str8_t s[static 1],
                        const merlin_str8_view_t view[static 1]) {
  const size_t index = view->index;
  __builtin_memmove(&s->buffer[index], &s->buffer[index + view->length],
                    s->length - index - view->length);
  s->length -= view->length;
}

int merlin_str8_copy(merlin_str8_t dest[static 1], merlin_str8_t src[static 1],
                     merlin_str8_factory_t factory[static 1]) {
  int err = merlin_str8_reserve(dest, src->capacity, factory);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src->buffer, src->length);
  dest->length = src->length;
  return 0;
}

int merlin_str8_from_view(merlin_str8_t s[static 1],
                          merlin_str8_view_t view[static 1],
                          merlin_str8_factory_t factory[static 1]) {
  int err = merlin_str8_reserve(s, view->length, factory);
  if (err) {
    return err;
  }
  __builtin_memcpy(s->buffer, &view->str->buffer[view->index], view->length);
  s->length = view->length;
  return 0;
}

int merlin_str8_concat(merlin_str8_t dest[static 1],
                       const merlin_str8_view_t src[static 1],
                       merlin_str8_factory_t factory[static 1]) {
  int err = merlin_str8_reserve(dest, dest->length + src->length, factory);
  if (err) {
    return err;
  }
  __builtin_memcpy(&dest->buffer[dest->length], &src->str->buffer[src->index],
                   src->length);
  dest->length += src->length;
  return 0;
}

int merlin_str8_cstr(merlin_str8_t dest[static 1], const char src[static 1],
                     merlin_str8_factory_t factory[static 1]) {
  const size_t length = strlen(src);
  int err = merlin_str8_reserve(dest, length, factory);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src, length);
  return 0;
}

int merlin_str8_ncstr(merlin_str8_t dest[static 1], const size_t length,
                      const char src[length],
                      merlin_str8_factory_t factory[static 1]) {

  int err = merlin_str8_reserve(dest, length, factory);
  if (err) {
    return err;
  }
  __builtin_memcpy(dest->buffer, src, length);
  return 0;
}

static _Thread_local char concat_buffer[128] = {0};

size_t concat_u64_helper(const uint64_t reminder, const size_t index) {
  if (!reminder) {
    return index;
  }
  size_t res = concat_u64_helper(reminder / 10, index + 1);
  concat_buffer[index] = (reminder % 10) + '0';
  return res;
}

int merlin_str8_concat_u64(merlin_str8_t dest[static 1], const uint64_t src,
                           const uint8_t padding_char,
                           const size_t padding_size,
                           merlin_str8_factory_t factory[static 1]) {
  size_t fmt_length;
  if (src == 0) {
    concat_buffer[0] = '0';
    fmt_length = 1;
  } else {
    fmt_length = concat_u64_helper(src, 0);
  }
  const size_t fmt_padding =
      padding_size > fmt_length ? padding_size - fmt_length : 0;

  int err = merlin_str8_reserve(dest, dest->length + fmt_length + fmt_padding,
                                factory);
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
                           const size_t padding_size,
                           merlin_str8_factory_t factory[static 1]) {
  int isneg = 0;
  uint64_t reminder = src;
  if (src < 0) {
    reminder = -1 * src;
    isneg = 1;
  }

  size_t fmt_length;

  if (src != 0) {
    fmt_length = concat_u64_helper(reminder, 0);
  } else {
    fmt_length = 1;
    concat_buffer[0] = '0';
  }

  size_t fmt_padding =
      padding_size > fmt_length ? padding_size - fmt_length : 0;

  int err = merlin_str8_reserve(
      dest, dest->length + fmt_padding + fmt_length + isneg, factory);
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
                           const size_t padding_size, const uint64_t n_decimal,
                           merlin_str8_factory_t factory[static 1]);

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
                           const size_t padding_size, const uint64_t n_decimal,
                           merlin_str8_factory_t factory[static 1]);

int merlin_str8_concat_char(merlin_str8_t dest[static 1], const char c,
                            merlin_str8_factory_t factory[static 1]) {
  int err = merlin_str8_reserve(dest, dest->length + 1, factory);
  if (err) {
    return err;
  }
  dest->buffer[dest->length] = c;
  dest->length += 1;
  return 0;
}

void merlin_str8_clear(merlin_str8_t dest[static 1]) { dest->length = 0; }

void merlin_st8_trim_left(merlin_str8_view_t s[static 1]) {
  while (s->length > 0 && isspace(s->str->buffer[s->index])) {
    s->index += 1;
    s->length -= 1;
  }
}

void merlin_st8_trim_right(merlin_str8_view_t s[static 1]) {
  while (s->length > 0 && isspace(s->str->buffer[s->index + s->length - 1])) {
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
                                  const size_t n) {
  const size_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
  s->index += m;
}

void merlin_str8_view_cut_end(merlin_str8_view_t s[static 1]) {
  merlin_str8_view_cut_n_end(s, 1);
}

void merlin_str8_view_cut_n_end(merlin_str8_view_t s[static 1],
                                const size_t n) {
  const size_t m = (intptr_t)s->length - n > 0 ? n : n - s->length;
  s->length -= m;
}

int merlin_str8_view_compare(const merlin_str8_view_t a[static 1],
                             const merlin_str8_view_t b[static 1]) {
  if (a->length != b->length) {
    return (intptr_t)a->length - b->length;
  }
  return __builtin_memcmp(&a->str->buffer[a->index], &b->str->buffer[b->index],
                          a->length);
}
