#include <broken/unit.h>
#include <merlin/simd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static test_t TEST_v16u8(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 16
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFF

  typedef u8 T;
  typedef u8 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  TT __attribute__((aligned(1)))
  b[NUM] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v16u8_t x, y, z;

  // test set
  {
    x = v16u8_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v16u8_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v16u8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v16u8_load_unaligned(b);
    x = v16u8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v16u8_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v16u8_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v16u8_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v16u8_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v16u8_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v16u8_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v16u8_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v16u8_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v16u8_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v16u8_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v16u8_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v16u8_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v16u8_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v16u8_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v16u8_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v16u8_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v16u8_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v16u8_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v16u8_eq(x, y);
    mask = v16u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v16u8_lt(x, y);
    mask = v16u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v16u8_gt(x, y);
    mask = v16u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v16u8_leq(x, y);
    mask = v16u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v16u8_geq(x, y);
    mask = v16u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v16i8(void) {

#undef NUM
#define NUM 16
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFF

  test_t test = TEST_MAKE();

  typedef u8 T;
  typedef i8 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  TT __attribute__((aligned(1)))
  b[NUM] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v16i8_t x, y, z;

  // test set
  {
    x = v16i8_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v16i8_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v16i8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v16i8_load_unaligned(b);
    x = v16i8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v16i8_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v16i8_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v16i8_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v16i8_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v16i8_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v16i8_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v16i8_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v16i8_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v16i8_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v16i8_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v16i8_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v16i8_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v16i8_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v16i8_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v16i8_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v16i8_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v16i8_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v16i8_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v16i8_eq(x, y);
    mask = v16i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v16i8_lt(x, y);
    mask = v16i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v16i8_gt(x, y);
    mask = v16i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v16i8_leq(x, y);
    mask = v16i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v16i8_geq(x, y);
    mask = v16i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v8u16(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 8
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFF

  typedef u16 T;
  typedef u16 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15, 14, 13, 12, 11, 10, 9};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v8u16_t x, y, z;

  // test set
  {
    x = v8u16_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v8u16_set(0, 1, 2, 3, 4, 5, 6, 7);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v8u16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v8u16_load_unaligned(b);
    x = v8u16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v8u16_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v8u16_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v8u16_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v8u16_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v8u16_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v8u16_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v8u16_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v8u16_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v8u16_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v8u16_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v8u16_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v8u16_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v8u16_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v8u16_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v8u16_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v8u16_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v8u16_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v8u16_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v8u16_eq(x, y);
    mask = v8u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v8u16_lt(x, y);
    mask = v8u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v8u16_gt(x, y);
    mask = v8u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v8u16_leq(x, y);
    mask = v8u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v8u16_geq(x, y);
    mask = v8u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v8i16(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 8
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFF

  typedef u16 T;
  typedef i16 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15, 14, 13, 12, 11, 10, 9};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v8i16_t x, y, z;

  // test set
  {
    x = v8i16_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v8i16_set(0, 1, 2, 3, 4, 5, 6, 7);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v8i16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v8i16_load_unaligned(b);
    x = v8i16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v8i16_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v8i16_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v8i16_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v8i16_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v8i16_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v8i16_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v8i16_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v8i16_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v8i16_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v8i16_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v8i16_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v8i16_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v8i16_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v8i16_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v8i16_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v8i16_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v8i16_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v8i16_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v8i16_eq(x, y);
    mask = v8i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v8i16_lt(x, y);
    mask = v8i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v8i16_gt(x, y);
    mask = v8i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v8i16_leq(x, y);
    mask = v8i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v8i16_geq(x, y);
    mask = v8i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v4u32(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 4
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFFFFFFu

  typedef u32 T;
  typedef u32 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15, 14, 13};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v4u32_t x, y, z;

  // test set
  {
    x = v4u32_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v4u32_set(0, 1, 2, 3);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v4u32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v4u32_load_unaligned(b);
    x = v4u32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v4u32_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v4u32_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v4u32_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v4u32_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v4u32_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v4u32_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v4u32_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v4u32_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v4u32_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v4u32_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v4u32_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v4u32_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v4u32_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v4u32_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v4u32_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v4u32_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v4u32_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v4u32_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v4u32_eq(x, y);
    mask = v4u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v4u32_lt(x, y);
    mask = v4u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v4u32_gt(x, y);
    mask = v4u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v4u32_leq(x, y);
    mask = v4u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v4u32_geq(x, y);
    mask = v4u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v4i32(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 4
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFFFFFFu

  typedef u32 T;
  typedef i32 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15, 14, 13};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v4i32_t x, y, z;

  // test set
  {
    x = v4i32_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v4i32_set(0, 1, 2, 3);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v4i32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v4i32_load_unaligned(b);
    x = v4i32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v4i32_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v4i32_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v4i32_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v4i32_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v4i32_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v4i32_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v4i32_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v4i32_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v4i32_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v4i32_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v4i32_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v4i32_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v4i32_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v4i32_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v4i32_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v4i32_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v4i32_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v4i32_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v4i32_eq(x, y);
    mask = v4i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v4i32_lt(x, y);
    mask = v4i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v4i32_gt(x, y);
    mask = v4i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v4i32_leq(x, y);
    mask = v4i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v4i32_geq(x, y);
    mask = v4i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v2u64(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 2
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFFFFFFFFFFFFFFull

  typedef u64 T;
  typedef u64 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v2u64_t x, y, z;

  // test set
  {
    x = v2u64_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v2u64_set(0, 1);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v2u64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v2u64_load_unaligned(b);
    x = v2u64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v2u64_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v2u64_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v2u64_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v2u64_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v2u64_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v2u64_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v2u64_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v2u64_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v2u64_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v2u64_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v2u64_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v2u64_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v2u64_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v2u64_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v2u64_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v2u64_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v2u64_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v2u64_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v2u64_eq(x, y);
    mask = v2u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v2u64_lt(x, y);
    mask = v2u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v2u64_gt(x, y);
    mask = v2u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v2u64_leq(x, y);
    mask = v2u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v2u64_geq(x, y);
    mask = v2u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v2i64(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 2
#undef ALIGN
#define ALIGN 16
#undef TRUE
#define TRUE 0xFFFFFFFFFFFFFFFFull

  typedef u64 T;
  typedef i64 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2};

  TT __attribute__((aligned(1))) b[NUM] = {16, 15};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v2i64_t x, y, z;

  // test set
  {
    x = v2i64_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v2i64_set(0, 1);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v2i64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v2i64_load_unaligned(b);
    x = v2i64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v2i64_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v2i64_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v2i64_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v2i64_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v2i64_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v2i64_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v2i64_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v2i64_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v2i64_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v2i64_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v2i64_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v2i64_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v2i64_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v2i64_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v2i64_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v2i64_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v2i64_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v2i64_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v2i64_eq(x, y);
    mask = v2i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v2i64_lt(x, y);
    mask = v2i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v2i64_gt(x, y);
    mask = v2i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v2i64_leq(x, y);
    mask = v2i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v2i64_geq(x, y);
    mask = v2i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v32u8(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 32
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFF

  typedef u8 T;
  typedef u8 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

  TT __attribute__((aligned(1)))
  b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
            16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v32u8_t x, y, z;

  // test set
  {
    x = v32u8_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v32u8_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v32u8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v32u8_load_unaligned(b);
    x = v32u8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v32u8_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v32u8_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v32u8_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v32u8_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v32u8_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v32u8_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v32u8_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v32u8_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v32u8_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v32u8_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v32u8_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v32u8_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v32u8_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v32u8_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v32u8_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v32u8_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v32u8_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v32u8_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v32u8_eq(x, y);
    mask = v32u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v32u8_lt(x, y);
    mask = v32u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v32u8_gt(x, y);
    mask = v32u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v32u8_leq(x, y);
    mask = v32u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v32u8_geq(x, y);
    mask = v32u8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v32i8(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 32
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFF

  typedef u8 T;
  typedef i8 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

  TT __attribute__((aligned(1)))
  b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
            16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v32i8_t x, y, z;

  // test set
  {
    x = v32i8_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v32i8_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v32i8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v32i8_load_unaligned(b);
    x = v32i8_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v32i8_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v32i8_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v32i8_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v32i8_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v32i8_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v32i8_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v32i8_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v32i8_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v32i8_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v32i8_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v32i8_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v32i8_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v32i8_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v32i8_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v32i8_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v32i8_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v32i8_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v32i8_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v32i8_eq(x, y);
    mask = v32i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v32i8_lt(x, y);
    mask = v32i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v32i8_gt(x, y);
    mask = v32i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v32i8_leq(x, y);
    mask = v32i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v32i8_geq(x, y);
    mask = v32i8_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v16u16(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 16
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFF

  typedef u16 T;
  typedef u16 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  TT __attribute__((aligned(1)))
  b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v16u16_t x, y, z;

  // test set
  {
    x = v16u16_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v16u16_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v16u16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v16u16_load_unaligned(b);
    x = v16u16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v16u16_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v16u16_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v16u16_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v16u16_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v16u16_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v16u16_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v16u16_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v16u16_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v16u16_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v16u16_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v16u16_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v16u16_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v16u16_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v16u16_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v16u16_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v16u16_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v16u16_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v16u16_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v16u16_eq(x, y);
    mask = v16u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v16u16_lt(x, y);
    mask = v16u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v16u16_gt(x, y);
    mask = v16u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v16u16_leq(x, y);
    mask = v16u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v16u16_geq(x, y);
    mask = v16u16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v16i16(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 16
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFF

  typedef u16 T;
  typedef i16 TT;

  TT __attribute__((aligned(ALIGN)))
  a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  TT __attribute__((aligned(1)))
  b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v16i16_t x, y, z;

  // test set
  {
    x = v16i16_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v16i16_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v16i16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v16i16_load_unaligned(b);
    x = v16i16_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v16i16_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v16i16_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v16i16_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v16i16_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v16i16_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v16i16_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v16i16_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v16i16_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v16i16_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v16i16_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v16i16_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v16i16_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v16i16_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v16i16_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v16i16_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v16i16_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v16i16_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v16i16_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v16i16_eq(x, y);
    mask = v16i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v16i16_lt(x, y);
    mask = v16i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v16i16_gt(x, y);
    mask = v16i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v16i16_leq(x, y);
    mask = v16i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v16i16_geq(x, y);
    mask = v16i16_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v8u32(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 8
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFFFFFFull

  typedef u32 T;
  typedef u32 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8};

  TT __attribute__((aligned(1))) b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v8u32_t x, y, z;

  // test set
  {
    x = v8u32_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v8u32_set(0, 1, 2, 3, 4, 5, 6, 7);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v8u32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v8u32_load_unaligned(b);
    x = v8u32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v8u32_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v8u32_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v8u32_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v8u32_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v8u32_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v8u32_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v8u32_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v8u32_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v8u32_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v8u32_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v8u32_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v8u32_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v8u32_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v8u32_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v8u32_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v8u32_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v8u32_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v8u32_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v8u32_eq(x, y);
    mask = v8u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v8u32_lt(x, y);
    mask = v8u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v8u32_gt(x, y);
    mask = v8u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v8u32_leq(x, y);
    mask = v8u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v8u32_geq(x, y);
    mask = v8u32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v8i32(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 8
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFFFFFFull

  typedef u32 T;
  typedef i32 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4, 5, 6, 7, 8};

  TT __attribute__((aligned(1))) b[NUM] = {32, 31, 30, 29, 28, 27, 26, 25};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v8i32_t x, y, z;

  // test set
  {
    x = v8i32_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v8i32_set(0, 1, 2, 3, 4, 5, 6, 7);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v8i32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v8i32_load_unaligned(b);
    x = v8i32_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v8i32_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v8i32_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v8i32_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v8i32_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v8i32_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v8i32_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v8i32_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v8i32_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v8i32_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v8i32_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v8i32_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v8i32_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v8i32_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v8i32_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v8i32_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v8i32_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v8i32_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v8i32_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v8i32_eq(x, y);
    mask = v8i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v8i32_lt(x, y);
    mask = v8i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v8i32_gt(x, y);
    mask = v8i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v8i32_leq(x, y);
    mask = v8i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v8i32_geq(x, y);
    mask = v8i32_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v4u64(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 4
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFFFFFFFFFFFFFFull

  typedef u64 T;
  typedef u64 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4};

  TT __attribute__((aligned(1))) b[NUM] = {32, 31, 30, 29};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v4u64_t x, y, z;

  // test set
  {
    x = v4u64_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v4u64_set(0, 1, 2, 3);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v4u64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v4u64_load_unaligned(b);
    x = v4u64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v4u64_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v4u64_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v4u64_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v4u64_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v4u64_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v4u64_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v4u64_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v4u64_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v4u64_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v4u64_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v4u64_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v4u64_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v4u64_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v4u64_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v4u64_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v4u64_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v4u64_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v4u64_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v4u64_eq(x, y);
    mask = v4u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v4u64_lt(x, y);
    mask = v4u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v4u64_gt(x, y);
    mask = v4u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v4u64_leq(x, y);
    mask = v4u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v4u64_geq(x, y);
    mask = v4u64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

static test_t TEST_v4i64(void) {

  test_t test = TEST_MAKE();

#undef NUM
#define NUM 4
#undef ALIGN
#define ALIGN 32
#undef TRUE
#define TRUE 0xFFFFFFFFFFFFFFFFull

  typedef u64 T;
  typedef i64 TT;

  TT __attribute__((aligned(ALIGN))) a[NUM] = {1, 2, 3, 4};

  TT __attribute__((aligned(1))) b[NUM] = {32, 31, 30, 29};

  TT __attribute__((aligned(ALIGN))) c[NUM] = {0};
  TT __attribute__((aligned(1))) d[NUM] = {0};

  v4i64_t x, y, z;

  // test set
  {
    x = v4i64_set1(0);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], 0, NULL);
    }
    y = v4i64_set(0, 1, 2, 3);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], i, NULL);
    }
  }

  // test load
  {
    x = v4i64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, x[i], a[i], NULL);
    }

    y = v4i64_load_unaligned(b);
    x = v4i64_load_aligned(a);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, y[i], b[i], NULL);
    }
  }

  // test store
  {
    v4i64_store_aligned(c, x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, a[i], c[i], NULL);
    }
    v4i64_store_unaligned(d, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, b[i], d[i], NULL);
    }
  }

  // test cmp
  {
    z = v4i64_eq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] == b[i] ? TRUE : 0), NULL);
    }
    z = v4i64_lt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] < b[i] ? TRUE : 0), NULL);
    }
    z = v4i64_gt(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] > b[i] ? TRUE : 0), NULL);
    }
    z = v4i64_leq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] <= b[i] ? TRUE : 0), NULL);
    }
    z = v4i64_geq(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (T)z[i], (T)(a[i] >= b[i] ? TRUE : 0), NULL);
    }
  }

  // test arithmetic
  {
    z = v4i64_add(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] + (T)b[i]), NULL);
    }
    z = v4i64_sub(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] - (T)b[i]), NULL);
    }
    z = v4i64_mul(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] * (T)b[i]), NULL);
    }
    z = v4i64_div(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] / (T)b[i]), NULL);
    }
    z = v4i64_mod(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] % (T)b[i]), NULL);
    }
  }

  // test bitwise
  {
    z = v4i64_and(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] & (T)b[i]), NULL);
    }
    z = v4i64_or(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] | (T)b[i]), NULL);
    }
    z = v4i64_xor(x, y);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] ^ (T)b[i]), NULL);
    }
    z = v4i64_not(x);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)(~(T)a[i]), NULL);
    }
    z = v4i64_sl(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] << 2), NULL);
    }
    z = v4i64_sr(x, 2);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, z[i], (TT)((T)a[i] >> 2), NULL);
    }
  }

  // test mask
  {
    u32 mask;
    z = v4i64_eq(x, y);
    mask = v4i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] == b[i], NULL);
    }
    z = v4i64_lt(x, y);
    mask = v4i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] < b[i], NULL);
    }
    z = v4i64_gt(x, y);
    mask = v4i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] > b[i], NULL);
    }
    z = v4i64_leq(x, y);
    mask = v4i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] <= b[i], NULL);
    }
    z = v4i64_geq(x, y);
    mask = v4i64_mask(z);
    for (int i = 0; i < NUM; ++i) {
      TEST_UINT(&test, (mask >> i) & 1, a[i] >= b[i], NULL);
    }
  }

  return test;
}

int main(void) {
  TEST_RUN(TEST_v16u8());
  TEST_RUN(TEST_v16i8());
  TEST_RUN(TEST_v8u16());
  TEST_RUN(TEST_v8i16());
  TEST_RUN(TEST_v4u32());
  TEST_RUN(TEST_v4i32());
  TEST_RUN(TEST_v2u64());
  TEST_RUN(TEST_v2i64());
  TEST_RUN(TEST_v32u8());
  TEST_RUN(TEST_v32i8());
  TEST_RUN(TEST_v16u16());
  TEST_RUN(TEST_v16i16());
  TEST_RUN(TEST_v8u32());
  TEST_RUN(TEST_v8i32());
  TEST_RUN(TEST_v4u64());
  TEST_RUN(TEST_v4i64());
  return TEST_CLEANUP();
}
