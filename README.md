![alt merlin-logo](./merlin.png)
# merlin
Making parallelism accessible.

## NOTE: WIP
This is a work in progress and thinks might change, or documentation / examples
might not be up to date.

## Concepts
### SIMD Vectors
The vector types and functions are prefixed with `mrln_vXXYYY_` where `X` is
the number of elements and `YYY` the type, currently implemented are:  `128` and
`256` bit vectors of `(u)int8_t`, `(u)int16_t`, `(u)int32_t`, `(u)int64_t`. With
basic functions for loading/storing, setting, arithmetics and comparisons and for
the unsigned types all bitwise operations (see the documentation for more).

#### Example

``` c
#include <merlin/simd.h>

#include <stdio.h>

int main(void) {
  const mrln_v4u32_t b = mrln_v4u32_set1(35);
  const mrln_v4u32_t a = mrln_v4u32_set1(34);

  mrln_v4u32_t c = mrln_v4u32_add(a, b);

  uint32_t array[4];

  mrln_v4u32_store_unaligned((void *)array, c);

  for (int i = 0; i < 4; ++i) {
    printf("%u\n", array[i]);
  }

  return 0;
}
```

#### Motivation
Why use this and not the classic intel intrinsics? In short intel intrinsics
have fucked names (which at least for me are hard to remember) and might not be
available on all platforms. This library aims to provide a reasonable subset of
the intel intrinsics with fallsbacks in case the instructions are not available.
And built on these for better implementations of standard datastructures and
algorithms (see `merlin_str8_t`).

### Allocator
Merlin doesn't use a specific allocation scheme by default but lets the user
decide how things are allocated. This is done through the `mrln_aloctr_t` type
and the `mrln_alloc` function. The `mrln_alloc` function acts either as a `malloc`,
a `realloc`, or a `free` depending on its parameters. In addition it takes in a
alignment parameter. If you don't want to write your own allocator you can get
the global allocator through `mrln_aloctr_global`.

#### Example
```c
#include <merlin/aloctr.h>
#include <stddef.h> // for NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  // get the global allocator

  mrln_aloctr_t *a = mrln_aloctr_global();

  // now we can use it to allocate an array

  int *array = NULL;
  intptr_t capacity = 0;

  // Explanation:
  // 1. parameter is the allocator we want to use
  // 2. paramter is a pointer to the current buffer, and will be set to
  //    the new buffer want to allocate
  // 3. paramter is the current capacity (in bytes) of the buffer, and will
  //    be set to the new capacity of the buffer
  // 4. paramter is the alignment of the buffer
  // 5. paramter is the new capcity of the buffer
  // -  return value is an error value
  int err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int),
                       10 * sizeof(*array));

  // check the error value
  if (err) {
    // in most cases the error is an ENOMEM
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  // now we can use our array
  for (int i = 0; i < 10; ++i) {
    array[i] = i;
  }

  // OH NO there is no space left in the array but I really need to put this
  // `69` in there
  //
  // no problem we can use the same function to reallocate the array

  err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int),
                   11 * sizeof(*array));

  // this reallocates the array, please keep in mind that capacity needs to be
  // the capacity in bytes of the current buffer, a mrln_aloctr is not required
  // to keep track of the buffer sizes

  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  array[10] = 69;

  // we use the same function again to deallocate the buffer, by reallocating it
  // to size zero, this will set `capacity` to `0` and `array` to NULL
  err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int), 0);

  if (err) {
    // checking the error value of the global allocator is not necessary, this
    // might not be true for custom, or special allocators
  }

  return 0;
}
```

#### Motivation
Custom allocators are very cool and too many libs out their missout on
supporting them. Not us though!


### str8
The `mrln_str8_t` type is a heap allocated string with functions for string
handling. Most of these functions operate on the `mrln_str8view_t` type which
is a constant pointer into a string and a length. Which can also be built from
a classic null terminated string.

#### Example
```c
#include <merlin/str8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char cstr[] = "Oh  no  we  used  2  spaces.\n";

  // get the global allocator used to allocate the strings
  mrln_aloctr_t *a = mrln_aloctr_global();

  int err;

  // str8 has to be zeroed
  mrln_str8_t str8 = {};

  // allocate string and fill it with cstr
  err = mrln_str8(&str8, cstr, a);

  // check for errors
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  // replace "  " with " "
  mrln_str8view_t target = mrln_str8view("  ");
  mrln_str8view_t replace = mrln_str8view(" ");
  err = mrln_str8_replace(&str8, &target, &replace, a);

  // check for errors
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  printf("%.*s\n", (int)str8.length, str8.buffer);

  // deallocate str8
  mrln_str8_destroy(&str8, a);

  return 0;
}
```

#### Motivation
String handling in C sucks, and I wanted to built something with the simd
types. And searching in a string is a pretty good application for that. And
through the `merlin_str8_view_t` the functions are mostly compatible with
classic C strings and C strings can be built from a `merlin_str8_t` or a
`merlin_str8_t` can be built from a C string.
