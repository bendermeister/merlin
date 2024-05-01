![alt merlin-logo](./merlin.png)
# merlin
Making parallelism accessible.

## NOTE: WIP
This is a work in progress and thinks might change, or documentation / examples might not be up to date.

## Concepts
### SIMD Vectors
The vector types and functions are prefixed with `merlin_vXXYYY_` where `X` is
the number of elements and `YYY` the type, currently implemented are:  `128` and
`256` bit vectors of `(u)int8_t`, `(u)int16_t`, `(u)int32_t`, `(u)int64_t`. With
basic functions for loading/storing, setting, arithmetics and comparisons and for
the unsigned types all bitwise operations (see the documentation for more).

#### Example

``` c
#include <merlin/simd.h>

#include <stdio.h>

int main(void) {
  const merlin_v4u32_t a = merlin_v4u32_set1(34);
  const merlin_v4u32_t b = merlin_v4u32_set1(35);

  merlin_v4u32_t c = merlin_v4u32_add(a, b);

  uint32_t array[4];

  merlin_v4u32_store_unaligned((void *)array, c);

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
The `merlin_str8_t` type is a heap allocated string with functions for string
handling. Most of these functions operate on the `merlin_str8_view_t` type which
is a constant pointer into a string and a length. Which can also be built from a
classic null terminated string.

#### Example

#### Motivation
String handling in C sucks, and I wanted to built something with the simd types. And searching in a string is a pretty good application for that. And through the `merlin_str8_view_t` the functions are mostly compatible with classic C strings and C strings can be built from a `merlin_str8_t` or a `merlin_str8_t` can be built from a C string.
