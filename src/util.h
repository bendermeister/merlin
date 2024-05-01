#ifndef SRC_UTIL_H
#define SRC_UTIL_H

#define LIKELY(A) __builtin_expect((A), 1)
#define UNLIKELY(A) __builtin_expect((A), 0)
#define ASSUME(A) __builtin_assume((A))

#endif // SRC_UTIL_H
