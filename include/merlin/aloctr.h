#ifndef MRLN_ALOCTR_H
#define MRLN_ALOCTR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <merlin/core.h>

// # aloctr_t
// ## Descritpion
// Opaque type for an allocator
//
// ## DIY
// If you want to build your own allocator you have to make sure it adheres to
// the following:
// - the first member is a `aloctr_allocate_f` function pointer
// - the second memeber is a `aloctr_reallocate_f` function pointer
// - the third member is a `aloctr_deallocate_f` function pointer
//
// the rest of the fields can be customized to your desire and the allocator
// itself will be passed to the defined function pointers, use this as a method
// of passing context into the allocation scheme
//
typedef void aloctr_t;

// #aloctr_allocate_f
// ## Description
// Type of the allocate function used to allocate a region of `size` bytes using
// the allocator `a`
//
// ### Parameter
// - `a`: a non null pointer to a valid allocator
// - `size`: number of bytes to be allocated, if `size` is `0` this function
//    shall return `NULL`
//
// ## Return
// A pointer to the allocated region on success, and `NULL` on error or `size ==
// 0`
//
// ## Error
// In case of an error `NULL` shall be returned and `errno` set to indicate the
// error
typedef void *(aloctr_allocate_f)(aloctr_t *a, u64 size);

// #aloctr_reallocate_f
// ## Description
// Reallocate the region pointed to by `base` to `size` bytes using the
// allocator `a`
//
// ### Parameter
// - `a`: a non null pointer to a valid allocator
// - `base`: a pointer to the region, which is to be reallocated, `base` might
//    be `NULL` in which case this function acts like `aloctr_allocate_f`
// - `size`: the number of bytes the reallocated region will have, if `size` is
//   `0` this function acts like `aloctr_deallocate_f` on `base` and shall
//   return `NULL`
//
// ## Return
// A pointer to the reallocated region, may be the same as `base`
//
// ## Error
// In case of an error `NULL` shall be returned, `errno` set to indicate the
// error and the region pointed to by base shall not be modified
typedef void *(aloctr_reallocate_f)(aloctr_t *a, void *base, u64 size);

// #aloctr_deallocate_f
// ## Description
// Deallocate the regoin pointed to by `p` using the allocator `a`
typedef void(aloctr_deallocate_f)(aloctr_t *a, void *p);

struct aloctr_layout_t {
  aloctr_allocate_f *allocate;
  aloctr_reallocate_f *reallocate;
  aloctr_deallocate_f *deallocate;
};

// ## Description
// allocate `size` bytes using `a`
//
// ## Return
// returns pointer to the allocated region on success and `NULL` on error
//
// ## Error
// in case of an error, mostly `ENOMEM` `NULL` is returned and `errno` is set to
// indicate the error
ATTR_INLINE ATTR_NODISCARD ATTR_ALLOCATE ATTR_NONNULL(1)
    ATTR_ALLOCATE_SIZE(2) static inline void *allocate(aloctr_t *a, u64 size) {
  return ((struct aloctr_layout_t *)a)->allocate(a, size);
}

// ## Description
// reallocate region pointed to by `base` to `size` bytes using `a`
//
// ## Return
// returns pointer to the allocated region on success and `NULL` on error
//
// ## Error
// in case of an error, mostly `ENOMEM` `NULL` is returned and `errno` is set to
// indicate the error, if an error occurs base will not be altered
ATTR_INLINE ATTR_NODISCARD ATTR_ALLOCATE ATTR_NONNULL(1)
    ATTR_ALLOCATE_SIZE(3) static inline void *reallocate(aloctr_t *a,
                                                         void *base, u64 size) {
  return ((struct aloctr_layout_t *)a)->reallocate(a, base, size);
}

// ## Description
// deallocate region pointed to by `base` using `a`
ATTR_INLINE ATTR_NONNULL(1) static inline void deallocate(aloctr_t *a,
                                                          void *base) {
  ((struct aloctr_layout_t *)a)->deallocate(a, base);
}

// ## Description
// returns a reference to the global allocator, the global allocator shall be
// thread safe
//
// ## Return
// a reference to the global allocator
aloctr_t *aloctr_global(void);

#ifdef __cplusplus
}
#endif

#endif // MRLN_ALOCTR_H
