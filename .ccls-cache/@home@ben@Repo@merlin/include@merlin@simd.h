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

typedef uint8_t merlin_mask2_t;
typedef uint8_t merlin_mask4_t;
typedef uint8_t merlin_mask8_t;
typedef uint16_t merlin_mask16_t;
typedef uint32_t merlin_mask32_t;
typedef uint64_t merlin_mask64_t;

//==================================v16u8=======================================

typedef uint8_t merlin_v16u8_t __attribute__((vector_size(16), aligned(16)));
typedef uint8_t merlin_v16u8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask16_t
merlin_v16u8_mask(const merlin_v16u8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  merlin_mask16_t result = 0;
#pragma clang loop unroll(16)
  for (int i = 0; i < 16; i += 1) {
    result |= (((v[i] & 128) >> 7) << i);
  }
  return result;
#endif
}

//==================================v16i8=======================================

typedef int8_t merlin_v16i8_t __attribute__((vector_size(16), aligned(16)));
typedef int8_t merlin_v16i8_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_and(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_or(const merlin_v16i8_t a,
                                                      const merlin_v16i8_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_xor(const merlin_v16i8_t a,
                                                       const merlin_v16i8_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v16i8_t merlin_v16i8_not(const merlin_v16i8_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask16_t
merlin_v16i8_mask(const merlin_v16i8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  merlin_mask16_t result = 0;
#pragma clang loop unroll(16)
  for (int i = 0; i < 16; i += 1) {
    result |= (((v[i] & 128) >> 7) << i);
  }
  return result;
#endif
}

//==================================v8u16=======================================

typedef uint16_t merlin_v8u16_t __attribute__((vector_size(16), aligned(16)));
typedef uint16_t merlin_v8u16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask8_t
merlin_v8u16_mask(const merlin_v8u16_t a) {
#ifdef __SSE2__
  const merlin_mask16_t mask = __builtin_ia32_pmovmskb128(a);
  merlin_mask16_t result = 0;
#pragma clang loop unroll(full)
  for (merlin_mask16_t i = 1, s = 2; i <= 8; i += 1, s <<= 2) {
    result |= ((mask & s) >> i);
  }
  return result;
#else
  merlin_mask8_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 8; i += 1) {
    result |= (((v[i] & (1 << 15)) >> 7) << i);
  }
  return result;
#endif
}

//==================================v8i16=======================================

typedef int16_t merlin_v8i16_t __attribute__((vector_size(16), aligned(16)));
typedef int16_t merlin_v8i16_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_and(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_or(const merlin_v8i16_t a,
                                                      const merlin_v8i16_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_xor(const merlin_v8i16_t a,
                                                       const merlin_v8i16_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v8i16_t merlin_v8i16_not(const merlin_v8i16_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask8_t
merlin_v8i16_mask(const merlin_v8i16_t a) {
#ifdef __SSE2__
  const merlin_mask16_t mask = __builtin_ia32_pmovmskb128(a);
  merlin_mask16_t result = 0;
#pragma clang loop unroll(full)
  for (merlin_mask16_t i = 1, s = 2; i <= 8; i += 1, s <<= 2) {
    result |= ((mask & s) >> i);
  }
  return result;
#else
  merlin_mask8_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 8; i += 1) {
    result |= (((v[i] & (1 << 15)) >> 7) << i);
  }
  return result;
#endif
}

//==================================v4u32=======================================

typedef uint32_t merlin_v4u32_t __attribute__((vector_size(16), aligned(16)));
typedef uint32_t merlin_v4u32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask4_t
merlin_v4u32_mask(const merlin_v4u32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  merlin_mask4_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 4; i += 1) {
    result |= (((v[i] & (1 << 31)) >> 7) << i);
  }
  return result;
#endif
}

//==================================v4i32=======================================

typedef int32_t merlin_v4i32_t __attribute__((vector_size(16), aligned(16)));
typedef int32_t merlin_v4i32_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_and(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_or(const merlin_v4i32_t a,
                                                      const merlin_v4i32_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_xor(const merlin_v4i32_t a,
                                                       const merlin_v4i32_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v4i32_t merlin_v4i32_not(const merlin_v4i32_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask4_t
merlin_v4i32_mask(const merlin_v4i32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  merlin_mask4_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 4; i += 1) {
    result |= (((v[i] & (1 << 31)) >> 7) << i);
  }
  return result;
#endif
}

//==================================v2u64=======================================

typedef uint64_t merlin_v2u64_t __attribute__((vector_size(16), aligned(16)));
typedef uint64_t merlin_v2u64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask2_t
merlin_v2u64_mask(const merlin_v2u64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  merlin_mask4_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 2; i += 1) {
    result |= (((v[i] & (1 << 63)) >> 7) << i);
  }
  return result;
#endif
}

//==================================v2i64=======================================

typedef int64_t merlin_v2i64_t __attribute__((vector_size(16), aligned(16)));
typedef int64_t merlin_v2i64_unaligned_t
    __attribute__((vector_size(16), aligned(1)));

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

//----------------------------------bitwise-------------------------------------
MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_and(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a & b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_or(const merlin_v2i64_t a,
                                                      const merlin_v2i64_t b) {
  return a | b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_xor(const merlin_v2i64_t a,
                                                       const merlin_v2i64_t b) {
  return a ^ b;
}

MERLIN_FN_ATTRS static merlin_v2i64_t merlin_v2i64_not(const merlin_v2i64_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

MERLIN_FN_ATTRS static merlin_mask2_t
merlin_v2i64_mask(const merlin_v2i64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  merlin_mask4_t result = 0;
#pragma clang loop unroll(full)
  for (int i = 0; i < 2; i += 1) {
    result |= (((v[i] & (1 << 63)) >> 7) << i);
  }
  return result;
#endif
}

#undef MERLIN_NODISCARD
#undef MERLIN_ASSUME
#undef MERLIN_NONNULL
#undef MERLIN_VOIDFN_ATTRS
#undef MERLIN_FN_ATTRS

#endif // MERLIN_SIMD_H
