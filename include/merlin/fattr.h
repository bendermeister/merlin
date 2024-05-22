#ifndef MRLN_FATTR_H
#define MRLN_FATTR_H

#include <stdarg.h>

#ifdef __GNUC__
#define FATTR_CONST __attribute__((const))
#define FATTR_PURE __attribute__((pure))
#define FATTR_NODISCARD __attribute__((warn_unused_result))
#define FATTR_NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define FATTR_PROG_SETUP __attribute__((constructor))
#define FATTR_PROG_CLEANUP __attribute__((destructor))
#else
#define FATTR_CONST
#define FATTR_PURE
#define FATTR_NODISCARD
#define FATTR_NONNULL(...)
#define FATTR_PROG_SETUP
#define FATTR_PROG_CLEANUP
#endif // __GNUC__

#endif // MRLN_FATTR_H
