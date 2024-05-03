#ifndef MRLN_ALOCTR_H
#define MRLN_ALOCTR_H

#include <stdint.h>

typedef struct mrln_chunk_t mrln_chunk_t;
struct mrln_chunk_t {
  void *chunk;
  intptr_t size;
};

// opaque type for a general allocator. a general allocator is everything which
// is able to be cast to a `mrln_aloctr_layout_t` where the alloc function can
// be called
typedef void mrln_aloctr_t;

// The aloctr function. It takes three arguments and return an error code
// - `mrln_aloctr_t *a` the aloctr itself, containing this function pointer and
//    if needed additional information
// - `void **dest`, destination of the allocated memory. If `dest == NULL` then
//   `*dest_size` must be `0` and the call can be thought of as a malloc,
//   otherwise `*dest_size` must be the size of the allocated chunk and the
//   chunk must have been allocated by the calling allocator, and the function
//   can be thought of as a `realloc`
// - `intptr_t *dest_size` the size of the current chunk. see `dest`
// - `new_size` the new size of the chunk, if it is non zero see the above,
//   otherwise if it **is** zero, this function can be thought of as a free
typedef int(mrln_aloctr_f)(mrln_aloctr_t *a, void **dest, intptr_t *dest_size,
                           const intptr_t align, const intptr_t new_size);

typedef struct mrln_aloctr_layout_t mrln_aloctr_layout_t;
struct mrln_aloctr_layout_t {
  mrln_aloctr_f *alloc;
};

// call an allocator to allocate or deallocate memory according to size
__attribute__((__always_inline__, __nonnull__(1, 2, 3),
               __warn_unused_result__)) static inline int
mrln_alloc(mrln_aloctr_t *a, void **chunk, intptr_t *chunk_size,
           const intptr_t align, const intptr_t size) {
  return ((mrln_aloctr_layout_t *)a)->alloc(a, chunk, chunk_size, align, size);
}

// get a the global aloctr this should be used for everything passed between
// threads. Currently this is just a wrapper around the systems malloc
mrln_aloctr_t *mrln_aloctr_global(void);

#endif // MRLN_ALOCTR_H
