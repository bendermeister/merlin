#ifndef MERLIN_SIMD_H
#define MERLIN_SIMD_H

// TODO(ben): blend functions
// TODO(ben): mul low / high ?
// TODO(ben): align function moves the elements inside the vector to different
// positions
// TODO(ben): floating point vector
// TODO(ben): abs functions for the signed vectors
// TODO(ben): max min
// TODO(ben): shuffle
// TODO(ben): documentation
// TODO(ben): __builtin_shufflevector(vec1, vec2, index1, index2, ...)

#include <sys/cdefs.h>
#ifndef __clang__
#error "currently only clang is supported"
#endif

#include <merlin/core.h>
#include <stddef.h>

//==================================v16u8=======================================

typedef uint8_t v16u8_t __attribute__((vector_size(16), aligned(16)));
typedef uint8_t v16u8_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v16u8_set1
// ## return
// returns a `v16u8_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_set1(const uint8_t v) {
  return (v16u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

// # v16u8_set
// ## return
// returns a `v16u8_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_set(const uint8_t v00, const uint8_t v01, const uint8_t v02,
          const uint8_t v03, const uint8_t v04, const uint8_t v05,
          const uint8_t v06, const uint8_t v07, const uint8_t v08,
          const uint8_t v09, const uint8_t v10, const uint8_t v11,
          const uint8_t v12, const uint8_t v13, const uint8_t v14,
          const uint8_t v15) {
  return (v16u8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                   v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

// # v16u8_load_aligned
// ## Description loads a `v16u8_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16u8_t
v16u8_load_aligned(const v16u8_t *addr) {
  return *addr;
}

// # v16u8_load_unaligned
// ## Description loads a `v16u8_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16u8_t
v16u8_load_unaligned(const v16u8_unaligned_t *addr) {
  return *addr;
}

// # v16u8_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16u8_store_aligned(v16u8_t *addr,
                                                   const v16u8_t data) {
  *addr = data;
}

// # v16u8_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16u8_store_unaligned(v16u8_unaligned_t *addr,
                                                     const v16u8_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v16u8_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_add(const v16u8_t a, const v16u8_t b) {
  return a + b;
}

// # v16u8_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_sub(const v16u8_t a, const v16u8_t b) {
  return a - b;
}

// # v16u8_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_mul(const v16u8_t a, const v16u8_t b) {
  return a * b;
}

// # v16u8_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_div(const v16u8_t a, const v16u8_t b) {
  return a / b;
}

// # v16u8_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_mod(const v16u8_t a, const v16u8_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v16u8_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_eq(const v16u8_t a, const v16u8_t b) {
  return a == b;
}

// # v16u8_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_lt(const v16u8_t a, const v16u8_t b) {
  return a < b;
}

// # v16u8_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_gt(const v16u8_t a, const v16u8_t b) {
  return a > b;
}

// # v16u8_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_leq(const v16u8_t a, const v16u8_t b) {
  return a <= b;
}

// # v16u8_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_geq(const v16u8_t a, const v16u8_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v16u8_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_and(const v16u8_t a, const v16u8_t b) {
  return a & b;
}

// # v16u8_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_or(const v16u8_t a, const v16u8_t b) {
  return a | b;
}

// # v16u8_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_xor(const v16u8_t a, const v16u8_t b) {
  return a ^ b;
}

// # v16u8_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_sl(const v16u8_t a, const int n) {
#ifdef __SSE2__
  v16u8_t tmp = __builtin_ia32_psllwi128(a, n);
  tmp = v16u8_and(tmp, v16u8_set1((uint8_t)-1 << n));
  return tmp;
#else
  return (v16u8_t){a[0] << n,  a[1] << n,  a[2] << n,  a[3] << n,
                   a[4] << n,  a[5] << n,  a[6] << n,  a[7] << n,
                   a[8] << n,  a[9] << n,  a[10] << n, a[11] << n,
                   a[12] << n, a[13] << n, a[14] << n, a[15] << n};
#endif // __SSE2__
}

// # v16u8_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_sr(const v16u8_t a, const int n) {
#ifdef __SSE2__
  v16u8_t tmp = __builtin_ia32_psrlwi128(a, (int)n);
  tmp = v16u8_and(tmp, v16u8_set1((uint8_t)-1 >> n));
  return tmp;
#else
  return (v16u8_t){a[0] >> n,  a[1] >> n,  a[2] >> n,  a[3] >> n,
                   a[4] >> n,  a[5] >> n,  a[6] >> n,  a[7] >> n,
                   a[8] >> n,  a[9] >> n,  a[10] >> n, a[11] >> n,
                   a[12] >> n, a[13] >> n, a[14] >> n, a[15] >> n};
#endif //__SSE2__
}

// # v16u8_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16u8_not(const v16u8_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v16u8_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 16; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v16u8_mask(const v16u8_t a) {
#ifdef __SSE2__
  return __builtin_ia32_pmovmskb128(a);
#else
  uint32_t result = 0;
  const v16u8_t tmp = v16u8_shift_right(a, 7);
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

//==================================types=======================================
typedef i8 v16i8_t __attribute__((__vector_size__(16), __aligned__(16)));
typedef i8 v16i8_unaligned_t
    __attribute__((__vector_size__(16), __aligned__(1)));
//==================================set=========================================

// # v16u8_set1
// ## return
// returns a `v16u8_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16i8_set1(const i8 v) {
  return (v16u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

// # v16u8_set
// ## return
// returns a `v16u8_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u8_t
v16i8_set(const i8 v00, const i8 v01, const i8 v02, const i8 v03, const i8 v04,
          const i8 v05, const i8 v06, const i8 v07, const i8 v08, const i8 v09,
          const i8 v10, const i8 v11, const i8 v12, const i8 v13, const i8 v14,
          const i8 v15) {
  return (v16u8_t){v00, v01, v02, v03, v04, v05, v06, v07,
                   v08, v09, v10, v11, v12, v13, v14, v15};
}

//==================================load/store==================================
// # v16i8_load_aligned
// ## Description
// loads a `v16i8_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16i8_t
v16i8_load_aligned(const v16i8_t *addr) {
  return *addr;
}

// # v16i8_load_unaligned
// ## Description
// loads a `v16i8_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16i8_t
v16i8_load_unaligned(const v16i8_unaligned_t *addr) {
  return *addr;
}

// # v16i8_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16i8_store_aligned(v16i8_t *addr,
                                                   const v16i8_t data) {
  *addr = data;
}

// # v16i8_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16i8_store_unaligned(v16i8_unaligned_t *addr,
                                                     const v16i8_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v16i8_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating a and b like their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_add(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_add((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_sub(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_sub((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_mul(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_mul((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_div(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_div((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` like their unsigned
// counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_mod(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_mod((v16u8_t)a, (v16u8_t)b);
}

//==================================cmp=========================================
// # v16i8_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_eq(const v16i8_t a, const v16i8_t b) {
  return a == b;
}

// # v16i8_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_lt(const v16i8_t a, const v16i8_t b) {
  return a < b;
}

// # v16i8_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_gt(const v16i8_t a, const v16i8_t b) {
  return a > b;
}

// # v16i8_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_leq(const v16i8_t a, const v16i8_t b) {
  return a <= b;
}

// # v16i8_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_geq(const v16i8_t a, const v16i8_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v16i8_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treats each member as its unsigned counterpart.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = (u8)a[i] & (u8)b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_and(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_and((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treats each member as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = (u8)a[i] | (u8)b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_or(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_or((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treat each member as its unsigned counterpart.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = (u8)a[i] ^ (u8)b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_xor(const v16i8_t a, const v16i8_t b) {
  return (v16i8_t)v16u8_xor((v16u8_t)a, (v16u8_t)b);
}

// # v16i8_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treats each member as its unsigned counterpart.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = (u8)a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_sl(const v16i8_t a, const int n) {
  return (v16i8_t)v16u8_sl((v16u8_t)a, n);
}

// # v16i8_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treats each member as its unsigned counterpart.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = (u8)a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_sr(const v16i8_t a, const int n) {
  return (v16i8_t)v16u8_sr((v16u8_t)a, n);
}

// # v16i8_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treats each member as its unsigned counterpart.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i8_t
v16i8_not(const v16i8_t a) {
  return (v16i8_t)v16u8_not((v16u8_t)a);
}

//----------------------------------mask----------------------------------------

// # v16i8_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`.
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 16; ++i) {
//   if ((u8)a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v16i8_mask(const v16i8_t a) {
  return v16u8_mask((v16u8_t)a);
}

//==================================v8u16=======================================

typedef u16 v8u16_t __attribute__((vector_size(16), aligned(16)));
typedef u16 v8u16_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v8u16_set1
// ## Return
// returns a `v8u16_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_set1(const u16 v) {
  return (v8u16_t){v, v, v, v, v, v, v, v};
}

// # v8u16_set
// ## Return
// Returns a `v8u16_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_set(const uint16_t v00, const uint16_t v01, const uint16_t v02,
          const uint16_t v03, const uint16_t v04, const uint16_t v05,
          const uint16_t v06, const uint16_t v07) {
  return (v8u16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

// # v8u16_load_aligned
// ## Description loads a `v8u16_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8u16_t
v8u16_load_aligned(const v8u16_t *addr) {
  return *addr;
}

// # v8u16_load_unaligned
// ## Description loads a `v8u16_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8u16_t
v8u16_load_unaligned(const v8u16_unaligned_t *addr) {
  return *addr;
}

// # v8u16_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8u16_store_aligned(v8u16_t *addr,
                                                   const v8u16_t data) {
  *addr = data;
}

// # v8u16_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8u16_store_unaligned(v8u16_unaligned_t *addr,
                                                     const v8u16_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v8u16_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_add(const v8u16_t a, const v8u16_t b) {
  return a + b;
}

// # v8u16_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_sub(const v8u16_t a, const v8u16_t b) {
  return a - b;
}

// # v8u16_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_mul(const v8u16_t a, const v8u16_t b) {
  return a * b;
}

// # v8u16_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_div(const v8u16_t a, const v8u16_t b) {
  return a / b;
}

// # v8u16_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_mod(const v8u16_t a, const v8u16_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v8u16_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_eq(const v8u16_t a, const v8u16_t b) {
  return a == b;
}

// # v8u16_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_lt(const v8u16_t a, const v8u16_t b) {
  return a < b;
}

// # v8u16_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_gt(const v8u16_t a, const v8u16_t b) {
  return a > b;
}

// # v8u16_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_leq(const v8u16_t a, const v8u16_t b) {
  return a <= b;
}

// # v8u16_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_geq(const v8u16_t a, const v8u16_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v8u16_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_and(const v8u16_t a, const v8u16_t b) {
  return a & b;
}

// # v8u16_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_or(const v8u16_t a, const v8u16_t b) {
  return a | b;
}

// # v8u16_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_xor(const v8u16_t a, const v8u16_t b) {
  return a ^ b;
}

// # v8u16_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_sl(const v8u16_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_psllwi128(a, n);
#else
  return (v8u16_t){a[0] << n, a[1] << n, a[2] << n, a[3] << n,
                   a[4] << n, a[5] << n, a[6] << n, a[7] << n};
#endif //__SSE2__
}

// # v8u16_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_sr(const v8u16_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_psrlwi128(a, (int)n);
#else
  return (v8u16_t){a[0] >> n, a[1] >> n, a[2] >> n, a[3] >> n,
                   a[4] >> n, a[5] >> n, a[6] >> n, a[7] >> n};
#endif //__SSE2__
}

// # v8u16_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u16_t
v8u16_not(const v8u16_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v8u16_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 8; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v8u16_mask(const v8u16_t a) {
  uint32_t result = 0;
  const v8u16_t tmp = v8u16_sr(a, 15);
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

typedef i16 v8i16_t __attribute__((vector_size(16), aligned(16)));
typedef i16 v8i16_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v8i16_set1
// ## Return
// returns a `v8i16_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_set1(const i16 v) {
  return (v8i16_t){v, v, v, v, v, v, v, v};
}

// # v8i16_set
// ## Return
// Returns a `v8i16_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_set(const i16 v00, const i16 v01, const i16 v02, const i16 v03,
          const i16 v04, const i16 v05, const i16 v06, const i16 v07) {
  return (v8i16_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

// # v8i16_load_aligned
// ## Description loads a `v8i16_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8i16_t
v8i16_load_aligned(const v8i16_t *addr) {
  return *addr;
}

// # v8i16_load_unaligned
// ## Description loads a `v8i16_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8i16_t
v8i16_load_unaligned(const v8i16_unaligned_t *addr) {
  return *addr;
}

// # v8i16_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8i16_store_aligned(v8i16_t *addr,
                                                   const v8i16_t data) {
  *addr = data;
}

// # v8i16_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8i16_store_unaligned(v8i16_unaligned_t *addr,
                                                     const v8i16_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v8i16_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_add(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_add((v8u16_t)a, (v8u16_t)b);
}

// # v8i16_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_sub(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_sub((v8u16_t)a, (v8u16_t)b);
}

// # v8i16_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_mul(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_mul((v8u16_t)a, (v8u16_t)b);
  return a * b;
}

// # v8i16_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_div(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_div((v8u16_t)a, (v8u16_t)b);
  return a / b;
}

// # v8i16_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` like their unsigned
// counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_mod(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_mod((v8u16_t)a, (v8u16_t)b);
  return a % b;
}

//==================================cmp=========================================
// # v8i16_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_eq(const v8i16_t a, const v8i16_t b) {
  return a == b;
}

// # v8i16_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_lt(const v8i16_t a, const v8i16_t b) {
  return a < b;
}

// # v8i16_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_gt(const v8i16_t a, const v8i16_t b) {
  return a > b;
}

// # v8i16_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_leq(const v8i16_t a, const v8i16_t b) {
  return a <= b;
}

// # v8i16_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_geq(const v8i16_t a, const v8i16_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v8i16_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_and(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_and((v8u16_t)a, (v8u16_t)b);
  return a & b;
}

// # v8i16_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_or(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_or((v8u16_t)a, (v8u16_t)b);
  return a | b;
}

// # v8i16_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_xor(const v8i16_t a, const v8i16_t b) {
  return (v8i16_t)v8u16_xor((v8u16_t)a, (v8u16_t)b);
  return a ^ b;
}

// # v8i16_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_sl(const v8i16_t a, const int n) {
  return (v8i16_t)v8u16_sl((v8u16_t)a, n);
}

// # v8i16_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_sr(const v8i16_t a, const int n) {
  return (v8i16_t)v8u16_sr((v8u16_t)a, n);
}

// # v8i16_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` and `b` like their unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i16_t
v8i16_not(const v8i16_t a) {
  return (v8i16_t)v8u16_not((v8u16_t)a);
}

//----------------------------------mask----------------------------------------

// # v8i16_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`.
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 8; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v8i16_mask(const v8i16_t a) {
  return v8u16_mask((v8u16_t)a);
}

//==================================v4u32=======================================

typedef u32 v4u32_t __attribute__((vector_size(16), aligned(16)));
typedef u32 v4u32_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v4u32_set1
// ## Return
// returns a `v4u32_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_set1(const uint32_t v) {
  return (v4u32_t){v, v, v, v};
}

// # v4u32_set
// ## Return
// Returns a `v4u32_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_set(const uint32_t v00, const uint32_t v01, const uint32_t v02,
          const uint32_t v03) {
  return (v4u32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

// # v4u32_load_aligned
// ## Description loads a `v4u32_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4u32_t
v4u32_load_aligned(const v4u32_t *addr) {
  return *addr;
}

// # v4u32_load_unaligned
// ## Description loads a `v4u32_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4u32_t
v4u32_load_unaligned(const v4u32_unaligned_t *addr) {
  return *addr;
}

// # v4u32_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4u32_store_aligned(v4u32_t *addr,
                                                   const v4u32_t data) {
  *addr = data;
}

// # v4u32_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4u32_store_unaligned(v4u32_unaligned_t *addr,
                                                     const v4u32_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v4u32_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_add(const v4u32_t a, const v4u32_t b) {
  return a + b;
}

// # v4u32_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_sub(const v4u32_t a, const v4u32_t b) {
  return a - b;
}

// # v4u32_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_mul(const v4u32_t a, const v4u32_t b) {
  return a * b;
}

// # v4u32_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_div(const v4u32_t a, const v4u32_t b) {
  return a / b;
}

// # v4u32_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_mod(const v4u32_t a, const v4u32_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v4u32_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_eq(const v4u32_t a, const v4u32_t b) {
  return a == b;
}

// # v4u32_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_lt(const v4u32_t a, const v4u32_t b) {
  return a < b;
}

// # v4u32_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_gt(const v4u32_t a, const v4u32_t b) {
  return a > b;
}

// # v4u32_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_leq(const v4u32_t a, const v4u32_t b) {
  return a <= b;
}

// # v4u32_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_geq(const v4u32_t a, const v4u32_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v4u32_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_and(const v4u32_t a, const v4u32_t b) {
  return a & b;
}

// # v4u32_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_or(const v4u32_t a, const v4u32_t b) {
  return a | b;
}

// # v4u32_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_xor(const v4u32_t a, const v4u32_t b) {
  return a ^ b;
}

// # v4u32_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_sl(const v4u32_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_pslldi128(a, (int)n);
#else
  return (v4u32_t){a[0] << n, a[1] << n, a[2] << n, a[3] << n};
#endif //__SSE2__
}

// # v4u32_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_sr(const v4u32_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_psrldi128(a, (int)n);
#else
  return (v4u32_t){a[0] >> n, a[1] >> n, a[2] >> n, a[3] >> n};
#endif //__SSE2__
}

// # v4u32_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u32_t
v4u32_not(const v4u32_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v4u32_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 4; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v4u32_mask(const v4u32_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskps(a);
#else
  uint32_t result = 0;
  const v4u32_t tmp = v4u32_shift_right(a, 31);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
  result |= tmp[2] << 2;
  result |= tmp[3] << 3;
  return result;
#endif
}

//==================================v4i32=======================================

typedef i32 v4i32_t __attribute__((vector_size(16), aligned(16)));
typedef i32 v4i32_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v4i32_set1
// ## Return
// returns a `v4i32_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_set1(const i32 v) {
  return (v4i32_t){v, v, v, v};
}

// # v4i32_set
// ## Return
// Returns a `v4i32_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_set(const i32 v00, const i32 v01, const i32 v02, const i32 v03) {
  return (v4i32_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

// # v4i32_load_aligned
// ## Description loads a `v4i32_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4i32_t
v4i32_load_aligned(const v4i32_t *addr) {
  return *addr;
}

// # v4i32_load_unaligned
// ## Description loads a `v4i32_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4i32_t
v4i32_load_unaligned(const v4i32_unaligned_t *addr) {
  return *addr;
}

// # v4i32_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4i32_store_aligned(v4i32_t *addr,
                                                   const v4i32_t data) {
  *addr = data;
}

// # v4i32_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4i32_store_unaligned(v4i32_unaligned_t *addr,
                                                     const v4i32_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v4i32_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_add(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_add((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_sub(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_sub((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_mul(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_mul((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_div(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_div((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_mod(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_mod((v4u32_t)a, (v4u32_t)b);
}

//==================================cmp=========================================
// # v4i32_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_eq(const v4i32_t a, const v4i32_t b) {
  return a == b;
}

// # v4i32_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_lt(const v4i32_t a, const v4i32_t b) {
  return a < b;
}

// # v4i32_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_gt(const v4i32_t a, const v4i32_t b) {
  return a > b;
}

// # v4i32_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_leq(const v4i32_t a, const v4i32_t b) {
  return a <= b;
}

// # v4i32_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_geq(const v4i32_t a, const v4i32_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v4i32_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_and(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_and((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_or(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_or((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_xor(const v4i32_t a, const v4i32_t b) {
  return (v4i32_t)v4u32_xor((v4u32_t)a, (v4u32_t)b);
}

// # v4i32_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_sl(const v4i32_t a, const int n) {
  return (v4i32_t)v4u32_sl((v4u32_t)a, n);
}

// # v4i32_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` as the unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_sr(const v4i32_t a, const int n) {
  return (v4i32_t)v4u32_sr((v4u32_t)a, n);
}

// # v4i32_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` as its unsigned counterpats
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i32_t
v4i32_not(const v4i32_t a) {
  return (v4i32_t)v4u32_not((v4u32_t)a);
}

//----------------------------------mask----------------------------------------

// # v4i32_mask
// ## Description return a `i32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// i32 mask = 0;
// for (int i = 0; i < 4; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v4i32_mask(const v4i32_t a) {
  return v4u32_mask((v4u32_t)a);
}

//==================================v2u64=======================================

typedef u64 v2u64_t __attribute__((vector_size(16), aligned(16)));
typedef u64 v2u64_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v2u64_set1
// ## Return
// returns a `v2u64_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_set1(const u64 v) {
  return (v2u64_t){v, v};
}

// # v2u64_set
// ## Return
// Returns a `v2u64_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_set(const u64 v00, const u64 v01) {
  return (v2u64_t){v00, v01};
}

//----------------------------------load/store----------------------------------

// # v2u64_load_aligned
// ## Description loads a `v2u64_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v2u64_t
v2u64_load_aligned(const v2u64_t *addr) {
  return *addr;
}

// # v2u64_load_unaligned
// ## Description loads a `v2u64_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v2u64_t
v2u64_load_unaligned(const v2u64_unaligned_t *addr) {
  return *addr;
}

// # v2u64_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v2u64_store_aligned(v2u64_t *addr,
                                                   const v2u64_t data) {
  *addr = data;
}

// # v2u64_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v2u64_store_unaligned(v2u64_unaligned_t *addr,
                                                     const v2u64_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v2u64_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_add(const v2u64_t a, const v2u64_t b) {
  return a + b;
}

// # v2u64_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_sub(const v2u64_t a, const v2u64_t b) {
  return a - b;
}

// # v2u64_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_mul(const v2u64_t a, const v2u64_t b) {
  return a * b;
}

// # v2u64_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_div(const v2u64_t a, const v2u64_t b) {
  return a / b;
}

// # v2u64_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_mod(const v2u64_t a, const v2u64_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v2u64_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_eq(const v2u64_t a, const v2u64_t b) {
  return a == b;
}

// # v2u64_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_lt(const v2u64_t a, const v2u64_t b) {
  return a < b;
}

// # v2u64_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_gt(const v2u64_t a, const v2u64_t b) {
  return a > b;
}

// # v2u64_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_leq(const v2u64_t a, const v2u64_t b) {
  return a <= b;
}

// # v2u64_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_geq(const v2u64_t a, const v2u64_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v2u64_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_and(const v2u64_t a, const v2u64_t b) {
  return a & b;
}

// # v2u64_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_or(const v2u64_t a, const v2u64_t b) {
  return a | b;
}

// # v2u64_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_xor(const v2u64_t a, const v2u64_t b) {
  return a ^ b;
}

// # v2u64_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 2; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_sl(const v2u64_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_psllqi128(a, (int)n);
#else
  return (v2u64_t){a[0] << n, a[1] << n};
#endif //__SSE2__
}

// # v2u64_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 2; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_sr(const v2u64_t a, const int n) {
#ifdef __SSE2__
  return __builtin_ia32_psrlqi128(a, (int)n);
#else
  return (v2u64_t){a[0] >> n, a[1] >> n};
#endif //__SSE2__
}

// # v2u64_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2u64_t
v2u64_not(const v2u64_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v2u64_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u64 mask = 0;
// for (int i = 0; i < 2; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v2u64_mask(const v2u64_t a) {
#ifdef __SSE2__
  return __builtin_ia32_movmskpd(a);
#else
  u32 result = 0;
  const v2u64_t tmp = v2u64_shift_right(a, 63);
  result |= tmp[0] << 0;
  result |= tmp[1] << 1;
#endif
}

//==================================v2i64=======================================

typedef i64 v2i64_t __attribute__((vector_size(16), aligned(16)));
typedef i64 v2i64_unaligned_t __attribute__((vector_size(16), aligned(1)));

//----------------------------------set-----------------------------------------

// # v2i64_set1
// ## Return
// returns a `v2i64_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_set1(const i64 v) {
  return (v2i64_t){v, v};
}

// # v2i64_set
// ## Return
// Returns a `v2i64_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_set(const i64 v00, const i64 v01) {
  return (v2i64_t){v00, v01};
}

//----------------------------------load/store----------------------------------

// # v2i64_load_aligned
// ## Description loads a `v2i64_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v2i64_t
v2i64_load_aligned(const v2i64_t *addr) {
  return *addr;
}

// # v2i64_load_unaligned
// ## Description loads a `v2i64_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v2i64_t
v2i64_load_unaligned(const v2i64_unaligned_t *addr) {
  return *addr;
}

// # v2i64_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v2i64_store_aligned(v2i64_t *addr,
                                                   const v2i64_t data) {
  *addr = data;
}

// # v2i64_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v2i64_store_unaligned(v2i64_unaligned_t *addr,
                                                     const v2i64_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v2i64_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_add(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_add((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_sub(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_sub((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_mul(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_mul((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_div(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_div((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` as their unsigned
// counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_mod(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_mod((v2u64_t)a, (v2u64_t)b);
}

//==================================cmp=========================================
// # v2i64_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_eq(const v2i64_t a, const v2i64_t b) {
  return a == b;
}

// # v2i64_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_lt(const v2i64_t a, const v2i64_t b) {
  return a < b;
}

// # v2i64_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_gt(const v2i64_t a, const v2i64_t b) {
  return a > b;
}

// # v2i64_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_leq(const v2i64_t a, const v2i64_t b) {
  return a <= b;
}

// # v2i64_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_geq(const v2i64_t a, const v2i64_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v2i64_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_and(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_and((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_or(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_or((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_xor(const v2i64_t a, const v2i64_t b) {
  return (v2i64_t)v2u64_xor((v2u64_t)a, (v2u64_t)b);
}

// # v2i64_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 2; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_sl(const v2i64_t a, const int n) {
  return (v2i64_t)v2u64_sl((v2u64_t)a, n);
}

// # v2i64_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 2; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_sr(const v2i64_t a, const int n) {
  return (v2i64_t)v2u64_sr((v2u64_t)a, n);
}

// # v2i64_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 2; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v2i64_t
v2i64_not(const v2i64_t a) {
  return (v2i64_t)v2u64_not((v2u64_t)a);
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v2i64_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// i64 mask = 0;
// for (int i = 0; i < 2; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v2i64_mask(const v2i64_t a) {
  return v2u64_mask((v2u64_t)a);
}

//==================================v32u8=======================================

typedef u8 v32u8_t __attribute__((vector_size(32), aligned(32)));
typedef u8 v32u8_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v32u8_set1
// ## Return
// returns a `v32u8_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_set1(const u8 v) {
  return (v32u8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
                   v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

// # v32u8_set
// ## Return
// Returns a `v32u8_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_set(const u8 v00, const u8 v01, const u8 v02, const u8 v03, const u8 v04,
          const u8 v05, const u8 v06, const u8 v07, const u8 v08, const u8 v09,
          const u8 v10, const u8 v11, const u8 v12, const u8 v13, const u8 v14,
          const u8 v15, const u8 v16, const u8 v17, const u8 v18, const u8 v19,
          const u8 v20, const u8 v21, const u8 v22, const u8 v23, const u8 v24,
          const u8 v25, const u8 v26, const u8 v27, const u8 v28, const u8 v29,
          const u8 v30, const u8 v31) {
  return (v32u8_t){v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10,
                   v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21,
                   v22, v23, v24, v25, v26, v27, v28, v29, v30, v31};
}

//----------------------------------load/store----------------------------------

// # v32u8_load_aligned
// ## Description loads a `v32u8_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v32u8_t
v32u8_load_aligned(const v32u8_t *addr) {
  return *addr;
}

// # v32u8_load_unaligned
// ## Description loads a `v32u8_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v32u8_t
v32u8_load_unaligned(const v32u8_unaligned_t *addr) {
  return *addr;
}

// # v32u8_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v32u8_store_aligned(v32u8_t *addr,
                                                   const v32u8_t data) {
  *addr = data;
}

// # v32u8_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v32u8_store_unaligned(v32u8_unaligned_t *addr,
                                                     const v32u8_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v32u8_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_add(const v32u8_t a, const v32u8_t b) {
  return a + b;
}

// # v32u8_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_sub(const v32u8_t a, const v32u8_t b) {
  return a - b;
}

// # v32u8_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_mul(const v32u8_t a, const v32u8_t b) {
  return a * b;
}

// # v32u8_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_div(const v32u8_t a, const v32u8_t b) {
  return a / b;
}

// # v32u8_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_mod(const v32u8_t a, const v32u8_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v32u8_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_eq(const v32u8_t a, const v32u8_t b) {
  return a == b;
}

// # v32u8_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_lt(const v32u8_t a, const v32u8_t b) {
  return a < b;
}

// # v32u8_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_gt(const v32u8_t a, const v32u8_t b) {
  return a > b;
}

// # v32u8_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_leq(const v32u8_t a, const v32u8_t b) {
  return a <= b;
}

// # v32u8_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_geq(const v32u8_t a, const v32u8_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v32u8_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_and(const v32u8_t a, const v32u8_t b) {
  return a & b;
}

// # v32u8_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_or(const v32u8_t a, const v32u8_t b) {
  return a | b;
}

// # v32u8_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_xor(const v32u8_t a, const v32u8_t b) {
  return a ^ b;
}

// # v32u8_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 32; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_sl(const v32u8_t a, const int n) {
#ifdef __AVX2__
  v32u8_t tmp = __builtin_ia32_psllwi256(a, (int)n);
  tmp = v32u8_and(tmp, v32u8_set1((uint8_t)-1 << n));
  return tmp;
#else
  v16u8_t lower = {a[0], a[1], a[2],  a[3],  a[4],  a[5],  a[6],  a[7],
                   a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
  v16u8_t upper = {a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
                   a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]};

  lower = v16u8_shift_left(lower, n);
  upper = v16u8_shift_left(upper, n);

  return (v32u8_t){
      lower[0],  lower[1],  lower[2],  lower[3],  lower[4],  lower[5],
      lower[6],  lower[7],  lower[8],  lower[9],  lower[10], lower[11],
      lower[12], lower[13], lower[14], lower[15],

      upper[0],  upper[1],  upper[2],  upper[3],  upper[4],  upper[5],
      upper[6],  upper[7],  upper[8],  upper[9],  upper[10], upper[11],
      upper[12], upper[13], upper[14], upper[15],
  };
#endif //__AVX2__
}

// # v32u8_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 32; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_sr(const v32u8_t a, const int n) {
#ifdef __AVX2__
  v32u8_t tmp = __builtin_ia32_psrlwi256(a, (int)n);
  tmp = v32u8_and(tmp, v32u8_set1((uint8_t)-1 >> n));
  return tmp;
#else
  v16u8_t lower = {a[0], a[1], a[2],  a[3],  a[4],  a[5],  a[6],  a[7],
                   a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
  v16u8_t upper = {a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
                   a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]};

  lower = v16u8_shift_right(lower, n);
  upper = v16u8_shift_right(upper, n);

  return (v32u8_t){
      lower[0],  lower[1],  lower[2],  lower[3],  lower[4],  lower[5],
      lower[6],  lower[7],  lower[8],  lower[9],  lower[10], lower[11],
      lower[12], lower[13], lower[14], lower[15],

      upper[0],  upper[1],  upper[2],  upper[3],  upper[4],  upper[5],
      upper[6],  upper[7],  upper[8],  upper[9],  upper[10], upper[11],
      upper[12], upper[13], upper[14], upper[15],
  };
#endif //__AVX2__
}

// # v32u8_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32u8_t
v32u8_not(const v32u8_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v32u8_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 32; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v32u8_mask(const v32u8_t a) {
#ifdef __AVX2__
  return __builtin_ia32_pmovmskb256(a);
#else
  u32 result = 0;
  v16u8_t lower = {a[0], a[1], a[2],  a[3],  a[4],  a[5],  a[6],  a[7],
                   a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
  v16u8_t upper = {a[16], a[17], a[18], a[19], a[20], a[21], a[22], a[23],
                   a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31]};

  result |= v16u8_mask(upper) << 16;
  result |= v16u8_mask(lower);

  return result;

#endif
}

//==================================v32i8=======================================

typedef i8 v32i8_t __attribute__((vector_size(32), aligned(32)));
typedef i8 v32i8_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v32i8_set1
// ## Return
// returns a `v32i8_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_set1(const i8 v) {
  return (v32i8_t){v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
                   v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
}

// # v32i8_set
// ## Return
// Returns a `v32i8_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_set(const i8 v00, const i8 v01, const i8 v02, const i8 v03, const i8 v04,
          const i8 v05, const i8 v06, const i8 v07, const i8 v08, const i8 v09,
          const i8 v10, const i8 v11, const i8 v12, const i8 v13, const i8 v14,
          const i8 v15, const i8 v16, const i8 v17, const i8 v18, const i8 v19,
          const i8 v20, const i8 v21, const i8 v22, const i8 v23, const i8 v24,
          const i8 v25, const i8 v26, const i8 v27, const i8 v28, const i8 v29,
          const i8 v30, const i8 v31) {
  return (v32i8_t){v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10,
                   v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21,
                   v22, v23, v24, v25, v26, v27, v28, v29, v30, v31};
}

//----------------------------------load/store----------------------------------

// # v32i8_load_aligned
// ## Description loads a `v32i8_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v32i8_t
v32i8_load_aligned(const v32i8_t *addr) {
  return *addr;
}

// # v32i8_load_unaligned
// ## Description loads a `v32i8_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v32i8_t
v32i8_load_unaligned(const v32i8_unaligned_t *addr) {
  return *addr;
}

// # v32i8_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v32i8_store_aligned(v32i8_t *addr,
                                                   const v32i8_t data) {
  *addr = data;
}

// # v32i8_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v32i8_store_unaligned(v32i8_unaligned_t *addr,
                                                     const v32i8_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v32i8_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_add(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_add((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_sub(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_add((v32u8_t)a, (v32u8_t)b);
  return a - b;
}

// # v32i8_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_mul(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_mul((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_div(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_div((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` as their unsigned
// counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_mod(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_mod((v32u8_t)a, (v32u8_t)b);
}

//==================================cmp=========================================
// # v32i8_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_eq(const v32i8_t a, const v32i8_t b) {
  return a == b;
}

// # v32i8_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_lt(const v32i8_t a, const v32i8_t b) {
  return a < b;
}

// # v32i8_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_gt(const v32i8_t a, const v32i8_t b) {
  return a > b;
}

// # v32i8_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_leq(const v32i8_t a, const v32i8_t b) {
  return a <= b;
}

// # v32i8_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_geq(const v32i8_t a, const v32i8_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v32i8_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_and(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_and((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_or(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_or((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_xor(const v32i8_t a, const v32i8_t b) {
  return (v32i8_t)v32u8_xor((v32u8_t)a, (v32u8_t)b);
}

// # v32i8_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 32; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_sl(const v32i8_t a, const int n) {
  return (v32i8_t)v32u8_sl((v32u8_t)a, n);
}

// # v32i8_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 32; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_sr(const v32i8_t a, const int n) {
  return (v32i8_t)v32u8_sr((v32u8_t)a, n);
}

// # v32i8_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` as its unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 32; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v32i8_t
v32i8_not(const v32i8_t a) {
  return (v32i8_t)v32u8_not((v32u8_t)a);
}

//----------------------------------mask----------------------------------------

// # v32i8_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 32; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v32i8_mask(const v32i8_t a) {
  return v32u8_mask((v32u8_t)a);
}

//==================================v16u16=======================================

typedef u16 v16u16_t __attribute__((vector_size(32), aligned(32)));
typedef u16 v16u16_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v16u16_set1
// ## Return
// returns a `v16u16_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_set1(const u16 v) {
  return (v16u16_t){
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
  };
}

// # v16u16_set
// ## Return
// Returns a `v16u16_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_set(const u16 v00, const u16 v01, const u16 v02, const u16 v03,
           const u16 v04, const u16 v05, const u16 v06, const u16 v07,
           const u16 v08, const u16 v09, const u16 v10, const u16 v11,
           const u16 v12, const u16 v13, const u16 v14, const u16 v15) {
  return (v16u16_t){v00, v01, v02, v03, v04, v05, v06, v07,
                    v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

// # v16u16_load_aligned
// ## Description loads a `v16u16_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16u16_t
v16u16_load_aligned(const v16u16_t *addr) {
  return *addr;
}

// # v16u16_load_unaligned
// ## Description loads a `v16u16_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16u16_t
v16u16_load_unaligned(const v16u16_unaligned_t *addr) {
  return *addr;
}

// # v16u16_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16u16_store_aligned(v16u16_t *addr,
                                                    const v16u16_t data) {
  *addr = data;
}

// # v16u16_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16u16_store_unaligned(v16u16_unaligned_t *addr,
                                                      const v16u16_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v16u16_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_add(const v16u16_t a, const v16u16_t b) {
  return a + b;
}

// # v16u16_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_sub(const v16u16_t a, const v16u16_t b) {
  return a - b;
}

// # v16u16_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_mul(const v16u16_t a, const v16u16_t b) {
  return a * b;
}

// # v16u16_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_div(const v16u16_t a, const v16u16_t b) {
  return a / b;
}

// # v16u16_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_mod(const v16u16_t a, const v16u16_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v16u16_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_eq(const v16u16_t a, const v16u16_t b) {
  return a == b;
}

// # v16u16_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_lt(const v16u16_t a, const v16u16_t b) {
  return a < b;
}

// # v16u16_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_gt(const v16u16_t a, const v16u16_t b) {
  return a > b;
}

// # v16u16_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_leq(const v16u16_t a, const v16u16_t b) {
  return a <= b;
}

// # v16u16_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_geq(const v16u16_t a, const v16u16_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v16u16_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_and(const v16u16_t a, const v16u16_t b) {
  return a & b;
}

// # v16u16_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_or(const v16u16_t a, const v16u16_t b) {
  return a | b;
}

// # v16u16_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_xor(const v16u16_t a, const v16u16_t b) {
  return a ^ b;
}

// # v16u16_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_sl(const v16u16_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_psllwi256(a, (int)n);
#else
  v8u16_t lower = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]};
  v8u16_t upper = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};

  lower = v8u16_shift_left(lower, n);
  upper = v8u16_shift_left(upper, n);

  return (v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      lower[4], lower[5], lower[6], lower[7],

      upper[0], upper[1], upper[2], upper[3],
      upper[4], upper[5], upper[6], upper[7],
  };
#endif //__AVX2__
}

// # v16u16_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_sr(const v16u16_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_psrlwi256(a, (int)n);
#else
  v8u16_t lower = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]};
  v8u16_t upper = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};

  lower = v8u16_shift_right(lower, n);
  upper = v8u16_shift_right(upper, n);

  return (v16u16_t){
      lower[0], lower[1], lower[2], lower[3],
      lower[4], lower[5], lower[6], lower[7],

      upper[0], upper[1], upper[2], upper[3],
      upper[4], upper[5], upper[6], upper[7],
  };
#endif //__AVX2__
}

// # v16u16_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16u16_t
v16u16_not(const v16u16_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v16u16_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 16; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v16u16_mask(const v16u16_t a) {
  u32 result = 0;
  v8u16_t lower = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]};
  v8u16_t upper = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};

  result |= v8u16_mask(upper) << 8;
  result |= v8u16_mask(lower);
  return result;
}

//==================================v16i16=======================================

typedef i16 v16i16_t __attribute__((vector_size(32), aligned(32)));
typedef i16 v16i16_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v16i16_set1
// ## Return
// returns a `v16i16_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_set1(const i16 v) {
  return (v16i16_t){
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v,
  };
}

// # v16i16_set
// ## Return
// Returns a `v16i16_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_set(const i16 v00, const i16 v01, const i16 v02, const i16 v03,
           const i16 v04, const i16 v05, const i16 v06, const i16 v07,
           const i16 v08, const i16 v09, const i16 v10, const i16 v11,
           const i16 v12, const i16 v13, const i16 v14, const i16 v15) {
  return (v16i16_t){v00, v01, v02, v03, v04, v05, v06, v07,
                    v08, v09, v10, v11, v12, v13, v14, v15};
}

//----------------------------------load/store----------------------------------

// # v16i16_load_aligned
// ## Description loads a `v16i16_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16i16_t
v16i16_load_aligned(const v16i16_t *addr) {
  return *addr;
}

// # v16i16_load_unaligned
// ## Description loads a `v16i16_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v16i16_t
v16i16_load_unaligned(const v16i16_unaligned_t *addr) {
  return *addr;
}

// # v16i16_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16i16_store_aligned(v16i16_t *addr,
                                                    const v16i16_t data) {
  *addr = data;
}

// # v16i16_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v16i16_store_unaligned(v16i16_unaligned_t *addr,
                                                      const v16i16_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v16i16_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_add(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_add((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_sub(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_sub((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_mul(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_mul((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_div(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_div((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` as their unsigned
// counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_mod(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_mod((v16u16_t)a, (v16u16_t)b);
}

//==================================cmp=========================================
// # v16i16_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_eq(const v16i16_t a, const v16i16_t b) {
  return a == b;
}

// # v16i16_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_lt(const v16i16_t a, const v16i16_t b) {
  return a < b;
}

// # v16i16_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_gt(const v16i16_t a, const v16i16_t b) {
  return a > b;
}

// # v16i16_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_leq(const v16i16_t a, const v16i16_t b) {
  return a <= b;
}

// # v16i16_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_geq(const v16i16_t a, const v16i16_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v16i16_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_and(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_and((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_or(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_or((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_xor(const v16i16_t a, const v16i16_t b) {
  return (v16i16_t)v16u16_xor((v16u16_t)a, (v16u16_t)b);
}

// # v16i16_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_sl(const v16i16_t a, const int n) {
  return (v16i16_t)v16u16_sl((v16u16_t)a, n);
}

// # v16i16_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 16; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_sr(const v16i16_t a, const int n) {
  return (v16i16_t)v16u16_sr((v16u16_t)a, n);
}

// # v16i16_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 16; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v16i16_t
v16i16_not(const v16i16_t a) {
  return (v16i16_t)v16u16_not(a);
}

//----------------------------------mask----------------------------------------

// # v16i16_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 16; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v16i16_mask(const v16i16_t a) {
  return v16u16_mask((v16u16_t)a);
}

//==================================v8u32=======================================
typedef u32 v8u32_t __attribute__((vector_size(32), aligned(32)));
typedef u32 v8u32_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v8u32_set1
// ## Return
// returns a `v8u32_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_set1(const u32 v) {
  return (v8u32_t){v, v, v, v, v, v, v, v};
}

// # v8u32_set
// ## Return
// Returns a `v8u32_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_set(const u32 v00, const u32 v01, const u32 v02, const u32 v03,
          const u32 v04, const u32 v05, const u32 v06, const u32 v07) {
  return (v8u32_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

// # v8u32_load_aligned
// ## Description loads a `v8u32_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8u32_t
v8u32_load_aligned(const v8u32_t *addr) {
  return *addr;
}

// # v8u32_load_unaligned
// ## Description loads a `v8u32_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8u32_t
v8u32_load_unaligned(const v8u32_unaligned_t *addr) {
  return *addr;
}

// # v8u32_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8u32_store_aligned(v8u32_t *addr,
                                                   const v8u32_t data) {
  *addr = data;
}

// # v8u32_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8u32_store_unaligned(v8u32_unaligned_t *addr,
                                                     const v8u32_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v8u32_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_add(const v8u32_t a, const v8u32_t b) {
  return a + b;
}

// # v8u32_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_sub(const v8u32_t a, const v8u32_t b) {
  return a - b;
}

// # v8u32_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_mul(const v8u32_t a, const v8u32_t b) {
  return a * b;
}

// # v8u32_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_div(const v8u32_t a, const v8u32_t b) {
  return a / b;
}

// # v8u32_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_mod(const v8u32_t a, const v8u32_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v8u32_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_eq(const v8u32_t a, const v8u32_t b) {
  return a == b;
}

// # v8u32_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_lt(const v8u32_t a, const v8u32_t b) {
  return a < b;
}

// # v8u32_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_gt(const v8u32_t a, const v8u32_t b) {
  return a > b;
}

// # v8u32_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_leq(const v8u32_t a, const v8u32_t b) {
  return a <= b;
}

// # v8u32_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_geq(const v8u32_t a, const v8u32_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v8u32_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_and(const v8u32_t a, const v8u32_t b) {
  return a & b;
}

// # v8u32_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_or(const v8u32_t a, const v8u32_t b) {
  return a | b;
}

// # v8u32_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_xor(const v8u32_t a, const v8u32_t b) {
  return a ^ b;
}

// # v8u32_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_sl(const v8u32_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_pslldi256(a, (int)n);
#else
  v4u32_t lower = {a[0], a[1], a[2], a[3]};
  v4u32_t upper = {a[4], a[5], a[6], a[7]};

  lower = v4u32_shift_left(lower, n);
  upper = v4u32_shift_left(upper, n);

  return (v8u32_t){
      lower[0], lower[1], lower[2], lower[3],
      upper[0], upper[1], upper[2], upper[3],
  };
#endif //__AVX2__
}

// # v8u32_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_sr(const v8u32_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_psrldi256(a, (int)n);
#else
  v4u32_t lower = {a[0], a[1], a[2], a[3]};
  v4u32_t upper = {a[4], a[5], a[6], a[7]};

  lower = v4u32_shift_right(lower, n);
  upper = v4u32_shift_right(upper, n);

  return (v8u32_t){
      lower[0], lower[1], lower[2], lower[3],
      upper[0], upper[1], upper[2], upper[3],
  };
#endif //__AVX2__
}

// # v8u32_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8u32_t
v8u32_not(const v8u32_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v8u32_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 8; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v8u32_mask(const v8u32_t a) {
#ifdef __AVX__
  return __builtin_ia32_movmskps256(a);
#else
  u32 result = 0;
  v4u32_t lower = {a[0], a[1], a[2], a[3]};
  v4u32_t upper = {a[4], a[5], a[6], a[7]};
  result |= v4u32_mask(upper) << 4;
  result |= v4u32_mask(lower);
  return result;
#endif // __AVX__
}

//==================================v8i32=======================================

typedef i32 v8i32_t __attribute__((vector_size(32), aligned(32)));
typedef i32 v8i32_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v8i32_set1
// ## Return
// returns a `v8i32_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_set1(const i32 v) {
  return (v8i32_t){v, v, v, v, v, v, v, v};
}

// # v8i32_set
// ## Return
// Returns a `v8i32_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_set(const i32 v00, const i32 v01, const i32 v02, const i32 v03,
          const i32 v04, const i32 v05, const i32 v06, const i32 v07) {
  return (v8i32_t){v00, v01, v02, v03, v04, v05, v06, v07};
}

//----------------------------------load/store----------------------------------

// # v8i32_load_aligned
// ## Description loads a `v8i32_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8i32_t
v8i32_load_aligned(const v8i32_t *addr) {
  return *addr;
}

// # v8i32_load_unaligned
// ## Description loads a `v8i32_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v8i32_t
v8i32_load_unaligned(const v8i32_unaligned_t *addr) {
  return *addr;
}

// # v8i32_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8i32_store_aligned(v8i32_t *addr,
                                                   const v8i32_t data) {
  *addr = data;
}

// # v8i32_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v8i32_store_unaligned(v8i32_unaligned_t *addr,
                                                     const v8i32_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v8i32_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_add(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_add((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_sub(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_sub((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_mul(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_mul((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_div(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_div((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_mod(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_mod((v8u32_t)a, (v8u32_t)b);
}

//==================================cmp=========================================
// # v8i32_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_eq(const v8i32_t a, const v8i32_t b) {
  return a == b;
}

// # v8i32_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_lt(const v8i32_t a, const v8i32_t b) {
  return a < b;
}

// # v8i32_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_gt(const v8i32_t a, const v8i32_t b) {
  return a > b;
}

// # v8i32_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_leq(const v8i32_t a, const v8i32_t b) {
  return a <= b;
}

// # v8i32_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_geq(const v8i32_t a, const v8i32_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v8i32_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_and(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_and((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_or(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_or((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_xor(const v8i32_t a, const v8i32_t b) {
  return (v8i32_t)v8u32_xor((v8u32_t)a, (v8u32_t)b);
}

// # v8i32_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_sl(const v8i32_t a, const int n) {
  return (v8i32_t)v8u32_sl((v8u32_t)a, n);
}

// # v8i32_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 8; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_sr(const v8i32_t a, const int n) {
  return (v8i32_t)v8u32_sr((v8u32_t)a, n);
}

// # v8i32_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 8; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v8i32_t
v8i32_not(const v8i32_t a) {
  return (v8i32_t)v8u32_not((v8u32_t)a);
}

//----------------------------------mask----------------------------------------

// # v8i32_mask
// ## Description return a `i32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// i32 mask = 0;
// for (int i = 0; i < 8; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline i32
v8i32_mask(const v8i32_t a) {
  return v8u32_mask((v8u32_t)a);
}

//==================================v4u64=======================================

typedef u64 v4u64_t __attribute__((vector_size(32), aligned(32)));
typedef u64 v4u64_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v4u64_set1
// ## Return
// returns a `v4u64_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_set1(const uint64_t v) {
  return (v4u64_t){v, v, v, v};
}

// # v4u64_set
// ## Return
// Returns a `v4u64_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_set(const uint64_t v00, const uint64_t v01, const uint64_t v02,
          const uint64_t v03) {
  return (v4u64_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

// # v4u64_load_aligned
// ## Description loads a `v4u64_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4u64_t
v4u64_load_aligned(const v4u64_t *addr) {
  return *addr;
}

// # v4u64_load_unaligned
// ## Description loads a `v4u64_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4u64_t
v4u64_load_unaligned(const v4u64_unaligned_t *addr) {
  return *addr;
}

// # v4u64_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4u64_store_aligned(v4u64_t *addr,
                                                   const v4u64_t data) {
  *addr = data;
}

// # v4u64_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4u64_store_unaligned(v4u64_unaligned_t *addr,
                                                     const v4u64_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v4u64_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_add(const v4u64_t a, const v4u64_t b) {
  return a + b;
}

// # v4u64_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_sub(const v4u64_t a, const v4u64_t b) {
  return a - b;
}

// # v4u64_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_mul(const v4u64_t a, const v4u64_t b) {
  return a * b;
}

// # v4u64_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_div(const v4u64_t a, const v4u64_t b) {
  return a / b;
}

// # v4u64_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_mod(const v4u64_t a, const v4u64_t b) {
  return a % b;
}

//==================================cmp=========================================
// # v4u64_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_eq(const v4u64_t a, const v4u64_t b) {
  return a == b;
}

// # v4u64_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_lt(const v4u64_t a, const v4u64_t b) {
  return a < b;
}

// # v4u64_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_gt(const v4u64_t a, const v4u64_t b) {
  return a > b;
}

// # v4u64_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_leq(const v4u64_t a, const v4u64_t b) {
  return a <= b;
}

// # v4u64_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_geq(const v4u64_t a, const v4u64_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v4u64_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_and(const v4u64_t a, const v4u64_t b) {
  return a & b;
}

// # v4u64_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_or(const v4u64_t a, const v4u64_t b) {
  return a | b;
}

// # v4u64_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_xor(const v4u64_t a, const v4u64_t b) {
  return a ^ b;
}

// # v4u64_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_sl(const v4u64_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_psllqi256(a, (int)n);
#else
  v2u64_t lower = {a[0], a[1]};
  v2u64_t upper = {a[2], a[3]};

  lower = v2u64_shift_left(lower, n);
  upper = v2u64_shift_left(upper, n);

  return (v4u64_t){lower[0], lower[1], upper[0], upper[1]};
#endif //__AVX2__
}

// # v4u64_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_sr(const v4u64_t a, const int n) {
#ifdef __AVX2__
  return __builtin_ia32_psrlqi256(a, (int)n);
#else
  v2u64_t lower = {a[0], a[1]};
  v2u64_t upper = {a[2], a[3]};

  lower = v2u64_shift_right(lower, n);
  upper = v2u64_shift_right(upper, n);

  return (v4u64_t){lower[0], lower[1], upper[0], upper[1]};
#endif //__AVX2__
}

// # v4u64_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4u64_t
v4u64_not(const v4u64_t a) {
  return ~a;
}

//----------------------------------mask----------------------------------------

// # v4u64_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 4; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v4u64_mask(const v4u64_t a) {
#ifdef __AVX__
  return __builtin_ia32_movmskpd256(a);
#else
  u32 result = 0;
  v2u64_t lower = {a[0], a[1]};
  v2u64_t upper = {a[2], a[3]};

  result |= v2u64_mask(upper) << 2;
  result |= v2u64_mask(lower);
  return result;

#endif // __AVX__
}

//==================================v4i64=======================================

typedef i64 v4i64_t __attribute__((vector_size(32), aligned(32)));
typedef i64 v4i64_unaligned_t __attribute__((vector_size(32), aligned(1)));

//----------------------------------set-----------------------------------------

// # v4i64_set1
// ## Return
// returns a `v4i64_t` with all members set to `v`
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_set1(const i64 v) {
  return (v4i64_t){v, v, v, v};
}

// # v4i64_set
// ## Return
// Returns a `v4i64_t` with each member set to the corresponding argument
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_set(const i64 v00, const i64 v01, const i64 v02, const i64 v03) {
  return (v4i64_t){v00, v01, v02, v03};
}

//----------------------------------load/store----------------------------------

// # v4i64_load_aligned
// ## Description loads a `v4i64_t` from the address `addr`. `addr` shall be
// `16` byte aligned
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4i64_t
v4i64_load_aligned(const v4i64_t *addr) {
  return *addr;
}

// # v4i64_load_unaligned
// ## Description loads a `v4i64_t` from the address `addr`
//
// ## Return
// Returns the vector loaded from `addr`
ATTR_NONNULL(1)
ATTR_INLINE ATTR_NODISCARD ATTR_PURE static inline v4i64_t
v4i64_load_unaligned(const v4i64_unaligned_t *addr) {
  return *addr;
}

// # v4i64_store_aligned
// ## Description
// Store `data` at the address `addr`. `addr` shall be `16` byte aligned
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4i64_store_aligned(v4i64_t *addr,
                                                   const v4i64_t data) {
  *addr = data;
}

// # v4i64_store_unaligned
// ## Description
// Store `data` at the address `addr`.
ATTR_NONNULL(1)
ATTR_INLINE static inline void v4i64_store_unaligned(v4i64_unaligned_t *addr,
                                                     const v4i64_t data) {
  *addr = data;
}

//==================================arithmetic===================================
// # v4i64_add
// ## Description
// add each member of `a` to the according member of `b` and return the
// resulting vector.
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] + b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_add(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_add((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_sub
// ## Description
// subtract each member of `b` from the according member of `a` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] - b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_sub(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_sub((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_mul
// ## Description
// multiply each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] * b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_mul(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_mul((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_div
// ## Description
// divide each member of `a` by the according member of `b` and return the
// resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] / b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_div(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_div((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_mod
// ## Description
// take the modulus of each member of `a` and the according member of `b` and
// return the resulting vector. Treating `a` and `b` as their unsigned
// counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] % b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_mod(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_mod((v4u64_t)a, (v4u64_t)b);
}

//==================================cmp=========================================
// # v4i64_eq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] == b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] == b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_eq(const v4i64_t a, const v4i64_t b) {
  return a == b;
}

// # v4i64_lt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] < b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] < b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_lt(const v4i64_t a, const v4i64_t b) {
  return a < b;
}

// # v4i64_gt
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] > b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] > b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_gt(const v4i64_t a, const v4i64_t b) {
  return a > b;
}

// # v4i64_leq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] <= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] <= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_leq(const v4i64_t a, const v4i64_t b) {
  return a <= b;
}

// # v4i64_geq
// ## Description
// compare each member of `a` to the according member of `b` and return the
// resulting vector. If `a[0] >= b[0]` than `result[0] == 0xFF` otherwise
// `result[0] == 0`
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] >= b[i] ? 0xFF : 0;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_geq(const v4i64_t a, const v4i64_t b) {
  return a >= b;
}

//==================================bitwise======================================
// # v4i64_and
// ## Description
// bitwise and each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] & b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_and(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_and((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_or
// ## Description
// bitwise or each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] | b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_or(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_or((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_xor
// ## Description
// bitwise xor each member of `a` with the according member of `b` and return
// the resulting vector. Treating `a` and `b` as their unsigned counterparts
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = a[i] ^ b[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_xor(const v4i64_t a, const v4i64_t b) {
  return (v4i64_t)v4u64_xor((v4u64_t)a, (v4u64_t)b);
}

// # v4i64_sl
// ## Description
// shift each member of `a` left by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] << n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_sl(const v4i64_t a, const int n) {
  return (v4i64_t)v4u64_sl((v4u64_t)a, n);
}

// # v4i64_sr
// ## Description
// shift each member of `a` right by the immediate value `n` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int j = 0; j < 4; ++j) {
//   result[j] = a[j] >> n;
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_sr(const v4i64_t a, const int n) {
  return (v4i64_t)v4u64_sr((v4u64_t)a, n);
}

// # v4i64_not
// ## Description
// bitwise not each member of `a` and return the
// resulting vector. Treating `a` as its unsigned counterpart
//
// A pseudo implementation of this function might look like this
// ```c
// for (int i = 0; i < 4; ++i) {
//   result[i] = ~a[i];
// }
// return result;
//```
//
// ## Return
// the resulting vector
ATTR_NODISCARD ATTR_CONST ATTR_INLINE static inline v4i64_t
v4i64_not(const v4i64_t a) {
  return (v4i64_t)v4u64_not((v4u64_t)a);
}

//----------------------------------mask----------------------------------------

// # v4i64_mask
// ## Description return a `u32` with each bit set according to the most
// significant bit of each member of `a`
//
// A pseudo implementation of this function might look like this:
// ```c
// u32 mask = 0;
// for (int i = 0; i < 4; ++i) {
//   if (a[i] >> 7) {
//     mask |= 1 << i;
//   }
// }
// return mask;
// ```
//
// ## Return
// the resulting mask
ATTR_NODISCARD ATTR_INLINE ATTR_CONST static inline u32
v4i64_mask(const v4i64_t a) {
  return v4u64_mask((v4u64_t)a);
}

#endif // MERLIN_SIMD_H
