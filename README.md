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
