#ifndef SRC_UTIL_H
#define SRC_UTIL_H

#define LIKELY(A) __builtin_expect((A), 1)
#define UNLIKELY(A) __builtin_expect((A), 0)

#ifdef MRLN_ASSUME_TRAPS
#define ASSUME(A)                                                              \
  if (!(A)) {                                                                  \
    __builtin_trap();                                                          \
  }
#else
#define ASSUME(A) __builtin_assume((A))
#endif // MRLN_ASSUME_TRAPS

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uintptr_t usize;
typedef intptr_t isize;

#define UNUSED(A) (void)A

#define MIN(A, B)                                                              \
  ({                                                                           \
    __auto_type MIN_a_ = A;                                                    \
    __auto_type MIN_b_ = B;                                                    \
    MIN_a_ < MIN_b_ ? MIN_a_ : MIN_b_;                                         \
  })

#define MAX(A, B)                                                              \
  ({                                                                           \
    __auto_type MAX_a_ = A;                                                    \
    __auto_type MAX_b_ = B;                                                    \
    MAX_a_ > MAX_b_ ? MAX_a_ : MAX_b_;                                         \
  })

#define let const __auto_type
#define var __auto_type

#define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define CONST_FUNC __attribute__((const, warn_unused_result))
#define NODISCARD __attribute__((warn_unused_result))
#define AINLINE __attribute__((always_inline))

#endif // SRC_UTIL_H
