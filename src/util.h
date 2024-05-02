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

#endif // SRC_UTIL_H
