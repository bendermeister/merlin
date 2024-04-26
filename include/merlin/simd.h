#ifndef MERLIN_SIMD_H
#define MERLIN_SIMD_H

#ifndef __clang__
#error "currently only clang is supported"
#endif

#include <stddef.h>
#include <stdint.h>

#define MERLIN_VOIDFN_ATTRS __attribute__((always_inline, nodebug, unused))
#define MERLIN_NODISCARD __attribute__((warn_unused_result))
#define MERLIN_FN_ATTRS                                                        \
  MERLIN_VOIDFN_ATTRS MERLIN_NODISCARD __attribute__((const))
#define MERLIN_NONNULL(PARAM) __attribute__((nonnull(PARAM)))

#define MERLIN_ASSUME(X)                                                       \
  if (!(X))                                                                    \
  __builtin_unreachable()

//==================================mask=types==================================

typedef uint32_t merlin_mask_t;

MERLIN_FN_ATTRS static uint32_t
merlin_mask_first_set(const merlin_mask_t mask) {
  return __builtin_ctz(mask);
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_mask_t mask_t;
#endif

//==================================v16u8=======================================

typedef uint8_t merlin_v16u8_t __attribute__((vector_size(16), aligned(16)));
typedef uint8_t merlin_v16u8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_set1(const uint8_t v) {
  return (merlin_v16u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_set(const uint8_t v00, const uint8_t v01, const uint8_t v02,
                 const uint8_t v03, const uint8_t v04, const uint8_t v05,
                 const uint8_t v06, const uint8_t v07, const uint8_t v08,
                 const uint8_t v09, const uint8_t v10, const uint8_t v11,
                 const uint8_t v12, const uint8_t v13, const uint8_t v14,
                 const uint8_t v15) {
  return (merlin_v16u8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                          v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v16u8_t
    merlin_v16u8_load_aligned(const merlin_v16u8_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v16u8_t
    merlin_v16u8_load_unaligned(const merlin_v16u8_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v16u8_store_aligned(
    merlin_v16u8_t addr[static 1], const merlin_v16u8_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v16u8_store_unaligned(
    merlin_v16u8_unaligned_t addr[static 1], const merlin_v16u8_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_add(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_sub(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_mul(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_div(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_mod(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_cmpeq(const merlin_v16u8_t a, const merlin_v16u8_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_cmplt(const merlin_v16u8_t a, const merlin_v16u8_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_cmpgt(const merlin_v16u8_t a, const merlin_v16u8_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_cmpleq(const merlin_v16u8_t a, const merlin_v16u8_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_cmpgeq(const merlin_v16u8_t a, const merlin_v16u8_t b) {
  return a >= b;
}

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_and(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_or(const merlin_v16u8_t a,
                                                      const merlin_v16u8_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_xor(const merlin_v16u8_t a,
                                                       const merlin_v16u8_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v16u8_t merlin_v16u8_not(const merlin_v16u8_t a) {
  return ~a;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_shift_left(const merlin_v16u8_t a, const uint8_t n) {
  return a << n;
}

MERLIN_FN_ATTRS static merlin_v16u8_t
merlin_v16u8_shift_right(const merlin_v16u8_t a, const uint8_t n) {
  return a >> n;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v16u8_mask(const merlin_v16u8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  merlin_mask_t result = 0;
  const merlin_v16u8_t tmp = merlin_v16u8_shift_right(a, 7);
  result |= (uint16_t)tmp[0] << 0;
  result |= (uint16_t)tmp[1] << 1;
  result |= (uint16_t)tmp[2] << 2;
  result |= (uint16_t)tmp[3] << 3;
  result |= (uint16_t)tmp[4] << 4;
  result |= (uint16_t)tmp[5] << 5;
  result |= (uint16_t)tmp[6] << 6;
  result |= (uint16_t)tmp[7] << 7;
  result |= (uint16_t)tmp[8] << 8;
  result |= (uint16_t)tmp[9] << 9;
  result |= (uint16_t)tmp[10] << 10;
  result |= (uint16_t)tmp[11] << 11;
  result |= (uint16_t)tmp[12] << 12;
  result |= (uint16_t)tmp[13] << 13;
  result |= (uint16_t)tmp[14] << 14;
  result |= (uint16_t)tmp[15] << 15;
  return result;

#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v16u8_t v16u8_t;
typedef merlin_v16u8_unaligned_t v16u8_unaligned_t;

#define v16u8_set1(v) merlin_v16u8_set1(v)
#define v16u8_set(v00, v01, v02, v02, v03, v04, v05, v06, v07, v08, v09, v10,  \
                  v11, v12, v12, v13, v14, v15)                                \
  merlin_v16u8_set(v00, v01, v02, v02, v03, v04, v05, v06, v07, v08, v09, v10, \
                   v11, v12, v12, v13, v14, v15)

#define v16u8_load_aligned(addr) merlin_v16u8_load_aligned(addr)
#define v16u8_load_unaligned(addr) merlin_v16u8_load_unaligned(addr)
#define v16u8_store_aligned(addr, data) merlin_v16u8_store_aligned(addr, data)
#define v16u8_store_unaligned(addr, data)                                      \
  merlin_v16u8_store_unaligned(addr, data)

#define v16u8_add(a, b) merlin_v16u8_add(a, b)
#define v16u8_sub(a, b) merlin_v16u8_sub(a, b)
#define v16u8_mul(a, b) merlin_v16u8_mul(a, b)
#define v16u8_div(a, b) merlin_v16u8_div(a, b)
#define v16u8_mod(a, b) merlin_v16u8_mod(a, b)

#define v16u8_cmpeq(a, b) merlin_v16u8_cmpeq(a, b)
#define v16u8_cmplt(a, b) merlin_v16u8_cmplt(a, b)
#define v16u8_cmpgt(a, b) merlin_v16u8_cmpgt(a, b)
#define v16u8_cmpleq(a, b) merlin_v16u8_cmpleq(a, b)
#define v16u8_cmpgeq(a, b) merlin_v16u8_cmpgeq(a, b)

#define v16u8_and(a, b) merlin_v16u8_and(a, b)
#define v16u8_or(a, b) merlin_v16u8_or(a, b)
#define v16u8_xor(a, b) merlin_v16u8_xor(a, b)
#define v16u8_not(a) merlin_v16u8_not(a)

#define v16u8_shift_left(a, n) merlin_v16u8_shift_left(a, n)
#define v16u8_shift_right(a, n) merlin_v16u8_shift_right(a, n)

#define v16u8_mask(a) merlin_v16u8_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v16i8=======================================

typedef int8_t merlin_v16i8_t __attribute__((vector_size(16), aligned(16)));
typedef int8_t merlin_v16i8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_set1(const int8_t v) {
  return (merlin_v16i8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_set(
    const int8_t v00, const int8_t v01, const int8_t v02, const int8_t v03,
    const int8_t v04, const int8_t v05, const int8_t v06, const int8_t v07,
    const int8_t v08, const int8_t v09, const int8_t v10, const int8_t v11,
    const int8_t v12, const int8_t v13, const int8_t v14, const int8_t v15) {
  return (merlin_v16i8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                          v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v16i8_t
    merlin_v16i8_load_aligned(const merlin_v16i8_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v16i8_t
    merlin_v16i8_load_unaligned(const merlin_v16i8_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v16i8_store_aligned(
    merlin_v16i8_t addr[static 1], const merlin_v16i8_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v16i8_store_unaligned(
    merlin_v16i8_unaligned_t addr[static 1], const merlin_v16i8_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_add(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_sub(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_mul(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_div(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_mod(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v16i8_t
merlin_v16i8_cmpeq(const merlin_v16i8_t a, const merlin_v16i8_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t
merlin_v16i8_cmplt(const merlin_v16i8_t a, const merlin_v16i8_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t
merlin_v16i8_cmpgt(const merlin_v16i8_t a, const merlin_v16i8_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t
merlin_v16i8_cmpleq(const merlin_v16i8_t a, const merlin_v16i8_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t
merlin_v16i8_cmpgeq(const merlin_v16i8_t a, const merlin_v16i8_t b) {
  return a >= b;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v16i8_mask(const merlin_v16i8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  merlin_mask_t result = 0;
  const merlin_v16u8_t tmp =
      merlin_v16u8_shift_right((const merlin_v16u8_t)a, 7);
  result |= (uint16_t)tmp[0] << 0;
  result |= (uint16_t)tmp[1] << 1;
  result |= (uint16_t)tmp[2] << 2;
  result |= (uint16_t)tmp[3] << 3;
  result |= (uint16_t)tmp[4] << 4;
  result |= (uint16_t)tmp[5] << 5;
  result |= (uint16_t)tmp[6] << 6;
  result |= (uint16_t)tmp[7] << 7;
  result |= (uint16_t)tmp[8] << 8;
  result |= (uint16_t)tmp[9] << 9;
  result |= (uint16_t)tmp[10] << 10;
  result |= (uint16_t)tmp[11] << 11;
  result |= (uint16_t)tmp[12] << 12;
  result |= (uint16_t)tmp[13] << 13;
  result |= (uint16_t)tmp[14] << 14;
  result |= (uint16_t)tmp[15] << 15;
  return result;
#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v16i8_t v16i8_t;
typedef merlin_v16i8_unaligned_t v16i8_unaligned_t;

#define v16i8_set1(v) merlin_v16i8_set1(v)
#define v16i8_set(v00, v01, v02, v02, v03, v04, v05, v06, v07, v08, v09, v10,  \
                  v11, v12, v12, v13, v14, v15)                                \
  merlin_v16i8_set(v00, v01, v02, v02, v03, v04, v05, v06, v07, v08, v09, v10, \
                   v11, v12, v12, v13, v14, v15)

#define v16i8_load_aligned(addr) merlin_v16i8_load_aligned(addr)
#define v16i8_load_unaligned(addr) merlin_v16i8_load_unaligned(addr)
#define v16i8_store_aligned(addr, data) merlin_v16i8_store_aligned(addr, data)
#define v16i8_store_unaligned(addr, data)                                      \
  merlin_v16i8_store_unaligned(addr, data)

#define v16i8_add(a, b) merlin_v16i8_add(a, b)
#define v16i8_sub(a, b) merlin_v16i8_sub(a, b)
#define v16i8_mul(a, b) merlin_v16i8_mul(a, b)
#define v16i8_div(a, b) merlin_v16i8_div(a, b)
#define v16i8_mod(a, b) merlin_v16i8_mod(a, b)

#define v16i8_cmpeq(a, b) merlin_v16i8_cmpeq(a, b)
#define v16i8_cmplt(a, b) merlin_v16i8_cmplt(a, b)
#define v16i8_cmpgt(a, b) merlin_v16i8_cmpgt(a, b)
#define v16i8_cmpleq(a, b) merlin_v16i8_cmpleq(a, b)
#define v16i8_cmpgeq(a, b) merlin_v16i8_cmpgeq(a, b)

#define v16i8_mask(a) merlin_v16i8_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v8u16=======================================

typedef uint16_t merlin_v8u16_t __attribute__((vector_size(16), aligned(16)));
typedef uint16_t merlin_v8u16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_set1(const uint16_t v) {
  return (merlin_v8u16_t){v, v, v, v, v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_set(const uint16_t v00, const uint16_t v01, const uint16_t v02,
                 const uint16_t v03, const uint16_t v04, const uint16_t v05,
                 const uint16_t v06, const uint16_t v07) {
  return (merlin_v8u16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v8u16_t
    merlin_v8u16_load_aligned(const merlin_v8u16_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v8u16_t
    merlin_v8u16_load_unaligned(const merlin_v8u16_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v8u16_store_aligned(
    merlin_v8u16_t addr[static 1], const merlin_v8u16_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v8u16_store_unaligned(
    merlin_v8u16_unaligned_t addr[static 1], const merlin_v8u16_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_add(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_sub(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_mul(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_div(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_mod(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_cmpeq(const merlin_v8u16_t a, const merlin_v8u16_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_cmplt(const merlin_v8u16_t a, const merlin_v8u16_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_cmpgt(const merlin_v8u16_t a, const merlin_v8u16_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_cmpleq(const merlin_v8u16_t a, const merlin_v8u16_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_cmpgeq(const merlin_v8u16_t a, const merlin_v8u16_t b) {
  return a >= b;
}

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_and(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_or(const merlin_v8u16_t a,
                                                      const merlin_v8u16_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_xor(const merlin_v8u16_t a,
                                                       const merlin_v8u16_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v8u16_t merlin_v8u16_not(const merlin_v8u16_t a) {
  return ~a;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_shift_left(const merlin_v8u16_t a, const uint16_t n) {
  return a << n;
}

MERLIN_FN_ATTRS static merlin_v8u16_t
merlin_v8u16_shift_right(const merlin_v8u16_t a, const uint16_t n) {
  return a >> n;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v8u16_mask(const merlin_v8u16_t a) {
  merlin_mask_t result = 0;
  const merlin_v8u16_t tmp = merlin_v8u16_shift_right(a, 15);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  return result;
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v8u16_t v8u16_t;
typedef merlin_v8u16_unaligned_t v8u16_unaligned_t;

#define v8u16_set1(v) merlin_v8u16_set1(v)
#define v8u16_set(v00, v01, v02, v02, v03, v04, v05, v06, v07)                 \
  merlin_v8u16_set(v00, v01, v02, v02, v03, v04, v05, v06, v07)

#define v8u16_load_aligned(addr) merlin_v8u16_load_aligned(addr)
#define v8u16_load_unaligned(addr) merlin_v8u16_load_unaligned(addr)
#define v8u16_store_aligned(addr, data) merlin_v8u16_store_aligned(addr, data)
#define v8u16_store_unaligned(addr, data)                                      \
  merlin_v8u16_store_unaligned(addr, data)

#define v8u16_add(a, b) merlin_v8u16_add(a, b)
#define v8u16_sub(a, b) merlin_v8u16_sub(a, b)
#define v8u16_mul(a, b) merlin_v8u16_mul(a, b)
#define v8u16_div(a, b) merlin_v8u16_div(a, b)
#define v8u16_mod(a, b) merlin_v8u16_mod(a, b)

#define v8u16_cmpeq(a, b) merlin_v8u16_cmpeq(a, b)
#define v8u16_cmplt(a, b) merlin_v8u16_cmplt(a, b)
#define v8u16_cmpgt(a, b) merlin_v8u16_cmpgt(a, b)
#define v8u16_cmpleq(a, b) merlin_v8u16_cmpleq(a, b)
#define v8u16_cmpgeq(a, b) merlin_v8u16_cmpgeq(a, b)

#define v8u16_and(a, b) merlin_v8u16_and(a, b)
#define v8u16_or(a, b) merlin_v8u16_or(a, b)
#define v8u16_xor(a, b) merlin_v8u16_xor(a, b)
#define v8u16_not(a) merlin_v8u16_not(a)

#define v8u16_shift_left(a, n) merlin_v8u16_shift_left(a, n)
#define v8u16_shift_right(a, n) merlin_v8u16_shift_right(a, n)

#define v8u16_mask(a) merlin_v8u16_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v8i16=======================================

typedef int16_t merlin_v8i16_t __attribute__((vector_size(16), aligned(16)));
typedef int16_t merlin_v8i16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_set1(const int16_t v) {
  return (merlin_v8i16_t){v, v, v, v, v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_set(const int16_t v00, const int16_t v01, const int16_t v02,
                 const int16_t v03, const int16_t v04, const int16_t v05,
                 const int16_t v06, const int16_t v07) {
  return (merlin_v8i16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v8i16_t
    merlin_v8i16_load_aligned(const merlin_v8i16_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v8i16_t
    merlin_v8i16_load_unaligned(const merlin_v8i16_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v8i16_store_aligned(
    merlin_v8i16_t addr[static 1], const merlin_v8i16_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v8i16_store_unaligned(
    merlin_v8i16_unaligned_t addr[static 1], const merlin_v8i16_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_add(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_sub(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_mul(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_div(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_mod(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_cmpeq(const merlin_v8i16_t a, const merlin_v8i16_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_cmplt(const merlin_v8i16_t a, const merlin_v8i16_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_cmpgt(const merlin_v8i16_t a, const merlin_v8i16_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_cmpleq(const merlin_v8i16_t a, const merlin_v8i16_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t
merlin_v8i16_cmpgeq(const merlin_v8i16_t a, const merlin_v8i16_t b) {
  return a >= b;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v8i16_mask(const merlin_v8i16_t a) {
  merlin_mask_t result = 0;
  const merlin_v8u16_t tmp =
      merlin_v8u16_shift_right((const merlin_v8u16_t)a, 15);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  return result;
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v8i16_t v8i16_t;
typedef merlin_v8i16_unaligned_t v8i16_unaligned_t;

#define v8i16_set1(v) merlin_v8i16_set1(v)
#define v8i16_set(v00, v01, v02, v02, v03, v04, v05, v06, v07)                 \
  merlin_v8i16_set(v00, v01, v02, v02, v03, v04, v05, v06, v07)

#define v8i16_load_aligned(addr) merlin_v8i16_load_aligned(addr)
#define v8i16_load_unaligned(addr) merlin_v8i16_load_unaligned(addr)
#define v8i16_store_aligned(addr, data) merlin_v8i16_store_aligned(addr, data)
#define v8i16_store_unaligned(addr, data)                                      \
  merlin_v8i16_store_unaligned(addr, data)

#define v8i16_add(a, b) merlin_v8i16_add(a, b)
#define v8i16_sub(a, b) merlin_v8i16_sub(a, b)
#define v8i16_mul(a, b) merlin_v8i16_mul(a, b)
#define v8i16_div(a, b) merlin_v8i16_div(a, b)
#define v8i16_mod(a, b) merlin_v8i16_mod(a, b)

#define v8i16_cmpeq(a, b) merlin_v8i16_cmpeq(a, b)
#define v8i16_cmplt(a, b) merlin_v8i16_cmplt(a, b)
#define v8i16_cmpgt(a, b) merlin_v8i16_cmpgt(a, b)
#define v8i16_cmpleq(a, b) merlin_v8i16_cmpleq(a, b)
#define v8i16_cmpgeq(a, b) merlin_v8i16_cmpgeq(a, b)

#define v8i16_mask(a) merlin_v8i16_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v4u32=======================================

typedef uint32_t merlin_v4u32_t __attribute__((vector_size(16), aligned(16)));
typedef uint32_t merlin_v4u32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_set1(const uint32_t v) {
  return (merlin_v4u32_t){v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_set(const uint32_t v00,
                                                       const uint32_t v01,
                                                       const uint32_t v02,
                                                       const uint32_t v03) {
  return (merlin_v4u32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v4u32_t
    merlin_v4u32_load_aligned(const merlin_v4u32_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v4u32_t
    merlin_v4u32_load_unaligned(const merlin_v4u32_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v4u32_store_aligned(
    merlin_v4u32_t addr[static 1], const merlin_v4u32_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v4u32_store_unaligned(
    merlin_v4u32_unaligned_t addr[static 1], const merlin_v4u32_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_add(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_sub(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_mul(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_div(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_mod(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_cmpeq(const merlin_v4u32_t a, const merlin_v4u32_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_cmplt(const merlin_v4u32_t a, const merlin_v4u32_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_cmpgt(const merlin_v4u32_t a, const merlin_v4u32_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_cmpleq(const merlin_v4u32_t a, const merlin_v4u32_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_cmpgeq(const merlin_v4u32_t a, const merlin_v4u32_t b) {
  return a >= b;
}

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_and(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_or(const merlin_v4u32_t a,
                                                      const merlin_v4u32_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_xor(const merlin_v4u32_t a,
                                                       const merlin_v4u32_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v4u32_t merlin_v4u32_not(const merlin_v4u32_t a) {
  return ~a;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_shift_left(const merlin_v4u32_t a, const uint32_t n) {
  return a << n;
}

MERLIN_FN_ATTRS static merlin_v4u32_t
merlin_v4u32_shift_right(const merlin_v4u32_t a, const uint32_t n) {
  return a >> n;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v4u32_mask(const merlin_v4u32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  merlin_mask_t result = 0;
  const merlin_v4u32_t tmp = merlin_v4u32_shift_right(a, 31);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v4u32_t v4u32_t;
typedef merlin_v4u32_unaligned_t v4u32_unaligned_t;

#define v4u32_set1(v) merlin_v4u32_set1(v)
#define v4u32_set(v00, v01, v02, v02, v03)                                     \
  merlin_v4u32_set(v00, v01, v02, v02, v03)

#define v4u32_load_aligned(addr) merlin_v4u32_load_aligned(addr)
#define v4u32_load_unaligned(addr) merlin_v4u32_load_unaligned(addr)
#define v4u32_store_aligned(addr, data) merlin_v4u32_store_aligned(addr, data)
#define v4u32_store_unaligned(addr, data)                                      \
  merlin_v4u32_store_unaligned(addr, data)

#define v4u32_add(a, b) merlin_v4u32_add(a, b)
#define v4u32_sub(a, b) merlin_v4u32_sub(a, b)
#define v4u32_mul(a, b) merlin_v4u32_mul(a, b)
#define v4u32_div(a, b) merlin_v4u32_div(a, b)
#define v4u32_mod(a, b) merlin_v4u32_mod(a, b)

#define v4u32_cmpeq(a, b) merlin_v4u32_cmpeq(a, b)
#define v4u32_cmplt(a, b) merlin_v4u32_cmplt(a, b)
#define v4u32_cmpgt(a, b) merlin_v4u32_cmpgt(a, b)
#define v4u32_cmpleq(a, b) merlin_v4u32_cmpleq(a, b)
#define v4u32_cmpgeq(a, b) merlin_v4u32_cmpgeq(a, b)

#define v4u32_and(a, b) merlin_v4u32_and(a, b)
#define v4u32_or(a, b) merlin_v4u32_or(a, b)
#define v4u32_xor(a, b) merlin_v4u32_xor(a, b)
#define v4u32_not(a) merlin_v4u32_not(a)

#define v4u32_shift_left(a, n) merlin_v4u32_shift_left(a, n)
#define v4u32_shift_right(a, n) merlin_v4u32_shift_right(a, n)

#define v4u32_mask(a) merlin_v4u32_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v4i32=======================================

typedef int32_t merlin_v4i32_t __attribute__((vector_size(16), aligned(16)));
typedef int32_t merlin_v4i32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_set1(const int32_t v) {
  return (merlin_v4i32_t){v, v, v, v};
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_set(const int32_t v00,
                                                       const int32_t v01,
                                                       const int32_t v02,
                                                       const int32_t v03) {
  return (merlin_v4i32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v4i32_t
    merlin_v4i32_load_aligned(const merlin_v4i32_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v4i32_t
    merlin_v4i32_load_unaligned(const merlin_v4i32_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v4i32_store_aligned(
    merlin_v4i32_t addr[static 1], const merlin_v4i32_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v4i32_store_unaligned(
    merlin_v4i32_unaligned_t addr[static 1], const merlin_v4i32_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_add(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_sub(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_mul(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_div(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_mod(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v4i32_t
merlin_v4i32_cmpeq(const merlin_v4i32_t a, const merlin_v4i32_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t
merlin_v4i32_cmplt(const merlin_v4i32_t a, const merlin_v4i32_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t
merlin_v4i32_cmpgt(const merlin_v4i32_t a, const merlin_v4i32_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t
merlin_v4i32_cmpleq(const merlin_v4i32_t a, const merlin_v4i32_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t
merlin_v4i32_cmpgeq(const merlin_v4i32_t a, const merlin_v4i32_t b) {
  return a >= b;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v4i32_mask(const merlin_v4i32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  merlin_mask_t result = 0;
  const merlin_v4i32_t tmp = merlin_v4i32_shift_right(a, 31);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v4i32_t v4i32_t;
typedef merlin_v4i32_unaligned_t v4i32_unaligned_t;

#define v4i32_set1(v) merlin_v4i32_set1(v)
#define v4i32_set(v00, v01, v02, v02, v03)                                     \
  merlin_v4i32_set(v00, v01, v02, v02, v03)

#define v4i32_load_aligned(addr) merlin_v4i32_load_aligned(addr)
#define v4i32_load_unaligned(addr) merlin_v4i32_load_unaligned(addr)
#define v4i32_store_aligned(addr, data) merlin_v4i32_store_aligned(addr, data)
#define v4i32_store_unaligned(addr, data)                                      \
  merlin_v4i32_store_unaligned(addr, data)

#define v4i32_add(a, b) merlin_v4i32_add(a, b)
#define v4i32_sub(a, b) merlin_v4i32_sub(a, b)
#define v4i32_mul(a, b) merlin_v4i32_mul(a, b)
#define v4i32_div(a, b) merlin_v4i32_div(a, b)
#define v4i32_mod(a, b) merlin_v4i32_mod(a, b)

#define v4i32_cmpeq(a, b) merlin_v4i32_cmpeq(a, b)
#define v4i32_cmplt(a, b) merlin_v4i32_cmplt(a, b)
#define v4i32_cmpgt(a, b) merlin_v4i32_cmpgt(a, b)
#define v4i32_cmpleq(a, b) merlin_v4i32_cmpleq(a, b)
#define v4i32_cmpgeq(a, b) merlin_v4i32_cmpgeq(a, b)

#define v4i32_mask(a) merlin_v4i32_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v2u64=======================================

typedef uint64_t merlin_v2u64_t __attribute__((vector_size(16), aligned(16)));
typedef uint64_t merlin_v2u64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_set1(const uint64_t v) {
  return (merlin_v2u64_t){v, v};
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_set(const uint64_t v00,
                                                       const uint64_t v01) {
  return (merlin_v2u64_t){v00, v01};
}

//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v2u64_t
    merlin_v2u64_load_aligned(const merlin_v2u64_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v2u64_t
    merlin_v2u64_load_unaligned(const merlin_v2u64_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v2u64_store_aligned(
    merlin_v2u64_t addr[static 1], const merlin_v2u64_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v2u64_store_unaligned(
    merlin_v2u64_unaligned_t addr[static 1], const merlin_v2u64_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_add(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_sub(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_mul(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_div(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_mod(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_cmpeq(const merlin_v2u64_t a, const merlin_v2u64_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_cmplt(const merlin_v2u64_t a, const merlin_v2u64_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_cmpgt(const merlin_v2u64_t a, const merlin_v2u64_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_cmpleq(const merlin_v2u64_t a, const merlin_v2u64_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_cmpgeq(const merlin_v2u64_t a, const merlin_v2u64_t b) {
  return a >= b;
}

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_and(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_or(const merlin_v2u64_t a,
                                                      const merlin_v2u64_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_xor(const merlin_v2u64_t a,
                                                       const merlin_v2u64_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v2u64_t merlin_v2u64_not(const merlin_v2u64_t a) {
  return ~a;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_shift_left(const merlin_v2u64_t a, const uint64_t n) {
  return a << n;
}

MERLIN_FN_ATTRS static merlin_v2u64_t
merlin_v2u64_shift_right(const merlin_v2u64_t a, const uint64_t n) {
  return a >> n;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v2u64_mask(const merlin_v2u64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  merlin_mask_t result = 0;
  const merlin_v2u64_t tmp = merlin_v2u64_shift_right(a, 63);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v2u64_t v2u64_t;
typedef merlin_v2u64_unaligned_t v2u64_unaligned_t;

#define v2u64_set1(v) merlin_v2u64_set1(v)
#define v2u64_set(v00, v01) merlin_v2u64_set(v00, v01)

#define v2u64_load_aligned(addr) merlin_v2u64_load_aligned(addr)
#define v2u64_load_unaligned(addr) merlin_v2u64_load_unaligned(addr)
#define v2u64_store_aligned(addr, data) merlin_v2u64_store_aligned(addr, data)
#define v2u64_store_unaligned(addr, data)                                      \
  merlin_v2u64_store_unaligned(addr, data)

#define v2u64_add(a, b) merlin_v2u64_add(a, b)
#define v2u64_sub(a, b) merlin_v2u64_sub(a, b)
#define v2u64_mul(a, b) merlin_v2u64_mul(a, b)
#define v2u64_div(a, b) merlin_v2u64_div(a, b)
#define v2u64_mod(a, b) merlin_v2u64_mod(a, b)

#define v2u64_cmpeq(a, b) merlin_v2u64_cmpeq(a, b)
#define v2u64_cmplt(a, b) merlin_v2u64_cmplt(a, b)
#define v2u64_cmpgt(a, b) merlin_v2u64_cmpgt(a, b)
#define v2u64_cmpleq(a, b) merlin_v2u64_cmpleq(a, b)
#define v2u64_cmpgeq(a, b) merlin_v2u64_cmpgeq(a, b)

#define v2u64_and(a, b) merlin_v2u64_and(a, b)
#define v2u64_or(a, b) merlin_v2u64_or(a, b)
#define v2u64_xor(a, b) merlin_v2u64_xor(a, b)
#define v2u64_not(a) merlin_v2u64_not(a)

#define v2u64_shift_left(a, n) merlin_v2u64_shift_left(a, n)
#define v2u64_shift_right(a, n) merlin_v2u64_shift_right(a, n)

#define v2u64_mask(a) merlin_v2u64_mask(a)
#endif // MERLIN_DROP_PREFIX

//==================================v2i64=======================================

typedef int64_t merlin_v2i64_t __attribute__((vector_size(16), aligned(16)));
typedef int64_t merlin_v2i64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_set1(const int64_t v) {
  return (merlin_v2i64_t){v, v};
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_set(const int64_t v00,
                                                       const int64_t v01) {
  return (merlin_v2i64_t){v00, v01};
}
//----------------------------------load/store----------------------------------

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v2i64_t
    merlin_v2i64_load_aligned(const merlin_v2i64_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_NODISCARD MERLIN_NONNULL(1) static merlin_v2i64_t
    merlin_v2i64_load_unaligned(const merlin_v2i64_unaligned_t addr[static 1]) {
  MERLIN_ASSUME(addr != NULL);
  return *addr;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v2i64_store_aligned(
    merlin_v2i64_t addr[static 1], const merlin_v2i64_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

MERLIN_VOIDFN_ATTRS MERLIN_NONNULL(1) static void merlin_v2i64_store_unaligned(
    merlin_v2i64_unaligned_t addr[static 1], const merlin_v2i64_t data) {
  MERLIN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_add(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a + b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_sub(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a - b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_mul(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a * b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_div(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a / b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_mod(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a % b;
}

//----------------------------------cmp-----------------------------------------

MERLIN_FN_ATTRS static merlin_v2i64_t
merlin_v2i64_cmpeq(const merlin_v2i64_t a, const merlin_v2i64_t b) {
  return a == b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t
merlin_v2i64_cmplt(const merlin_v2i64_t a, const merlin_v2i64_t b) {
  return a < b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t
merlin_v2i64_cmpgt(const merlin_v2i64_t a, const merlin_v2i64_t b) {
  return a > b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t
merlin_v2i64_cmpleq(const merlin_v2i64_t a, const merlin_v2i64_t b) {
  return a <= b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t
merlin_v2i64_cmpgeq(const merlin_v2i64_t a, const merlin_v2i64_t b) {
  return a >= b;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask_t merlin_v2i64_mask(const merlin_v2i64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  merlin_mask_t result = 0;
  const merlin_v2u64_t tmp = merlin_v2u64_shift_right(a, 63);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
#endif
}

#ifdef MERLIN_DROP_PREFIX
typedef merlin_v2i64_t v2i64_t;
typedef merlin_v2i64_unaligned_t v2i64_unaligned_t;

#define v2i64_set1(v) merlin_v2i64_set1(v)
#define v2i64_set(v00, v01) merlin_v2i64_set(v00, v01)

#define v2i64_load_aligned(addr) merlin_v2i64_load_aligned(addr)
#define v2i64_load_unaligned(addr) merlin_v2i64_load_unaligned(addr)
#define v2i64_store_aligned(addr, data) merlin_v2i64_store_aligned(addr, data)
#define v2i64_store_unaligned(addr, data)                                      \
  merlin_v2i64_store_unaligned(addr, data)

#define v2i64_add(a, b) merlin_v2i64_add(a, b)
#define v2i64_sub(a, b) merlin_v2i64_sub(a, b)
#define v2i64_mul(a, b) merlin_v2i64_mul(a, b)
#define v2i64_div(a, b) merlin_v2i64_div(a, b)
#define v2i64_mod(a, b) merlin_v2i64_mod(a, b)

#define v2i64_cmpeq(a, b) merlin_v2i64_cmpeq(a, b)
#define v2i64_cmplt(a, b) merlin_v2i64_cmplt(a, b)
#define v2i64_cmpgt(a, b) merlin_v2i64_cmpgt(a, b)
#define v2i64_cmpleq(a, b) merlin_v2i64_cmpleq(a, b)
#define v2i64_cmpgeq(a, b) merlin_v2i64_cmpgeq(a, b)

#define v2i64_mask(a) merlin_v2i64_mask(a)
#endif // MERLIN_DROP_PREFIX

#undef MERLIN_NODISCARD
#undef MERLIN_ASSUME
#undef MERLIN_NONNULL
#undef MERLIN_VOIDFN_ATTRS
#undef MERLIN_FN_ATTRS

#endif // MERLIN_SIMD_H
