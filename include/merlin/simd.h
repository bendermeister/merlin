#ifndef MRLN_SIMD_H
#define MRLN_SIMD_H

// TODO(ben): blend functions
// TODO(ben): mul low / high ?
// TODO(ben): align function moves the elements inside the vector to different
// positions
// TODO(ben): floating point vector
// TODO(ben): abs functions for the signed vectors
// TODO(ben): max min
// TODO(ben): shuffle
// TODO(ben): documentation

#include <sys/cdefs.h>
#ifndef __clang__
#error "currently only clang is supported"
#endif

#include <stddef.h>
#include <stdint.h>

#define MRLN_VOIDFN_ATTRS                                                      \
  __attribute__((__always_inline__, nodebug, __unused__))
#define MRLN_NODISCARD __attribute__((__warn_unused_result__))
#define MRLN_FN_ATTRS                                                          \
  MRLN_VOIDFN_ATTRS MRLN_NODISCARD __attribute__((__const__))
#define MRLN_NONNULL(PARAM) __attribute__((__nonnull__(PARAM)))

#define MRLN_ASSUME(X)                                                         \
  if (!(X))                                                                    \
  __builtin_unreachable()

//==================================mask=types==================================

MRLN_FN_ATTRS static uint32_t mrln_mask_first_set(const uint32_t mask) {
  return __builtin_ctz(mask);
}

// generator macros
#define MRLN_OP_ATTRS                                                          \
  __attribute__((__always_inline__, __unused__, __warn_unused_result__,        \
                 __const__))

#define MRLN_BOP(NAME, TYPE, BOP)                                              \
  __attribute__((__always_inline__, __unused__, __warn_unused_result__,        \
                 __const__)) static TYPE                                       \
  NAME(const TYPE a, const TYPE b) {                                           \
    return a BOP b;                                                            \
  }

#define MRLN_UOP(NAME, TYPE, UOP)                                              \
  __attribute__((__always_inline__, __unused__, __warn_unused_result__,        \
                 __const__)) static TYPE                                       \
  NAME(const TYPE a) {                                                         \
    return UOP a;                                                              \
  }

//==================================v16u8=======================================

typedef uint8_t mrln_v16u8_t __attribute__((vector_size(16), aligned(16)));
typedef uint8_t mrln_v16u8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v16u8_t mrln_v16u8_set1(const uint8_t v) {
  return (mrln_v16u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v16u8_t
mrln_v16u8_set(const uint8_t v00, const uint8_t v01, const uint8_t v02,
               const uint8_t v03, const uint8_t v04, const uint8_t v05,
               const uint8_t v06, const uint8_t v07, const uint8_t v08,
               const uint8_t v09, const uint8_t v10, const uint8_t v11,
               const uint8_t v12, const uint8_t v13, const uint8_t v14,
               const uint8_t v15) {
  return (mrln_v16u8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                        v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16u8_t
    mrln_v16u8_load_aligned(const mrln_v16u8_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16u8_t
    mrln_v16u8_load_unaligned(const mrln_v16u8_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16u8_store_aligned(
    mrln_v16u8_t addr[static 1], const mrln_v16u8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16u8_store_unaligned(
    mrln_v16u8_unaligned_t addr[static 1], const mrln_v16u8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v16u8_add, mrln_v16u8_t, +)
MRLN_BOP(mrln_v16u8_sub, mrln_v16u8_t, -)
MRLN_BOP(mrln_v16u8_mul, mrln_v16u8_t, *)
MRLN_BOP(mrln_v16u8_div, mrln_v16u8_t, /)
MRLN_BOP(mrln_v16u8_mod, mrln_v16u8_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v16u8_cmpeq, mrln_v16u8_t, ==)
MRLN_BOP(mrln_v16u8_cmplt, mrln_v16u8_t, <)
MRLN_BOP(mrln_v16u8_cmpgt, mrln_v16u8_t, >)
MRLN_BOP(mrln_v16u8_cmpleq, mrln_v16u8_t, <=)
MRLN_BOP(mrln_v16u8_cmpgeq, mrln_v16u8_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v16u8_and, mrln_v16u8_t, &)
MRLN_BOP(mrln_v16u8_or, mrln_v16u8_t, |)
MRLN_BOP(mrln_v16u8_xor, mrln_v16u8_t, ^)
MRLN_UOP(mrln_v16u8_not, mrln_v16u8_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint8_t[16]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint8_t[16] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v16u8_t mrln_v16u8_shift_left(const mrln_v16u8_t a,
                                                        const uint32_t n) {
#ifdef __SSE2__
  mrln_v16u8_t tmp = __builtin_ia32_psllwi128(a, (int)n);
  tmp = mrln_v16u8_and(tmp, mrln_v16u8_set1((uint8_t)-1 << n));
  return tmp;
#else
  return (mrln_v16u8_t){a[0] << n,  a[1] << n,  a[2] << n,  a[3] << n,
                        a[4] << n,  a[5] << n,  a[6] << n,  a[7] << n,
                        a[8] << n,  a[9] << n,  a[10] << n, a[11] << n,
                        a[12] << n, a[13] << n, a[14] << n, a[15] << n};
#endif //__SSE2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint8_t[16]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint8_t[16] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v16u8_t mrln_v16u8_shift_right(const mrln_v16u8_t a,
                                                         const uint32_t n) {
#ifdef __SSE2__
  mrln_v16u8_t tmp = __builtin_ia32_psrlwi128(a, (int)n);
  tmp = mrln_v16u8_and(tmp, mrln_v16u8_set1((uint8_t)-1 >> n));
  return tmp;
#else
  return (mrln_v16u8_t){a[0] >> n,  a[1] >> n,  a[2] >> n,  a[3] >> n,
                        a[4] >> n,  a[5] >> n,  a[6] >> n,  a[7] >> n,
                        a[8] >> n,  a[9] >> n,  a[10] >> n, a[11] >> n,
                        a[12] >> n, a[13] >> n, a[14] >> n, a[15] >> n};
#endif //__SSE2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v16u8_mask(const mrln_v16u8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  uint32_t result = 0;
  const mrln_v16u8_t tmp = mrln_v16u8_shift_right(a, 7);
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

//==================================v16i8=======================================

typedef int8_t mrln_v16i8_t __attribute__((vector_size(16), aligned(16)));
typedef int8_t mrln_v16i8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v16i8_t mrln_v16i8_set1(const int8_t v) {
  return (mrln_v16i8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v16i8_t mrln_v16i8_set(
    const int8_t v00, const int8_t v01, const int8_t v02, const int8_t v03,
    const int8_t v04, const int8_t v05, const int8_t v06, const int8_t v07,
    const int8_t v08, const int8_t v09, const int8_t v10, const int8_t v11,
    const int8_t v12, const int8_t v13, const int8_t v14, const int8_t v15) {
  return (mrln_v16i8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                        v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16i8_t
    mrln_v16i8_load_aligned(const mrln_v16i8_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16i8_t
    mrln_v16i8_load_unaligned(const mrln_v16i8_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16i8_store_aligned(
    mrln_v16i8_t addr[static 1], const mrln_v16i8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16i8_store_unaligned(
    mrln_v16i8_unaligned_t addr[static 1], const mrln_v16i8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v16i8_add, mrln_v16i8_t, +)
MRLN_BOP(mrln_v16i8_sub, mrln_v16i8_t, -)
MRLN_BOP(mrln_v16i8_mul, mrln_v16i8_t, *)
MRLN_BOP(mrln_v16i8_div, mrln_v16i8_t, /)
MRLN_BOP(mrln_v16i8_mod, mrln_v16i8_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v16i8_cmpeq, mrln_v16i8_t, ==)
MRLN_BOP(mrln_v16i8_cmplt, mrln_v16i8_t, <)
MRLN_BOP(mrln_v16i8_cmpgt, mrln_v16i8_t, >)
MRLN_BOP(mrln_v16i8_cmpleq, mrln_v16i8_t, <=)
MRLN_BOP(mrln_v16i8_cmpgeq, mrln_v16i8_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v16i8_mask(const mrln_v16i8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  uint32_t result = 0;
  const mrln_v16u8_t tmp =
      mrln_v16u8_shift_right((const mrln_v16u8_t)a, mrln_v16u8_set1(7));
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

//==================================v8u16=======================================

typedef uint16_t mrln_v8u16_t __attribute__((vector_size(16), aligned(16)));
typedef uint16_t mrln_v8u16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v8u16_t mrln_v8u16_set1(const uint16_t v) {
  return (mrln_v8u16_t){v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v8u16_t
mrln_v8u16_set(const uint16_t v00, const uint16_t v01, const uint16_t v02,
               const uint16_t v03, const uint16_t v04, const uint16_t v05,
               const uint16_t v06, const uint16_t v07) {
  return (mrln_v8u16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8u16_t
    mrln_v8u16_load_aligned(const mrln_v8u16_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8u16_t
    mrln_v8u16_load_unaligned(const mrln_v8u16_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8u16_store_aligned(
    mrln_v8u16_t addr[static 1], const mrln_v8u16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8u16_store_unaligned(
    mrln_v8u16_unaligned_t addr[static 1], const mrln_v8u16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v8u16_add, mrln_v8u16_t, +)
MRLN_BOP(mrln_v8u16_sub, mrln_v8u16_t, -)
MRLN_BOP(mrln_v8u16_mul, mrln_v8u16_t, *)
MRLN_BOP(mrln_v8u16_div, mrln_v8u16_t, /)
MRLN_BOP(mrln_v8u16_mod, mrln_v8u16_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v8u16_cmpeq, mrln_v8u16_t, ==)
MRLN_BOP(mrln_v8u16_cmplt, mrln_v8u16_t, <)
MRLN_BOP(mrln_v8u16_cmpgt, mrln_v8u16_t, >)
MRLN_BOP(mrln_v8u16_cmpleq, mrln_v8u16_t, <=)
MRLN_BOP(mrln_v8u16_cmpgeq, mrln_v8u16_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v8u16_and, mrln_v8u16_t, &)
MRLN_BOP(mrln_v8u16_or, mrln_v8u16_t, |)
MRLN_BOP(mrln_v8u16_xor, mrln_v8u16_t, ^)
MRLN_UOP(mrln_v8u16_not, mrln_v8u16_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint16_t[8]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint16_t[8] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v8u16_t mrln_v8u16_shift_left(const mrln_v8u16_t a,
                                                        const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_psllwi128(a, (int)n);
#else
  return (mrln_v8u16_t){a[0] << n, a[1] << n, a[2] << n, a[3] << n,
                        a[4] << n, a[5] << n, a[6] << n, a[7] << n};
#endif //__SSE2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint16_t[8]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint16_t[8] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v8u16_t mrln_v8u16_shift_right(const mrln_v8u16_t a,
                                                         const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_psrlwi128(a, (int)n);
#else
  return (mrln_v8u16_t){a[0] >> n, a[1] >> n, a[2] >> n, a[3] >> n,
                        a[4] >> n, a[5] >> n, a[6] >> n, a[7] >> n};
#endif //__SSE2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v8u16_mask(const mrln_v8u16_t a) {
  uint32_t result = 0;
  const mrln_v8u16_t tmp = mrln_v8u16_shift_right(a, 15);
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

//==================================v8i16=======================================

typedef int16_t mrln_v8i16_t __attribute__((vector_size(16), aligned(16)));
typedef int16_t mrln_v8i16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v8i16_t mrln_v8i16_set1(const int16_t v) {
  return (mrln_v8i16_t){v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v8i16_t
mrln_v8i16_set(const int16_t v00, const int16_t v01, const int16_t v02,
               const int16_t v03, const int16_t v04, const int16_t v05,
               const int16_t v06, const int16_t v07) {
  return (mrln_v8i16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8i16_t
    mrln_v8i16_load_aligned(const mrln_v8i16_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8i16_t
    mrln_v8i16_load_unaligned(const mrln_v8i16_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8i16_store_aligned(
    mrln_v8i16_t addr[static 1], const mrln_v8i16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8i16_store_unaligned(
    mrln_v8i16_unaligned_t addr[static 1], const mrln_v8i16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v8i16_add, mrln_v8i16_t, +)
MRLN_BOP(mrln_v8i16_sub, mrln_v8i16_t, -)
MRLN_BOP(mrln_v8i16_mul, mrln_v8i16_t, *)
MRLN_BOP(mrln_v8i16_div, mrln_v8i16_t, /)
MRLN_BOP(mrln_v8i16_mod, mrln_v8i16_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v8i16_cmpeq, mrln_v8i16_t, ==)
MRLN_BOP(mrln_v8i16_cmplt, mrln_v8i16_t, <)
MRLN_BOP(mrln_v8i16_cmpgt, mrln_v8i16_t, >)
MRLN_BOP(mrln_v8i16_cmpleq, mrln_v8i16_t, <=)
MRLN_BOP(mrln_v8i16_cmpgeq, mrln_v8i16_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v8i16_mask(const mrln_v8i16_t a) {
  uint32_t result = 0;
  const mrln_v8u16_t tmp = mrln_v8u16_shift_right((const mrln_v8u16_t)a, 15);
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

//==================================v4u32=======================================

typedef uint32_t mrln_v4u32_t __attribute__((vector_size(16), aligned(16)));
typedef uint32_t mrln_v4u32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v4u32_t mrln_v4u32_set1(const uint32_t v) {
  return (mrln_v4u32_t){v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v4u32_t mrln_v4u32_set(const uint32_t v00,
                                                 const uint32_t v01,
                                                 const uint32_t v02,
                                                 const uint32_t v03) {
  return (mrln_v4u32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4u32_t
    mrln_v4u32_load_aligned(const mrln_v4u32_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4u32_t
    mrln_v4u32_load_unaligned(const mrln_v4u32_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4u32_store_aligned(
    mrln_v4u32_t addr[static 1], const mrln_v4u32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4u32_store_unaligned(
    mrln_v4u32_unaligned_t addr[static 1], const mrln_v4u32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v4u32_add, mrln_v4u32_t, +)
MRLN_BOP(mrln_v4u32_sub, mrln_v4u32_t, -)
MRLN_BOP(mrln_v4u32_mul, mrln_v4u32_t, *)
MRLN_BOP(mrln_v4u32_div, mrln_v4u32_t, /)
MRLN_BOP(mrln_v4u32_mod, mrln_v4u32_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v4u32_cmpeq, mrln_v4u32_t, ==)
MRLN_BOP(mrln_v4u32_cmplt, mrln_v4u32_t, <)
MRLN_BOP(mrln_v4u32_cmpgt, mrln_v4u32_t, >)
MRLN_BOP(mrln_v4u32_cmpleq, mrln_v4u32_t, <=)
MRLN_BOP(mrln_v4u32_cmpgeq, mrln_v4u32_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v4u32_and, mrln_v4u32_t, &)
MRLN_BOP(mrln_v4u32_or, mrln_v4u32_t, |)
MRLN_BOP(mrln_v4u32_xor, mrln_v4u32_t, ^)
MRLN_UOP(mrln_v4u32_not, mrln_v4u32_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type `uint32_t[4]`
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type `uint32_t[4]` with the shifted values
 */
MRLN_OP_ATTRS static mrln_v4u32_t mrln_v4u32_shift_left(const mrln_v4u32_t a,
                                                        const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_pslldi128(a, (int)n);
#else
  return (mrln_v4u32_t){a[0] << n, a[1] << n, a[2] << n, a[3] << n};
#endif //__SSE2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint32_t[4]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint32_t[4] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v4u32_t mrln_v4u32_shift_right(const mrln_v4u32_t a,
                                                         const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_psrldi128(a, (int)n);
#else
  return (mrln_v4u32_t){a[0] >> n, a[1] >> n, a[2] >> n, a[3] >> n};
#endif //__SSE2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v4u32_mask(const mrln_v4u32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  uint32_t result = 0;
  const mrln_v4u32_t tmp = mrln_v4u32_shift_right(a, 31);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif
}

//==================================v4i32=======================================

typedef int32_t mrln_v4i32_t __attribute__((vector_size(16), aligned(16)));
typedef int32_t mrln_v4i32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v4i32_t mrln_v4i32_set1(const int32_t v) {
  return (mrln_v4i32_t){v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v4i32_t mrln_v4i32_set(const int32_t v00,
                                                 const int32_t v01,
                                                 const int32_t v02,
                                                 const int32_t v03) {
  return (mrln_v4i32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4i32_t
    mrln_v4i32_load_aligned(const mrln_v4i32_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4i32_t
    mrln_v4i32_load_unaligned(const mrln_v4i32_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4i32_store_aligned(
    mrln_v4i32_t addr[static 1], const mrln_v4i32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4i32_store_unaligned(
    mrln_v4i32_unaligned_t addr[static 1], const mrln_v4i32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v4i32_add, mrln_v4i32_t, +)
MRLN_BOP(mrln_v4i32_sub, mrln_v4i32_t, -)
MRLN_BOP(mrln_v4i32_mul, mrln_v4i32_t, *)
MRLN_BOP(mrln_v4i32_div, mrln_v4i32_t, /)
MRLN_BOP(mrln_v4i32_mod, mrln_v4i32_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v4i32_cmpeq, mrln_v4i32_t, ==)
MRLN_BOP(mrln_v4i32_cmplt, mrln_v4i32_t, <)
MRLN_BOP(mrln_v4i32_cmpgt, mrln_v4i32_t, >)
MRLN_BOP(mrln_v4i32_cmpleq, mrln_v4i32_t, <=)
MRLN_BOP(mrln_v4i32_cmpgeq, mrln_v4i32_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v4i32_mask(const mrln_v4i32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  uint32_t result = 0;
  const mrln_v4i32_t tmp = mrln_v4u32_shift_right(a, 31);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif
}

//==================================v2u64=======================================

typedef uint64_t mrln_v2u64_t __attribute__((vector_size(16), aligned(16)));
typedef uint64_t mrln_v2u64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v2u64_t mrln_v2u64_set1(const uint64_t v) {
  return (mrln_v2u64_t){v, v};
}

MRLN_FN_ATTRS static mrln_v2u64_t mrln_v2u64_set(const uint64_t v00,
                                                 const uint64_t v01) {
  return (mrln_v2u64_t){v00, v01};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v2u64_t
    mrln_v2u64_load_aligned(const mrln_v2u64_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v2u64_t
    mrln_v2u64_load_unaligned(const mrln_v2u64_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v2u64_store_aligned(
    mrln_v2u64_t addr[static 1], const mrln_v2u64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v2u64_store_unaligned(
    mrln_v2u64_unaligned_t addr[static 1], const mrln_v2u64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v2u64_add, mrln_v2u64_t, +)
MRLN_BOP(mrln_v2u64_sub, mrln_v2u64_t, -)
MRLN_BOP(mrln_v2u64_mul, mrln_v2u64_t, *)
MRLN_BOP(mrln_v2u64_div, mrln_v2u64_t, /)
MRLN_BOP(mrln_v2u64_mod, mrln_v2u64_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v2u64_cmpeq, mrln_v2u64_t, ==)
MRLN_BOP(mrln_v2u64_cmplt, mrln_v2u64_t, <)
MRLN_BOP(mrln_v2u64_cmpgt, mrln_v2u64_t, >)
MRLN_BOP(mrln_v2u64_cmpleq, mrln_v2u64_t, <=)
MRLN_BOP(mrln_v2u64_cmpgeq, mrln_v2u64_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v2u64_and, mrln_v2u64_t, &)
MRLN_BOP(mrln_v2u64_or, mrln_v2u64_t, |)
MRLN_BOP(mrln_v2u64_xor, mrln_v2u64_t, ^)
MRLN_UOP(mrln_v2u64_not, mrln_v2u64_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type `uint64_t[2]`
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type `uint64_t[2]` with the shifted values
 */
MRLN_OP_ATTRS static mrln_v2u64_t mrln_v2u64_shift_left(const mrln_v2u64_t a,
                                                        const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_psllqi128(a, (int)n);
#else
  return (mrln_v2u64_t){a[0] << n, a[1] << n};
#endif //__SSE2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint64_t[2]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint64_t[2] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v2u64_t mrln_v2u64_shift_right(const mrln_v2u64_t a,
                                                         const uint32_t n) {
#ifdef __SSE2__
  return __builtin_ia32_psrlqi128(a, (int)n);
#else
  return (mrln_v2u64_t){a[0] >> n, a[1] >> n};
#endif //__SSE2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v2u64_mask(const mrln_v2u64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  uint32_t result = 0;
  const mrln_v2u64_t tmp = mrln_v2u64_shift_right(a, 63);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
#endif
}

//==================================v2i64=======================================

typedef int64_t mrln_v2i64_t __attribute__((vector_size(16), aligned(16)));
typedef int64_t mrln_v2i64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v2i64_t mrln_v2i64_set1(const int64_t v) {
  return (mrln_v2i64_t){v, v};
}

MRLN_FN_ATTRS static mrln_v2i64_t mrln_v2i64_set(const int64_t v00,
                                                 const int64_t v01) {
  return (mrln_v2i64_t){v00, v01};
}
//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v2i64_t
    mrln_v2i64_load_aligned(const mrln_v2i64_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v2i64_t
    mrln_v2i64_load_unaligned(const mrln_v2i64_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v2i64_store_aligned(
    mrln_v2i64_t addr[static 1], const mrln_v2i64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v2i64_store_unaligned(
    mrln_v2i64_unaligned_t addr[static 1], const mrln_v2i64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v2i64_add, mrln_v2i64_t, +)
MRLN_BOP(mrln_v2i64_sub, mrln_v2i64_t, -)
MRLN_BOP(mrln_v2i64_mul, mrln_v2i64_t, *)
MRLN_BOP(mrln_v2i64_div, mrln_v2i64_t, /)
MRLN_BOP(mrln_v2i64_mod, mrln_v2i64_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v2i64_cmpeq, mrln_v2i64_t, ==)
MRLN_BOP(mrln_v2i64_cmplt, mrln_v2i64_t, <)
MRLN_BOP(mrln_v2i64_cmpgt, mrln_v2i64_t, >)
MRLN_BOP(mrln_v2i64_cmpleq, mrln_v2i64_t, <=)
MRLN_BOP(mrln_v2i64_cmpgeq, mrln_v2i64_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v2i64_mask(const mrln_v2i64_t a) {
#ifdef __SSE2__
  typedef double cast_type __attribute__((vector_size(16)));
  return __builtin_ia32_movmskpd((cast_type)a);
#else
  uint32_t result = 0;
  const mrln_v2u64_t tmp = mrln_v2u64_shift_right(a, 63);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
#endif
}

//==================================v32u8=======================================

typedef uint8_t mrln_v32u8_t __attribute__((vector_size(32), aligned(32)));
typedef uint8_t mrln_v32u8_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v32u8_t mrln_v32u8_set1(const uint8_t v) {
  return (mrln_v32u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
                        v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v32u8_t mrln_v32u8_set(
    const uint8_t v00, const uint8_t v01, const uint8_t v02, const uint8_t v03,
    const uint8_t v04, const uint8_t v05, const uint8_t v06, const uint8_t v07,
    const uint8_t v08, const uint8_t v09, const uint8_t v10, const uint8_t v11,
    const uint8_t v12, const uint8_t v13, const uint8_t v14, const uint8_t v15,
    const uint8_t v16, const uint8_t v17, const uint8_t v18, const uint8_t v19,
    const uint8_t v20, const uint8_t v21, const uint8_t v22, const uint8_t v23,
    const uint8_t v24, const uint8_t v25, const uint8_t v26, const uint8_t v27,
    const uint8_t v28, const uint8_t v29, const uint8_t v30,
    const uint8_t v31) {
  return (mrln_v32u8_t){v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10,
                        v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21,
                        v22, v23, v24, v25, v26, v27, v28, v29, v30, v31};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v32u8_t
    mrln_v32u8_load_aligned(const mrln_v32u8_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v32u8_t
    mrln_v32u8_load_unaligned(const mrln_v32u8_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v32u8_store_aligned(
    mrln_v32u8_t addr[static 1], const mrln_v32u8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v32u8_store_unaligned(
    mrln_v32u8_unaligned_t addr[static 1], const mrln_v32u8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v32u8_add, mrln_v32u8_t, +)
MRLN_BOP(mrln_v32u8_sub, mrln_v32u8_t, -)
MRLN_BOP(mrln_v32u8_mul, mrln_v32u8_t, *)
MRLN_BOP(mrln_v32u8_div, mrln_v32u8_t, /)
MRLN_BOP(mrln_v32u8_mod, mrln_v32u8_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v32u8_cmpeq, mrln_v32u8_t, ==)
MRLN_BOP(mrln_v32u8_cmplt, mrln_v32u8_t, <)
MRLN_BOP(mrln_v32u8_cmpgt, mrln_v32u8_t, >)
MRLN_BOP(mrln_v32u8_cmpleq, mrln_v32u8_t, <=)
MRLN_BOP(mrln_v32u8_cmpgeq, mrln_v32u8_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v32u8_and, mrln_v32u8_t, &)
MRLN_BOP(mrln_v32u8_or, mrln_v32u8_t, |)
MRLN_BOP(mrln_v32u8_xor, mrln_v32u8_t, ^)
MRLN_UOP(mrln_v32u8_not, mrln_v32u8_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint8_t[32]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint8_t[32] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v32u8_t mrln_v32u8_shift_left(const mrln_v32u8_t a,
                                                        const uint32_t n) {
#ifdef __AVX2__
  mrln_v32u8_t tmp = __builtin_ia32_psllwi256(a, (int)n);
  tmp = mrln_v32u8_and(tmp, mrln_v32u8_set1((uint8_t)-1 << n));
  return tmp;
#else
  mrln_v16u8_t lower = {a[0], a[1], a[2],  a[3],  a[4],  a[5],  a[6],  a[7],
                        a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
  mrln_v16u8_t upper = {a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
                        a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]};

  lower = mrln_v16u8_shift_left(lower, n);
  upper = mrln_v16u8_shift_left(upper, n);

  return (mrln_v32u8_t){
      lower[0],  lower[1],  lower[2],  lower[3],  lower[4],  lower[5],
      lower[6],  lower[7],  lower[8],  lower[9],  lower[10], lower[11],
      lower[12], lower[13], lower[14], lower[15],

      upper[0],  upper[1],  upper[2],  upper[3],  upper[4],  upper[5],
      upper[6],  upper[7],  upper[8],  upper[9],  upper[10], upper[11],
      upper[12], upper[13], upper[14], upper[15],
  };
#endif //__AVX2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint8_t[32]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint8_t[32] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v32u8_t mrln_v32u8_shift_right(const mrln_v32u8_t a,
                                                         const uint32_t n) {
#ifdef __AVX2__
  mrln_v32u8_t tmp = __builtin_ia32_psrlwi256(a, (int)n);
  tmp = mrln_v32u8_and(tmp, mrln_v32u8_set1((uint8_t)-1 >> n));
  return tmp;
#else
  mrln_v16u8_t lower = {a[0], a[1], a[2],  a[3],  a[4],  a[5],  a[6],  a[7],
                        a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
  mrln_v16u8_t upper = {a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
                        a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]};

  lower = mrln_v16u8_shift_right(lower, n);
  upper = mrln_v16u8_shift_right(upper, n);

  return (mrln_v32u8_t){
      lower[0],  lower[1],  lower[2],  lower[3],  lower[4],  lower[5],
      lower[6],  lower[7],  lower[8],  lower[9],  lower[10], lower[11],
      lower[12], lower[13], lower[14], lower[15],

      upper[0],  upper[1],  upper[2],  upper[3],  upper[4],  upper[5],
      upper[6],  upper[7],  upper[8],  upper[9],  upper[10], upper[11],
      upper[12], upper[13], upper[14], upper[15],
  };
#endif //__AVX2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v32u8_mask(const mrln_v32u8_t a) {
#ifdef __AVX2__
  return __builtin_ia32_pmovmskb256(a);
#else
  uint32_t result = 0;
  const mrln_v32u8_t tmp = mrln_v32u8_shift_right(a, 7);
  result |= (uint32_t)tmp[0] << 0;
  result |= (uint32_t)tmp[1] << 1;
  result |= (uint32_t)tmp[2] << 2;
  result |= (uint32_t)tmp[3] << 3;
  result |= (uint32_t)tmp[4] << 4;
  result |= (uint32_t)tmp[5] << 5;
  result |= (uint32_t)tmp[6] << 6;
  result |= (uint32_t)tmp[7] << 7;
  result |= (uint32_t)tmp[8] << 8;
  result |= (uint32_t)tmp[9] << 9;
  result |= (uint32_t)tmp[10] << 10;
  result |= (uint32_t)tmp[11] << 11;
  result |= (uint32_t)tmp[12] << 12;
  result |= (uint32_t)tmp[13] << 13;
  result |= (uint32_t)tmp[14] << 14;
  result |= (uint32_t)tmp[15] << 15;
  result |= (uint32_t)tmp[16] << 16;
  result |= (uint32_t)tmp[17] << 17;
  result |= (uint32_t)tmp[18] << 18;
  result |= (uint32_t)tmp[19] << 19;
  result |= (uint32_t)tmp[20] << 20;
  result |= (uint32_t)tmp[21] << 21;
  result |= (uint32_t)tmp[22] << 22;
  result |= (uint32_t)tmp[23] << 23;
  result |= (uint32_t)tmp[24] << 24;
  result |= (uint32_t)tmp[25] << 25;
  result |= (uint32_t)tmp[26] << 26;
  result |= (uint32_t)tmp[27] << 27;
  result |= (uint32_t)tmp[28] << 28;
  result |= (uint32_t)tmp[29] << 29;
  result |= (uint32_t)tmp[30] << 30;
  result |= (uint32_t)tmp[31] << 31;
  return result;
#endif
}

//==================================v32i8=======================================

typedef int8_t mrln_v32i8_t __attribute__((vector_size(32), aligned(32)));
typedef int8_t mrln_v32i8_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v32i8_t mrln_v32i8_set1(const int8_t v) {
  return (mrln_v32i8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
                        v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v32i8_t mrln_v32i8_set(
    const int8_t v00, const int8_t v01, const int8_t v02, const int8_t v03,
    const int8_t v04, const int8_t v05, const int8_t v06, const int8_t v07,
    const int8_t v08, const int8_t v09, const int8_t v10, const int8_t v11,
    const int8_t v12, const int8_t v13, const int8_t v14, const int8_t v15,
    const int8_t v16, const int8_t v17, const int8_t v18, const int8_t v19,
    const int8_t v20, const int8_t v21, const int8_t v22, const int8_t v23,
    const int8_t v24, const int8_t v25, const int8_t v26, const int8_t v27,
    const int8_t v28, const int8_t v29, const int8_t v30, const int8_t v31) {
  return (mrln_v32i8_t){v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10,
                        v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21,
                        v22, v23, v24, v25, v26, v27, v28, v29, v30, v31};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v32i8_t
    mrln_v32i8_load_aligned(const mrln_v32i8_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v32i8_t
    mrln_v32i8_load_unaligned(const mrln_v32i8_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v32i8_store_aligned(
    mrln_v32i8_t addr[static 1], const mrln_v32i8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v32i8_store_unaligned(
    mrln_v32i8_unaligned_t addr[static 1], const mrln_v32i8_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v32i8_add, mrln_v32i8_t, +)
MRLN_BOP(mrln_v32i8_sub, mrln_v32i8_t, -)
MRLN_BOP(mrln_v32i8_mul, mrln_v32i8_t, *)
MRLN_BOP(mrln_v32i8_div, mrln_v32i8_t, /)
MRLN_BOP(mrln_v32i8_mod, mrln_v32i8_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v32i8_cmpeq, mrln_v32i8_t, ==)
MRLN_BOP(mrln_v32i8_cmplt, mrln_v32i8_t, <)
MRLN_BOP(mrln_v32i8_cmpgt, mrln_v32i8_t, >)
MRLN_BOP(mrln_v32i8_cmpleq, mrln_v32i8_t, <=)
MRLN_BOP(mrln_v32i8_cmpgeq, mrln_v32i8_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v32i8_mask(const mrln_v32i8_t a) {
#ifdef __AVX2__
  return __builtin_ia32_pmovmskb256(a);
#else
  uint32_t result = 0;
  const mrln_v32i8_t tmp = mrln_v32u8_shift_right((mrln_v32u8_t)a, 7);
  result |= (uint32_t)tmp[1] << 1;
  result |= (uint32_t)tmp[2] << 2;
  result |= (uint32_t)tmp[3] << 3;
  result |= (uint32_t)tmp[4] << 4;
  result |= (uint32_t)tmp[5] << 5;
  result |= (uint32_t)tmp[6] << 6;
  result |= (uint32_t)tmp[7] << 7;
  result |= (uint32_t)tmp[8] << 8;
  result |= (uint32_t)tmp[9] << 9;
  result |= (uint32_t)tmp[10] << 10;
  result |= (uint32_t)tmp[11] << 11;
  result |= (uint32_t)tmp[12] << 12;
  result |= (uint32_t)tmp[13] << 13;
  result |= (uint32_t)tmp[14] << 14;
  result |= (uint32_t)tmp[15] << 15;
  result |= (uint32_t)tmp[16] << 16;
  result |= (uint32_t)tmp[17] << 17;
  result |= (uint32_t)tmp[18] << 18;
  result |= (uint32_t)tmp[19] << 19;
  result |= (uint32_t)tmp[20] << 20;
  result |= (uint32_t)tmp[21] << 21;
  result |= (uint32_t)tmp[22] << 22;
  result |= (uint32_t)tmp[23] << 23;
  result |= (uint32_t)tmp[24] << 24;
  result |= (uint32_t)tmp[25] << 25;
  result |= (uint32_t)tmp[26] << 26;
  result |= (uint32_t)tmp[27] << 27;
  result |= (uint32_t)tmp[28] << 28;
  result |= (uint32_t)tmp[29] << 29;
  result |= (uint32_t)tmp[30] << 30;
  result |= (uint32_t)tmp[31] << 31;
  return result;
#endif
}

//==================================v16u16=======================================

typedef uint16_t mrln_v16u16_t __attribute__((vector_size(32), aligned(32)));
typedef uint16_t mrln_v16u16_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v16u16_t mrln_v16u16_set1(const uint16_t v) {
  return (mrln_v16u16_t){
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
  };
}

MRLN_FN_ATTRS static mrln_v16u16_t
mrln_v16u16_set(const uint16_t v00, const uint16_t v01, const uint16_t v02,
                const uint16_t v03, const uint16_t v04, const uint16_t v05,
                const uint16_t v06, const uint16_t v07, const uint16_t v08,
                const uint16_t v09, const uint16_t v10, const uint16_t v11,
                const uint16_t v12, const uint16_t v13, const uint16_t v14,
                const uint16_t v15) {
  return (mrln_v16u16_t){v00, v01, v02, v03, v04, v05, v06, v07,
                         v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16u16_t
    mrln_v16u16_load_aligned(const mrln_v16u16_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16u16_t
    mrln_v16u16_load_unaligned(const mrln_v16u16_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16u16_store_aligned(
    mrln_v16u16_t addr[static 1], const mrln_v16u16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16u16_store_unaligned(
    mrln_v16u16_unaligned_t addr[static 1], const mrln_v16u16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v16u16_add, mrln_v16u16_t, +)
MRLN_BOP(mrln_v16u16_sub, mrln_v16u16_t, -)
MRLN_BOP(mrln_v16u16_mul, mrln_v16u16_t, *)
MRLN_BOP(mrln_v16u16_div, mrln_v16u16_t, /)
MRLN_BOP(mrln_v16u16_mod, mrln_v16u16_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v16u16_cmpeq, mrln_v16u16_t, ==)
MRLN_BOP(mrln_v16u16_cmplt, mrln_v16u16_t, <)
MRLN_BOP(mrln_v16u16_cmpgt, mrln_v16u16_t, >)
MRLN_BOP(mrln_v16u16_cmpleq, mrln_v16u16_t, <=)
MRLN_BOP(mrln_v16u16_cmpgeq, mrln_v16u16_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v16u16_and, mrln_v16u16_t, &)
MRLN_BOP(mrln_v16u16_or, mrln_v16u16_t, |)
MRLN_BOP(mrln_v16u16_xor, mrln_v16u16_t, ^)
MRLN_UOP(mrln_v16u16_not, mrln_v16u16_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint16_t[16]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint16_t[16] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v16u16_t mrln_v16u16_shift_left(const mrln_v16u16_t a,
                                                          const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_psllwi256(a, (int)n);
#else
  mrln_v8u16_t lower = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]};
  mrln_v8u16_t upper = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};

  lower = mrln_v8u16_shift_left(lower, n);
  upper = mrln_v8u16_shift_left(upper, n);

  return (mrln_v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      lower[4], lower[5], lower[6], lower[7],

      upper[0], upper[1], upper[2], upper[3],
      upper[4], upper[5], upper[6], upper[7],
  };
#endif //__AVX2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint16_t[16]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint16_t[16] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v16u16_t
mrln_v16u16_shift_right(const mrln_v16u16_t a, const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_psrlwi256(a, (int)n);
#else
  mrln_v8u16_t lower = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]};
  mrln_v8u16_t upper = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};

  lower = mrln_v8u16_shift_right(lower, n);
  upper = mrln_v8u16_shift_right(upper, n);

  return (mrln_v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      lower[4], lower[5], lower[6], lower[7],

      upper[0], upper[1], upper[2], upper[3],
      upper[4], upper[5], upper[6], upper[7],
  };
#endif //__AVX2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v16u16_mask(const mrln_v16u16_t a) {
  const mrln_v16u16_t tmp = mrln_v16u16_shift_right(a, 15);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  result |= tmp[8] << 8;
  result |= tmp[9] << 9;
  result |= tmp[10] << 10;
  result |= tmp[11] << 11;
  result |= tmp[12] << 12;
  result |= tmp[13] << 13;
  result |= tmp[14] << 14;
  result |= tmp[15] << 15;
  return result;
}

//==================================v16i16=======================================

typedef int16_t mrln_v16i16_t __attribute__((vector_size(32), aligned(32)));
typedef int16_t mrln_v16i16_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v16i16_t mrln_v16i16_set1(const int16_t v) {
  return (mrln_v16i16_t){
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
  };
}

MRLN_FN_ATTRS static mrln_v16i16_t
mrln_v16i16_set(const int16_t v00, const int16_t v01, const int16_t v02,
                const int16_t v03, const int16_t v04, const int16_t v05,
                const int16_t v06, const int16_t v07, const int16_t v08,
                const int16_t v09, const int16_t v10, const int16_t v11,
                const int16_t v12, const int16_t v13, const int16_t v14,
                const int16_t v15) {
  return (mrln_v16i16_t){v00, v01, v02, v03, v04, v05, v06, v07,
                         v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16i16_t
    mrln_v16i16_load_aligned(const mrln_v16i16_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v16i16_t
    mrln_v16i16_load_unaligned(const mrln_v16i16_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16i16_store_aligned(
    mrln_v16i16_t addr[static 1], const mrln_v16i16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v16i16_store_unaligned(
    mrln_v16i16_unaligned_t addr[static 1], const mrln_v16i16_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v16i16_add, mrln_v16i16_t, +)
MRLN_BOP(mrln_v16i16_sub, mrln_v16i16_t, -)
MRLN_BOP(mrln_v16i16_mul, mrln_v16i16_t, *)
MRLN_BOP(mrln_v16i16_div, mrln_v16i16_t, /)
MRLN_BOP(mrln_v16i16_mod, mrln_v16i16_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v16i16_cmpeq, mrln_v16i16_t, ==)
MRLN_BOP(mrln_v16i16_cmplt, mrln_v16i16_t, <)
MRLN_BOP(mrln_v16i16_cmpgt, mrln_v16i16_t, >)
MRLN_BOP(mrln_v16i16_cmpleq, mrln_v16i16_t, <=)
MRLN_BOP(mrln_v16i16_cmpgeq, mrln_v16i16_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v16i16_mask(const mrln_v16i16_t a) {
  const mrln_v16i16_t tmp = mrln_v16u16_shift_right(a, 15);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  result |= tmp[8] << 8;
  result |= tmp[9] << 9;
  result |= tmp[10] << 10;
  result |= tmp[11] << 11;
  result |= tmp[12] << 12;
  result |= tmp[13] << 13;
  result |= tmp[14] << 14;
  result |= tmp[15] << 15;
  return result;
}

//==================================v8u32=======================================

typedef uint32_t mrln_v8u32_t __attribute__((vector_size(32), aligned(32)));
typedef uint32_t mrln_v8u32_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v8u32_t mrln_v8u32_set1(const uint32_t v) {
  return (mrln_v8u32_t){v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v8u32_t
mrln_v8u32_set(const uint32_t v00, const uint32_t v01, const uint32_t v02,
               const uint32_t v03, const uint32_t v04, const uint32_t v05,
               const uint32_t v06, const uint32_t v07) {
  return (mrln_v8u32_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8u32_t
    mrln_v8u32_load_aligned(const mrln_v8u32_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8u32_t
    mrln_v8u32_load_unaligned(const mrln_v8u32_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8u32_store_aligned(
    mrln_v8u32_t addr[static 1], const mrln_v8u32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8u32_store_unaligned(
    mrln_v8u32_unaligned_t addr[static 1], const mrln_v8u32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v8u32_add, mrln_v8u32_t, +)
MRLN_BOP(mrln_v8u32_sub, mrln_v8u32_t, -)
MRLN_BOP(mrln_v8u32_mul, mrln_v8u32_t, *)
MRLN_BOP(mrln_v8u32_div, mrln_v8u32_t, /)
MRLN_BOP(mrln_v8u32_mod, mrln_v8u32_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v8u32_cmpeq, mrln_v8u32_t, ==)
MRLN_BOP(mrln_v8u32_cmplt, mrln_v8u32_t, <)
MRLN_BOP(mrln_v8u32_cmpgt, mrln_v8u32_t, >)
MRLN_BOP(mrln_v8u32_cmpleq, mrln_v8u32_t, <=)
MRLN_BOP(mrln_v8u32_cmpgeq, mrln_v8u32_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v8u32_and, mrln_v8u32_t, &)
MRLN_BOP(mrln_v8u32_or, mrln_v8u32_t, |)
MRLN_BOP(mrln_v8u32_xor, mrln_v8u32_t, ^)
MRLN_UOP(mrln_v8u32_not, mrln_v8u32_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint32_t[8]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint32_t[8] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v8u32_t mrln_v8u32_shift_left(const mrln_v8u32_t a,
                                                        const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_pslldi256(a, (int)n);
#else
  mrln_v4u32_t lower = {a[0], a[1], a[2], a[3]};
  mrln_v4u32_t upper = {a[4], a[5], a[6], a[7]};

  lower = mrln_v4u32_shift_left(lower, n);
  upper = mrln_v4u32_shift_left(upper, n);

  return (mrln_v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      upper[0], upper[1], upper[2], upper[3],
  };
#endif //__AVX2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint32_t[8]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint32_t[8] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v8u32_t mrln_v8u32_shift_right(const mrln_v8u32_t a,
                                                         const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_psrldi256(a, (int)n);
#else
  mrln_v4u32_t lower = {a[0], a[1], a[2], a[3]};
  mrln_v4u32_t upper = {a[4], a[5], a[6], a[7]};

  lower = mrln_v4u32_shift_right(lower, n);
  upper = mrln_v4u32_shift_right(upper, n);

  return (mrln_v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      upper[0], upper[1], upper[2], upper[3],
  };
#endif //__AVX2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v8u32_mask(const mrln_v8u32_t a) {
#ifdef __AVX__
  return __builtin_ia32_movmskps256(a);
#else
  const mrln_v8u32_t tmp = mrln_v8u32_shift_right(a, 31);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  return result;
#endif // __AVX__
}

//==================================v8i32=======================================

typedef int32_t mrln_v8i32_t __attribute__((vector_size(32), aligned(32)));
typedef int32_t mrln_v8i32_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v8i32_t mrln_v8i32_set1(const int32_t v) {
  return (mrln_v8i32_t){v, v, v, v, v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v8i32_t
mrln_v8i32_set(const int32_t v00, const int32_t v01, const int32_t v02,
               const int32_t v03, const int32_t v04, const int32_t v05,
               const int32_t v06, const int32_t v07) {
  return (mrln_v8i32_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8i32_t
    mrln_v8i32_load_aligned(const mrln_v8i32_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v8i32_t
    mrln_v8i32_load_unaligned(const mrln_v8i32_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8i32_store_aligned(
    mrln_v8i32_t addr[static 1], const mrln_v8i32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v8i32_store_unaligned(
    mrln_v8i32_unaligned_t addr[static 1], const mrln_v8i32_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v8i32_add, mrln_v8i32_t, +)
MRLN_BOP(mrln_v8i32_sub, mrln_v8i32_t, -)
MRLN_BOP(mrln_v8i32_mul, mrln_v8i32_t, *)
MRLN_BOP(mrln_v8i32_div, mrln_v8i32_t, /)
MRLN_BOP(mrln_v8i32_mod, mrln_v8i32_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v8i32_cmpeq, mrln_v8i32_t, ==)
MRLN_BOP(mrln_v8i32_cmplt, mrln_v8i32_t, <)
MRLN_BOP(mrln_v8i32_cmpgt, mrln_v8i32_t, >)
MRLN_BOP(mrln_v8i32_cmpleq, mrln_v8i32_t, <=)
MRLN_BOP(mrln_v8i32_cmpgeq, mrln_v8i32_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v8i32_mask(const mrln_v8i32_t a) {
#ifdef __AVX__
  return __builtin_ia32_movmskps256(a);
#else
  const mrln_v8u32_t tmp = mrln_v8u32_shift_right((mrln_v8u32_t)a, 31);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  result |= tmp[4] << 4;
  result |= tmp[5] << 5;
  result |= tmp[6] << 6;
  result |= tmp[7] << 7;
  return result;
#endif // __AVX__
}

//==================================v4u64=======================================

typedef uint64_t mrln_v4u64_t __attribute__((vector_size(32), aligned(32)));
typedef uint64_t mrln_v4u64_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v4u64_t mrln_v4u64_set1(const uint64_t v) {
  return (mrln_v4u64_t){v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v4u64_t mrln_v4u64_set(const uint64_t v00,
                                                 const uint64_t v01,
                                                 const uint64_t v02,
                                                 const uint64_t v03) {
  return (mrln_v4u64_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4u64_t
    mrln_v4u64_load_aligned(const mrln_v4u64_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4u64_t
    mrln_v4u64_load_unaligned(const mrln_v4u64_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4u64_store_aligned(
    mrln_v4u64_t addr[static 1], const mrln_v4u64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4u64_store_unaligned(
    mrln_v4u64_unaligned_t addr[static 1], const mrln_v4u64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v4u64_add, mrln_v4u64_t, +)
MRLN_BOP(mrln_v4u64_sub, mrln_v4u64_t, -)
MRLN_BOP(mrln_v4u64_mul, mrln_v4u64_t, *)
MRLN_BOP(mrln_v4u64_div, mrln_v4u64_t, /)
MRLN_BOP(mrln_v4u64_mod, mrln_v4u64_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v4u64_cmpeq, mrln_v4u64_t, ==)
MRLN_BOP(mrln_v4u64_cmplt, mrln_v4u64_t, <)
MRLN_BOP(mrln_v4u64_cmpgt, mrln_v4u64_t, >)
MRLN_BOP(mrln_v4u64_cmpleq, mrln_v4u64_t, <=)
MRLN_BOP(mrln_v4u64_cmpgeq, mrln_v4u64_t, >=)

//----------------------------------bitwise-------------------------------------
MRLN_BOP(mrln_v4u64_and, mrln_v4u64_t, &)
MRLN_BOP(mrln_v4u64_or, mrln_v4u64_t, |)
MRLN_BOP(mrln_v4u64_xor, mrln_v4u64_t, ^)
MRLN_UOP(mrln_v4u64_not, mrln_v4u64_t, ~)

/** doc
 * @description: Shifts each element of vector `a` left by immediate value `n`
 * @param a: vector of type uint64_t[4]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint64_t[4] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v4u64_t mrln_v4u64_shift_left(const mrln_v4u64_t a,
                                                        const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_psllqi256(a, (int)n);
#else
  mrln_v2u64_t lower = {a[0], a[1]};
  mrln_v2u64_t upper = {a[2], a[3]};

  lower = mrln_v2u64_shift_left(lower, n);
  upper = mrln_v2u64_shift_left(upper, n);

  return (mrln_v16u16_t){lower[0], lower[1], upper[0], upper[1]};
#endif //__AVX2__
}

/** doc
 * @description: Shifts each element of vector `a` right by immediate value `n`
 * @param a: vector of type uint64_t[4]
 * @param b: immediate value specifying the number of bits each element in a
 * will be shifted by
 * @return: vector of type uint64_t[4] with the shifted values
 */
MRLN_OP_ATTRS static mrln_v4u64_t mrln_v4u64_shift_right(const mrln_v4u64_t a,
                                                         const uint32_t n) {
#ifdef __AVX2__
  return __builtin_ia32_psrlqi256(a, (int)n);
#else
  mrln_v2u64_t lower = {a[0], a[1]};
  mrln_v2u64_t upper = {a[2], a[3]};

  lower = mrln_v2u64_shift_right(lower, n);
  upper = mrln_v2u64_shift_right(upper, n);

  return (mrln_v16u16_t){lower[0], lower[1], upper[0], upper[1]};
#endif //__AVX2__
}

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v4u64_mask(const mrln_v4u64_t a) {
  return __builtin_ia32_movmskpd256(a);
#ifdef __AVX__
#else
  const mrln_v4u64_t tmp = mrln_v4u64_shift_right(a, 63);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif // __AVX__
}

//==================================v4i64=======================================

typedef int64_t mrln_v4i64_t __attribute__((vector_size(32), aligned(32)));
typedef int64_t mrln_v4i64_unaligned_t
    __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------
MRLN_FN_ATTRS static mrln_v4i64_t mrln_v4i64_set1(const int64_t v) {
  return (mrln_v4i64_t){v, v, v, v};
}

MRLN_FN_ATTRS static mrln_v4i64_t mrln_v4i64_set(const int64_t v00,
                                                 const int64_t v01,
                                                 const int64_t v02,
                                                 const int64_t v03) {
  return (mrln_v4i64_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4i64_t
    mrln_v4i64_load_aligned(const mrln_v4i64_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_FN_ATTRS MRLN_NONNULL(1) static mrln_v4i64_t
    mrln_v4i64_load_unaligned(const mrln_v4i64_unaligned_t addr[static 1]) {
  MRLN_ASSUME(addr != NULL);
  return *addr;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4i64_store_aligned(
    mrln_v4i64_t addr[static 1], const mrln_v4i64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

MRLN_VOIDFN_ATTRS MRLN_NONNULL(1) static void mrln_v4i64_store_unaligned(
    mrln_v4i64_unaligned_t addr[static 1], const mrln_v4i64_t data) {
  MRLN_ASSUME(addr != NULL);
  *addr = data;
}

//----------------------------------arithmetic----------------------------------
MRLN_BOP(mrln_v4i64_add, mrln_v4i64_t, +)
MRLN_BOP(mrln_v4i64_sub, mrln_v4i64_t, -)
MRLN_BOP(mrln_v4i64_mul, mrln_v4i64_t, *)
MRLN_BOP(mrln_v4i64_div, mrln_v4i64_t, /)
MRLN_BOP(mrln_v4i64_mod, mrln_v4i64_t, %)

//----------------------------------cmp-----------------------------------------
MRLN_BOP(mrln_v4i64_cmpeq, mrln_v4i64_t, ==)
MRLN_BOP(mrln_v4i64_cmplt, mrln_v4i64_t, <)
MRLN_BOP(mrln_v4i64_cmpgt, mrln_v4i64_t, >)
MRLN_BOP(mrln_v4i64_cmpleq, mrln_v4i64_t, <=)
MRLN_BOP(mrln_v4i64_cmpgeq, mrln_v4i64_t, >=)

//----------------------------------mask----------------------------------------

MRLN_FN_ATTRS static uint32_t mrln_v4i64_mask(const mrln_v4i64_t a) {
  return __builtin_ia32_movmskpd256(a);
#ifdef __AVX__
#else
  const mrln_v4u64_t tmp = mrln_v4u64_shift_right((mrln_v4u64_t)a, 63);
  uint32_t result = 0;
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif // __AVX__
}

#undef MRLN_BOP
#undef MRLN_UOP
#undef MRLN_NODISCARD
#undef MRLN_ASSUME
#undef MRLN_NONNULL
#undef MRLN_VOIDFN_ATTRS
#undef MRLN_FN_ATTRS

#endif // MRLN_SIMD_H
