#ifndef SRC_UTIL_H
#define SRC_UTIL_H

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
