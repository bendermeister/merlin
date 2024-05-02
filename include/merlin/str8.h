#ifndef MRLN_STR8_H
#define MRLN_STR8_H

#include <merlin/aloctr.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct mrln_str8_t mrln_str8_t;
struct mrln_str8_t {
  uint8_t *buffer;
  intptr_t length;
  intptr_t capacity;
} __attribute__((__aligned__(32)));

typedef struct mrln_str8view_t mrln_str8view_t;
struct mrln_str8view_t {
  const uint8_t *buffer;
  intptr_t length;
} __attribute__((__aligned__(16)));

__attribute__((__always_inline__, __const__)) static inline mrln_str8view_t
mrln_str8view_from_view(const mrln_str8view_t a) {
  return (mrln_str8view_t){.buffer = a.buffer, .length = a.length};
}

__attribute__((__always_inline__, __nonnull__(1))) static inline mrln_str8view_t
mrln_str8view_from_viewptr(const mrln_str8view_t *a) {
  return (mrln_str8view_t){.buffer = a->buffer, .length = a->length};
}

__attribute__((__always_inline__, __const__)) static inline mrln_str8view_t
mrln_str8view_from_str8(const mrln_str8_t a) {
  return (mrln_str8view_t){.buffer = a.buffer, .length = a.length};
}

__attribute__((__always_inline__, __nonnull__(1))) static inline mrln_str8view_t
mrln_str8view_from_str8ptr(const mrln_str8_t *a) {
  return (mrln_str8view_t){.buffer = a->buffer, .length = a->length};
}

__attribute__((__always_inline__, __nonnull__(1))) static inline mrln_str8view_t
mrln_str8view_from_cstr(const char *a) {
  return (mrln_str8view_t){.buffer = (const uint8_t *)a,
                           .length = (intptr_t)__builtin_strlen(a)};
}

#define mrln_str8view(ARG)                                                     \
  _Generic(ARG,                                                                \
      mrln_str8view_t: mrln_str8view_from_view,                                \
      mrln_str8view_t *: mrln_str8view_from_viewptr,                           \
      mrln_str8_t: mrln_str8view_from_str8,                                    \
      mrln_str8_t *: mrln_str8view_from_str8ptr,                               \
      char *: mrln_str8view_from_cstr,                                         \
      const char *: mrln_str8view_from_cstr)(ARG)

int mrln_str8_from_cstr(mrln_str8_t dest[static 1], const char src[static 1],
                        mrln_aloctr_t *a);

int mrln_str8_from_view(mrln_str8_t dest[static 1],
                        const mrln_str8view_t src[static 1], mrln_aloctr_t *a);

__attribute__((__always_inline__)) static inline int
mrln_str8_from_viewlit(mrln_str8_t dest[static 1], const mrln_str8view_t view,
                       mrln_aloctr_t *a) {
  return mrln_str8_from_view(dest, &view, a);
}

int mrln_str8_from_str8(mrln_str8_t dest[static 1],
                        const mrln_str8_t src[static 1], mrln_aloctr_t *a);

__attribute__((__always_inline__)) static inline int
mrln_str8_from_str8lit(mrln_str8_t dest[static 1], const mrln_str8_t src,
                       mrln_aloctr_t *a) {
  return mrln_str8_from_str8(dest, &src, a);
}

#define mrln_str8(DEST, SRC, A)                                                \
  ({                                                                           \
    mrln_str8_t *mrln_dest_ = DEST;                                            \
    mrln_aloctr_t *mrln_a_ = A;                                                \
    _Generic(SRC,                                                              \
        mrln_str8view_t: mrln_str8_from_viewlit,                               \
        mrln_str8view_t *: mrln_str8_from_view,                                \
        mrln_str8_t: mrln_str8_from_str8lit,                                   \
        mrln_str8_t *: mrln_str8_from_str8,                                    \
        char *: mrln_str8_from_cstr,                                           \
        const char *: mrln_str8_from_cstr)(mrln_dest_, SRC, mrln_a_);          \
  })

/*** doc
 * @desription: reserve space for later writes
 * @return: error
 */
int mrln_str8_reserve(mrln_str8_t s[static 1], const intptr_t capacity,
                      mrln_aloctr_t *a);

/*** doc
 * @description: shrinks the allocated space for `s` to fit elements in
 * `s`
 * @return: error
 */
int mrln_str8_shrink(mrln_str8_t s[static 1], mrln_aloctr_t *a);

/*** doc
 * @description: deallocates `s`
 */
void mrln_str8_destroy(mrln_str8_t s[static 1], mrln_aloctr_t *a);

/*** doc
 * @description: get view into `s` at position `index` with length
 * `length`
 * @return: view into `s`
 */
mrln_str8view_t mrln_str8_get_view(const mrln_str8_t s[static 1],
                                   const intptr_t index, const intptr_t length);

/*** doc
 * @description: find first occurence of `needle` in `haystack`
 * @return: index first occurence
 */
intptr_t mrln_str8_find(const mrln_str8view_t haystack[static 1],
                        const mrln_str8view_t needle[static 1]);

/*** doc
 * @description: find first occurence of `needle` in `haystack`
 * @return: index of first occurence
 */
intptr_t mrln_str8_find_char(const mrln_str8view_t haystack[static 1],
                             const char needle);

/*** doc
 * @description: split `s` into `lower` and `upper` at the first occurence
 * of `delim`
 * @param(delim): delimiter where `s` is split
 * @param(lower): view over lower part of `s` not including delim
 * @param(upper): view over upper part of `s` not including delim
 */
void mrln_str8_split_at_view(const mrln_str8view_t s[static 1],
                             const mrln_str8view_t delim[static 1],
                             mrln_str8view_t lower[static 1],
                             mrln_str8view_t upper[static 1]);

/*** doc
 * @description: split `s` into `lower` and `upper` at the first occurence
 * of `delim`
 * @param(delim): delimiter where `s` is split
 * @param(lower): view over lower part of `s` not including delim
 * @param(upper): view over upper part of `s` not including delim
 */
void mrln_str8_split_at_char(const mrln_str8view_t s[static 1],
                             const uint8_t delim,
                             mrln_str8view_t lower[static 1],
                             mrln_str8view_t upper[static 1]);

/*** doc
 * @description: split `s` into `lower` and `upper` at `index`
 * `delim`
 * @param(index): index where `s` is split
 * @param(lower): view over lower part of `s` not including char at
 * `index`
 * @param(upper): view over upper part of `s` including char at `index`
 */
void mrln_str8_split_at_index(const mrln_str8view_t s[static 1],
                              const intptr_t index,
                              mrln_str8view_t lower[static 1],
                              mrln_str8view_t upper[static 1]);

/*** doc
 * @description: replace each `target` view with `replacement` in `s`
 * @param(target): target view to be replaced by `replacement`
 * @param(replacement): replacement view for each `target`
 * @return: error
 */
int mrln_str8_replace(mrln_str8_t s[static 1],
                      const mrln_str8view_t target[static 1],
                      const mrln_str8view_t replacement[static 1],
                      mrln_aloctr_t *a);

/*** doc
 * @description: replace first `n` `target` view with `replacement` in `s`
 * @param(target): target view to be replaced by `replacement`
 * @param(replacement): replacement view for each `target`
 * @param(n): number of targets to replace
 * @return: error
 */
int mrln_str8_replace_n(mrln_str8_t s[static 1],
                        const mrln_str8view_t target[static 1],
                        const mrln_str8view_t replacement[static 1],
                        const intptr_t n, mrln_aloctr_t *a);

/*** doc
 * @description: insert `view` into `s` at `index`
 * @param(index): index where `view` is to be inserted
 * @param(view): `view` which is to be inserted
 * @return: error
 */
int mrln_str8_insert(mrln_str8_t s[static 1], intptr_t index,
                     mrln_str8view_t view[static 1], mrln_aloctr_t *a);

/*** doc
 * @description: remove `view` from `s`
 * @param(view): view must be pointing into `s`
 */
void mrln_str8_remove(mrln_str8_t s[static 1],
                      const mrln_str8view_t view[static 1]);

/*** doc
 * @description copy `src` into `dest`
 * @return: error
 */
int mrln_str8_copy(mrln_str8_t dest[static 1], mrln_str8_t src[static 1],
                   mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int mrln_str8_concat(mrln_str8_t dest[static 1],
                     const mrln_str8view_t src[static 1], mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int mrln_str8_cstr(mrln_str8_t dest[static 1], const char src[static 1],
                   mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int mrln_str8_ncstr(mrln_str8_t dest[static 1], const intptr_t len,
                    const char src[len], mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @return: error
 */
int mrln_str8_concat_u64(mrln_str8_t dest[static 1], const uint64_t src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @return: error
 */
int mrln_str8_concat_i64(mrln_str8_t dest[static 1], const int64_t src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
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
 * @param(n_decimal): number of decimal digits formatted
 * @return: error
 */
int mrln_str8_concat_f32(mrln_str8_t dest[static 1], const float src,
                         const uint8_t padding_char,
                         const intptr_t padding_size, const uint64_t n_decimal,
                         mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @return: error
 */
int mrln_str8_concat_char(mrln_str8_t dest[static 1], const char c,
                          mrln_aloctr_t *a);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @return: error
 */
void mrln_str8_clear(mrln_str8_t dest[static 1]);

/*** doc
 * @description: returns s without whitespace at the beginning
 * @retun: `s` without whitespace at the beginning
 */
void mrln_st8_trim_left(mrln_str8view_t s[static 1]);

/*** doc
 * @description: returns s without whitespace at end
 * @retun: `s` without whitespace at the end
 */
void mrln_st8_trim_right(mrln_str8view_t s[static 1]);

/*** doc
 * @description: returns s without whitespace at beginning and end
 * @retun: `s` without whitespace at the beginning and end
 */
void mrln_st8_trim(mrln_str8view_t s[static 1]);

/*** doc
 * @description: cut the first char from view `s`
 * @return: `s` without first char
 * */
void mrln_str8view_cut_front(mrln_str8view_t s[static 1]);

/*** doc
 * @description: cut the first `n` chars from view `s`
 * @return: `s` without first `n` chars
 * */
void mrln_str8view_cut_n_front(mrln_str8view_t s[static 1], const intptr_t n);

/*** doc
 * @description: cut the last char from view `s`
 * @return: `s` without last char
 * */
void mrln_str8view_cut_end(mrln_str8view_t s[static 1]);

/*** doc
 * @description: cut the last `n` chars from view `s`
 * @return: `s` without last `n` chars
 * */
void mrln_str8view_cut_n_end(mrln_str8view_t s[static 1], const intptr_t n);

/*** doc
 * @description: compares `a` and `b` similar to `strcmp`
 * return: difference of first char
 */
int mrln_str8view_compare(const mrln_str8view_t a[static 1],
                          const mrln_str8view_t b[static 1]);

/*** doc
 * @description: returns `1` if `a` and `b` are equal, returns false
 * otherwise
 */
bool mrln_str8view_is_equal(const mrln_str8view_t a[static 1],
                            const mrln_str8view_t b[static 1]);
#endif // MRLN_STR8_H
