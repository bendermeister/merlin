#ifndef SRC_UTIL_H
#define SRC_UTIL_H

#define LIKELY(A) __builtin_expect((A), 1)
#define UNLIKELY(A) __builtin_expect((A), 0)
#define ASSUME(A) __builtin_assume((A))

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

#endif // SRC_UTIL_H
