#ifndef MERLIN_STR8_H
#define MERLIN_STR8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct merlin_str8_t merlin_str8_t;
struct merlin_str8_t {
  uint8_t *buffer;
  intptr_t length;
  intptr_t capacity;
};

typedef struct merlin_str8_pview_t merlin_str8_pview_t;
struct merlin_str8_pview_t {
  const merlin_str8_t *str;
  intptr_t index;
  intptr_t length;
};

typedef struct merlin_str8_view_t merlin_str8_view_t;
struct merlin_str8_view_t {
  const uint8_t *buffer;
  intptr_t length;
};

/*** doc
 * returns a persistent version of the non owning view `view` over str8 `s`.
 * @param(view): must be view into `s`
 */
merlin_str8_pview_t
merlin_str8_view_get_pview(const merlin_str8_t s[static 1],
                           const merlin_str8_view_t v[static 1]);

/*** doc
 * returns reconcstructed normal view from persistent view
 */
merlin_str8_view_t
merlin_str8_pview_get_view(const merlin_str8_pview_t pview[static 1]);

/*** doc
 * @desription: reserve space for later writes
 * @return: error
 */
int merlin_str8_reserve(merlin_str8_t s[static 1], const intptr_t capacity);

/*** doc
 * @description: shrinks the allocated space for `s` to fit elements in `s`
 * @return: error
 */
int merlin_str8_shrink(merlin_str8_t s[static 1]);

#define merlin_str8_view_from_static_cstr(CSTR)                                \
  (merlin_str8_view_t) { .buffer = (uint8_t *)CSTR, .length = sizeof(CSTR) - 1 }

#define merlin_str8_view_from_ncstr(CSTR, LEN)                                 \
  (merlin_str8_view_t) { .buffer = (uint8_t *)CSTR, .length = LEN }

/*** doc
 * @description: deallocates `s`
 */
void merlin_str8_destroy(merlin_str8_t s[static 1]);

/*** doc
 * @description: get view into `s` at position `index` with length `length`
 * @return: view into `s`
 */
merlin_str8_view_t merlin_str8_get_view(const merlin_str8_t s[static 1],
                                        const intptr_t index,
                                        const intptr_t length);

/*** doc
 * @description: find first occurence of `needle` in `haystack`
 * @return: index first occurence
 */
intptr_t merlin_str8_find(const merlin_str8_view_t haystack[static 1],
                          const merlin_str8_view_t needle[static 1]);

/*** doc
 * @description: find first occurence of `needle` in `haystack`
 * @return: index of first occurence
 */
intptr_t merlin_str8_find_char(const merlin_str8_view_t haystack[static 1],
                               const char needle);

/*** doc
 * @description: split `s` into `lower` and `upper` at the first occurence of
 * `delim`
 * @param(delim): delimiter where `s` is split
 * @param(lower): view over lower part of `s` not including delim
 * @param(upper): view over upper part of `s` not including delim
 */
void merlin_str8_split_at_view(const merlin_str8_view_t s[static 1],
                               const merlin_str8_view_t delim[static 1],
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]);

/*** doc
 * @description: split `s` into `lower` and `upper` at the first occurence of
 * `delim`
 * @param(delim): delimiter where `s` is split
 * @param(lower): view over lower part of `s` not including delim
 * @param(upper): view over upper part of `s` not including delim
 */
void merlin_str8_split_at_char(const merlin_str8_view_t s[static 1],
                               const uint8_t delim,
                               merlin_str8_view_t lower[static 1],
                               merlin_str8_view_t upper[static 1]);

/*** doc
 * @description: split `s` into `lower` and `upper` at `index`
 * `delim`
 * @param(index): index where `s` is split
 * @param(lower): view over lower part of `s` not including char at `index`
 * @param(upper): view over upper part of `s` including char at `index`
 */
void merlin_str8_split_at_index(const merlin_str8_view_t s[static 1],
                                const intptr_t index,
                                merlin_str8_view_t lower[static 1],
                                merlin_str8_view_t upper[static 1]);

/*** doc
 * @description: replace each `target` view with `replacement` in `s`
 * @param(target): target view to be replaced by `replacement`
 * @param(replacement): replacement view for each `target`
 * @return: error
 */
int merlin_str8_replace(merlin_str8_t s[static 1],
                        const merlin_str8_view_t target[static 1],
                        const merlin_str8_view_t replacement[static 1]);

/*** doc
 * @description: replace first `n` `target` view with `replacement` in `s`
 * @param(target): target view to be replaced by `replacement`
 * @param(replacement): replacement view for each `target`
 * @param(n): number of targets to replace
 * @return: error
 */
int merlin_str8_replace_n(merlin_str8_t s[static 1],
                          const merlin_str8_view_t target[static 1],
                          const merlin_str8_view_t replacement[static 1],
                          const intptr_t n);

/*** doc
 * @description: insert `view` into `s` at `index`
 * @param(index): index where `view` is to be inserted
 * @param(view): `view` which is to be inserted
 * @return: error
 */
int merlin_str8_insert(merlin_str8_t s[static 1], intptr_t index,
                       merlin_str8_view_t view[static 1]);

/*** doc
 * @description: remove `view` from `s`
 * @param(view): view must be pointing into `s`
 */
void merlin_str8_remove(merlin_str8_t s[static 1],
                        const merlin_str8_view_t view[static 1]);

/*** doc
 * @description copy `src` into `dest`
 * @return: error
 */
int merlin_str8_copy(merlin_str8_t dest[static 1], merlin_str8_t src[static 1]);

/***
 * @description: create a new str8 from `view`
 * @param(s): string to create
 * @param(view): view from which the string is created
 * @return: error
 */
int merlin_str8_from_view(merlin_str8_t s[static 1],
                          merlin_str8_view_t view[static 1]);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int merlin_str8_concat(merlin_str8_t dest[static 1],
                       const merlin_str8_view_t src[static 1]);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int merlin_str8_cstr(merlin_str8_t dest[static 1], const char src[static 1]);

/*** doc
 * @description: concatinates `dest` with `src`
 * @return: error
 */
int merlin_str8_ncstr(merlin_str8_t dest[static 1], const intptr_t len,
                      const char src[len]);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @return: error
 */
int merlin_str8_concat_u64(merlin_str8_t dest[static 1], const uint64_t src,
                           const uint8_t padding_char,
                           const intptr_t padding_size);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
 * @return: error
 */
int merlin_str8_concat_i64(merlin_str8_t dest[static 1], const int64_t src,
                           const uint8_t padding_char,
                           const intptr_t padding_size);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @param(padding_char): char used to pad src
 * @param(padding_size): size to which `src` will be padded
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
 * @param(n_decimal): number of decimal digits formatted
 * @return: error
 */
int merlin_str8_concat_f32(merlin_str8_t dest[static 1], const float src,
                           const uint8_t padding_char,
                           const intptr_t padding_size,
                           const uint64_t n_decimal);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @return: error
 */
int merlin_str8_concat_char(merlin_str8_t dest[static 1], const char c);

/*** doc
 * @description: concatinates `dest` with formatted `src`
 * @return: error
 */
void merlin_str8_clear(merlin_str8_t dest[static 1]);

/*** doc
 * @description: returns s without whitespace at the beginning
 * @retun: `s` without whitespace at the beginning
 */
void merlin_st8_trim_left(merlin_str8_view_t s[static 1]);

/*** doc
 * @description: returns s without whitespace at end
 * @retun: `s` without whitespace at the end
 */
void merlin_st8_trim_right(merlin_str8_view_t s[static 1]);

/*** doc
 * @description: returns s without whitespace at beginning and end
 * @retun: `s` without whitespace at the beginning and end
 */
void merlin_st8_trim(merlin_str8_view_t s[static 1]);

/*** doc
 * @description: cut the first char from view `s`
 * @return: `s` without first char
 * */
void merlin_str8_view_cut_front(merlin_str8_view_t s[static 1]);

/*** doc
 * @description: cut the first `n` chars from view `s`
 * @return: `s` without first `n` chars
 * */
void merlin_str8_view_cut_n_front(merlin_str8_view_t s[static 1],
                                  const intptr_t n);

/*** doc
 * @description: cut the last char from view `s`
 * @return: `s` without last char
 * */
void merlin_str8_view_cut_end(merlin_str8_view_t s[static 1]);

/*** doc
 * @description: cut the last `n` chars from view `s`
 * @return: `s` without last `n` chars
 * */
void merlin_str8_view_cut_n_end(merlin_str8_view_t s[static 1],
                                const intptr_t n);

/*** doc
 * @description: compares `a` and `b` similar to `strcmp`
 * return: difference of first char
 */
int merlin_str8_view_compare(const merlin_str8_view_t a[static 1],
                             const merlin_str8_view_t b[static 1]);

/*** doc
 * @description: returns `1` if `a` and `b` are equal, returns false otherwise
 */
bool merlin_str8_view_is_equal(const merlin_str8_view_t a[static 1],
                               const merlin_str8_view_t b[static 1]);

#endif // MERLIN_STR8_H
