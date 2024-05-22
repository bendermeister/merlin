#ifndef MRLN_CORE_H
#define MRLN_CORE_H

#ifdef __cplusplus__
extern "C" {
#endif // __cplusplus__

#include <stdarg.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

//=================================ASSUME======================================
#ifdef ASSUME_TRAPS
#if defined(__GNUC__)
#define ASSUME(...)                                                            \
  if (!(__VA_ARGS__))                                                          \
  __builtin_trap()
#else
#define ASSUME(...)                                                            \
  while (!(__VA_ARGS__)) {                                                     \
    *NULL = 0;                                                                 \
  }
#endif // defined(__clang__)

#else

#if defined(__clang__)
#define ASSUME(...) __builtin_assume(__VA_ARGS__)
#elif defined(__GNUC__)
#define ASSUME(...)                                                            \
  if (!(__VA_ARGS__))                                                          \
  __builtin_unreachable()
#else
#define ASUMME(...)

#endif // defined(__clang__)

#endif

//=================================RESTRICT====================================
#ifdef __cplusplus__
#defien restrict __restrict__
#endif // __cplusplus__

//=================================LIKELY/UNLIKELY=============================
#ifdef __GNUC__
#define LIKELY(...) __builtin_expect((__VA_ARGS__), 1)
#define UNLIKELY(...) __builtin_expect((__VA_ARGS__), 0)
#else
#define LIKELY(...)
#define UNLIKELY(...)
#endif

//=================================ATTRIBUTE=WRAPPER===========================
#ifdef __GNUC__

#define ATTR_NODISCARD __attribute__((__warn_unused_result__))
#define ATTR_CONST __attribute__((__const__))
#define ATTR_PURE __attribute__((__pure__))
#define ATTR_NONNULL(...) __attribute__((__nonnull__(__VA_ARGS__)))
#define ATTR_ALIGNED(A) __attribute__((__aligned__(A)))
#define ATTR_INLINE __attribute__((__always_inline__))
#define ATTR_ALLOCATE __attribute__((__malloc__))
#define ATTR_ALLOCATE_SIZE(...) __attribute__((__alloc_size__(__VA_ARGS__)))

#else

// TODO: what are the msvc equivalents?

#define ATTR_NODISCARD
#define ATTR_CONST
#define ATTR_PURE
#define ATTR_NONNULL(...)
#define ATTR_ALIGNED(A)

#endif // __GNUC__

#ifdef __cplusplus__
}
#endif // __cplusplus__

#endif // MRLN_CORE_H
