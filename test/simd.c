#include <broken/unit.h>
#include <merlin/simd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

//=================================v16u8========================================
static test_t TEST_v16u8(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint8_t *arr_a = malloc(16 * sizeof(*arr_a));
  uint8_t *arr_b = malloc(17 * sizeof(*arr_b));
  uint8_t *arr_a_store = malloc(16 * sizeof(*arr_a_store));
  uint8_t *arr_b_store = malloc(17 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 16; ++i) {
    arr_a[i] = (uint8_t)rand();
    arr_b[i + 1] = (uint8_t)rand();
  }

  v16u8_t a;
  v16u8_t b;
  v16u8_t c;
  v16u8_t tmp;
  uint32_t mask;

  // test load
  a = v16u8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v16u8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v16u8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v16u8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v16u8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v16u8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v16u8_eq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v16u8_lt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v16u8_gt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v16u8_leq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v16u8_geq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v16u8_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v16u8_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] + b[i]), NULL);
  }

  c = v16u8_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] - b[i]), NULL);
  }

  c = v16u8_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v16u8_eq(b, v16u8_set1(0));
  tmp = v16u8_and(tmp, v16u8_set1(1));
  b = v16u8_or(b, tmp);

  c = v16u8_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] / b[i]), NULL);
  }

  c = v16u8_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v16u8_and(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v16u8_or(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v16u8_xor(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v16u8_not(a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = v16u8_sl(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] << shift_val), NULL);
  }

  c = v16u8_sr(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v16i8========================================
static test_t TEST_v16i8(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int8_t *arr_a = malloc(16 * sizeof(*arr_a));
  int8_t *arr_b = malloc(17 * sizeof(*arr_b));
  int8_t *arr_a_store = malloc(16 * sizeof(*arr_a_store));
  int8_t *arr_b_store = malloc(17 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 16; ++i) {
    arr_a[i] = (int8_t)rand();
    arr_b[i + 1] = (int8_t)rand();
  }

  v16i8_t a;
  v16i8_t b;
  v16i8_t c;
  v16i8_t tmp;
  uint32_t mask;

  // test load
  a = v16i8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[i], NULL);
  }
  b = v16i8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v16i8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v16i8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v16i8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[0], NULL);
  }
  a = v16i8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v16i8_eq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v16i8_lt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v16i8_gt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v16i8_leq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v16i8_geq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v16i8_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_INT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v16i8_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (u8)a[i] + (u8)b[i], NULL);
  }

  c = v16i8_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (u8)a[i] - (u8)b[i], NULL);
  }

  c = v16i8_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (u8)a[i] * (u8)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v16i8_eq(b, v16i8_set1(0));
  tmp = v16u8_and((v16u8_t)tmp, v16u8_set1(1));
  b = v16u8_or(b, tmp);

  c = v16i8_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (u8)a[i] / (u8)b[i], NULL);
  }

  c = v16i8_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (u8)a[i] % (u8)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v8u16========================================
static test_t TEST_v8u16(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint16_t *arr_a = malloc(8 * sizeof(*arr_a));
  uint16_t *arr_b = malloc(9 * sizeof(*arr_b));
  uint16_t *arr_a_store = malloc(8 * sizeof(*arr_a_store));
  uint16_t *arr_b_store = malloc(9 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 8; ++i) {
    arr_a[i] = (uint16_t)rand();
    arr_b[i + 1] = (uint16_t)rand();
  }

  v8u16_t a;
  v8u16_t b;
  v8u16_t c;
  v8u16_t tmp;
  uint32_t mask;

  // test load
  a = v8u16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v8u16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v8u16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v8u16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v8u16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v8u16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v8u16_eq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v8u16_lt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v8u16_gt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v8u16_leq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v8u16_geq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v8u16_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v8u16_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] + b[i]), NULL);
  }

  c = v8u16_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] - b[i]), NULL);
  }

  c = v8u16_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v8u16_eq(b, v8u16_set1(0));
  tmp = v8u16_and(tmp, v8u16_set1(1));
  b = v8u16_or(b, tmp);

  c = v8u16_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] / b[i]), NULL);
  }

  c = v8u16_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v8u16_and(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v8u16_or(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v8u16_xor(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v8u16_not(a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(~a[i]), NULL);
  }

  b = v8u16_set1(rand() % 16);

  const uint32_t shift_val = rand() % 16;
  c = v8u16_sl(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] << shift_val), NULL);
  }

  c = v8u16_sr(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v8i16========================================
static test_t TEST_v8i16(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int16_t *arr_a = malloc(8 * sizeof(*arr_a));
  int16_t *arr_b = malloc(9 * sizeof(*arr_b));
  int16_t *arr_a_store = malloc(8 * sizeof(*arr_a_store));
  int16_t *arr_b_store = malloc(9 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 8; ++i) {
    arr_a[i] = (int16_t)rand();
    arr_b[i + 1] = (int16_t)rand();
  }

  v8i16_t a;
  v8i16_t b;
  v8i16_t c;
  v8i16_t tmp;
  uint32_t mask;

  // test load
  a = v8i16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v8i16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v8i16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v8i16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v8i16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v8i16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v8i16_eq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v8i16_lt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v8i16_gt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v8i16_leq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v8i16_geq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v8i16_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v8i16_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u16)a[i] + (u16)b[i], NULL);
  }

  c = v8i16_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u16)a[i] - (u16)b[i], NULL);
  }

  c = v8i16_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u16)a[i] * (u16)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v8i16_eq(b, v8i16_set1(0));
  tmp = v8u16_and(tmp, v8u16_set1(1));
  b = v8u16_or(b, tmp);

  c = v8i16_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u16)a[i] / (u16)b[i], NULL);
  }

  c = v8i16_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u16)a[i] % (u16)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v4u32========================================
static test_t TEST_v4u32(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint32_t *arr_a = malloc(4 * sizeof(*arr_a));
  uint32_t *arr_b = malloc(5 * sizeof(*arr_b));
  uint32_t *arr_a_store = malloc(4 * sizeof(*arr_a_store));
  uint32_t *arr_b_store = malloc(5 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 4; ++i) {
    arr_a[i] = (uint32_t)rand();
    arr_b[i + 1] = (uint32_t)rand();
  }

  v4u32_t a;
  v4u32_t b;
  v4u32_t c;
  v4u32_t tmp;
  uint32_t mask;

  // test load
  a = v4u32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v4u32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v4u32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v4u32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v4u32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v4u32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v4u32_eq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v4u32_lt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v4u32_gt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v4u32_leq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v4u32_geq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v4u32_mask(c);
  for (uint32_t i = 0; i < 4; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v4u32_add(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] + b[i]), NULL);
  }

  c = v4u32_sub(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] - b[i]), NULL);
  }

  c = v4u32_mul(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v4u32_eq(b, v4u32_set1(0));
  tmp = v4u32_and(tmp, v4u32_set1(1));
  b = v4u32_or(b, tmp);

  c = v4u32_div(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] / b[i]), NULL);
  }

  c = v4u32_mod(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v4u32_and(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v4u32_or(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v4u32_xor(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v4u32_not(a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(~a[i]), NULL);
  }

  b = v4u32_set1(rand() % 32);

  const uint32_t shift_val = rand() % 32;
  c = v4u32_sl(a, shift_val);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] << shift_val), NULL);
  }

  c = v4u32_sr(a, shift_val);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v4i32========================================
static test_t TEST_v4i32(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int32_t *arr_a = malloc(4 * sizeof(*arr_a));
  int32_t *arr_b = malloc(5 * sizeof(*arr_b));
  int32_t *arr_a_store = malloc(4 * sizeof(*arr_a_store));
  int32_t *arr_b_store = malloc(5 * sizeof(*arr_b_store));

  for (int32_t i = 0; i < 4; ++i) {
    arr_a[i] = (int32_t)rand();
    arr_b[i + 1] = (int32_t)rand();
  }

  v4i32_t a;
  v4i32_t b;
  v4i32_t c;
  v4i32_t tmp;
  uint32_t mask;

  // test load
  a = v4i32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v4i32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v4i32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v4i32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v4i32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v4i32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v4i32_eq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v4i32_lt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v4i32_gt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v4i32_leq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v4i32_geq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v4i32_mask(c);
  for (uint32_t i = 0; i < 4; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v4i32_add(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] + (u32)b[i], NULL);
  }

  c = v4i32_sub(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] - (u32)b[i], NULL);
  }

  c = v4i32_mul(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] * (u32)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v4i32_eq(b, v4i32_set1(0));
  tmp = v4u32_and(tmp, v4i32_set1(1));
  b = v4u32_or(b, tmp);

  c = v4i32_div(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] / (u8)b[i], NULL);
  }

  c = v4i32_mod(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] % (u8)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v2u64========================================
static test_t TEST_v2u64(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint64_t *arr_a = malloc(2 * sizeof(*arr_a));
  uint64_t *arr_b = malloc(3 * sizeof(*arr_b));
  uint64_t *arr_a_store = malloc(2 * sizeof(*arr_a_store));
  uint64_t *arr_b_store = malloc(3 * sizeof(*arr_b_store));

  for (uint64_t i = 0; i < 2; ++i) {
    arr_a[i] = (uint64_t)rand();
    arr_b[i + 1] = (uint64_t)rand();
  }

  v2u64_t a;
  v2u64_t b;
  v2u64_t c;
  v2u64_t tmp;
  uint32_t mask;

  // test load
  a = v2u64_load_aligned((void *)arr_a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v2u64_load_unaligned((void *)(&arr_b[1]));
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v2u64_store_aligned((void *)arr_a_store, a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v2u64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v2u64_set1(arr_a[0]);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v2u64_set(arr_a[0], arr_a[1]);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v2u64_eq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v2u64_lt(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v2u64_gt(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v2u64_leq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v2u64_geq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v2u64_mask(c);
  for (uint64_t i = 0; i < 2; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v2u64_add(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] + b[i]), NULL);
  }

  c = v2u64_sub(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] - b[i]), NULL);
  }

  c = v2u64_mul(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v2u64_eq(b, v2u64_set1(0));
  tmp = v2u64_and(tmp, v2u64_set1(1));
  b = v2u64_or(b, tmp);

  c = v2u64_div(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] / b[i]), NULL);
  }

  c = v2u64_mod(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v2u64_and(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v2u64_or(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v2u64_xor(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v2u64_not(a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(~a[i]), NULL);
  }

  b = v2u64_set1(rand() % 32);

  const uint32_t shift_val = rand() % 64;
  c = v2u64_sl(a, shift_val);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] << shift_val), NULL);
  }

  c = v2u64_sr(a, shift_val);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v2i64========================================
static test_t TEST_v2i64(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int64_t *arr_a = malloc(2 * sizeof(*arr_a));
  int64_t *arr_b = malloc(3 * sizeof(*arr_b));
  int64_t *arr_a_store = malloc(2 * sizeof(*arr_a_store));
  int64_t *arr_b_store = malloc(3 * sizeof(*arr_b_store));

  for (int64_t i = 0; i < 2; ++i) {
    arr_a[i] = (int64_t)rand();
    arr_b[i + 1] = (int64_t)rand();
  }

  v2i64_t a;
  v2i64_t b;
  v2i64_t c;
  v2i64_t tmp;
  uint32_t mask;

  // test load
  a = v2i64_load_aligned((void *)arr_a);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v2i64_load_unaligned((void *)(&arr_b[1]));
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v2i64_store_aligned((void *)arr_a_store, a);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v2i64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v2i64_set1(arr_a[0]);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v2i64_set(arr_a[0], arr_a[1]);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v2i64_eq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v2i64_lt(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v2i64_gt(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v2i64_leq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v2i64_geq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v2i64_mask(c);
  for (int64_t i = 0; i < 2; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v2i64_add(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] + (u64)b[i], NULL);
  }

  c = v2i64_sub(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] - (u64)b[i], NULL);
  }

  c = v2i64_mul(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] * (u64)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v2i64_eq(b, v2i64_set1(0));
  tmp = v2u64_and(tmp, v2i64_set1(1));
  b = v2u64_or(b, tmp);

  c = v2i64_div(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] / (u64)b[i], NULL);
  }

  c = v2i64_mod(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] % (u64)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v32u8========================================
static test_t TEST_v32u8(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint8_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  uint8_t *arr_b = malloc(33 * sizeof(*arr_b));
  uint8_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  uint8_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 32; ++i) {
    arr_a[i] = (uint8_t)rand();
    arr_b[i + 1] = (uint8_t)rand();
  }

  v32u8_t a;
  v32u8_t b;
  v32u8_t c;
  v32u8_t tmp;
  uint32_t mask;

  // test load
  a = v32u8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v32u8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v32u8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v32u8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v32u8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v32u8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                arr_a[12], arr_a[13], arr_a[14], arr_a[15], arr_a[16],
                arr_a[17], arr_a[18], arr_a[19], arr_a[20], arr_a[21],
                arr_a[22], arr_a[23], arr_a[24], arr_a[25], arr_a[26],
                arr_a[27], arr_a[28], arr_a[29], arr_a[30], arr_a[31]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v32u8_eq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v32u8_lt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v32u8_gt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v32u8_leq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v32u8_geq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v32u8_mask(c);
  for (uint32_t i = 0; i < 32; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v32u8_add(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] + b[i]), NULL);
  }

  c = v32u8_sub(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] - b[i]), NULL);
  }

  c = v32u8_mul(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v32u8_eq(b, v32u8_set1(0));
  tmp = v32u8_and(tmp, v32u8_set1(1));
  b = v32u8_add(b, tmp);

  c = v32u8_div(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] / b[i]), NULL);
  }

  c = v32u8_mod(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v32u8_and(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v32u8_or(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v32u8_xor(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v32u8_not(a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = v32u8_sl(a, shift_val);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] << shift_val), NULL);
  }

  c = v32u8_sr(a, shift_val);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v32i8========================================
static test_t TEST_v32i8(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int8_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  int8_t *arr_b = malloc(33 * sizeof(*arr_b));
  int8_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  int8_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 32; ++i) {
    arr_a[i] = (int8_t)rand();
    arr_b[i + 1] = (int8_t)rand();
  }

  v32i8_t a;
  v32i8_t b;
  v32i8_t c;
  v32i8_t tmp;
  uint32_t mask;

  // test load
  a = v32i8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v32i8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v32i8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v32i8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v32i8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v32i8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                arr_a[12], arr_a[13], arr_a[14], arr_a[15], arr_a[16],
                arr_a[17], arr_a[18], arr_a[19], arr_a[20], arr_a[21],
                arr_a[22], arr_a[23], arr_a[24], arr_a[25], arr_a[26],
                arr_a[27], arr_a[28], arr_a[29], arr_a[30], arr_a[31]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v32i8_eq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v32i8_lt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v32i8_gt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v32i8_leq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v32i8_geq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v32i8_mask(c);
  for (uint32_t i = 0; i < 32; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v32i8_add(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] + (u8)b[i], NULL);
  }

  c = v32i8_sub(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] - (u8)b[i], NULL);
  }

  c = v32i8_mul(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] * (u8)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v32i8_eq(b, v32i8_set1(0));
  tmp = v32u8_and(tmp, v32u8_set1(1));
  b = v32u8_or(b, tmp);

  c = v32i8_div(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] / (u8)b[i], NULL);
  }

  c = v32i8_mod(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (u8)a[i] % (u8)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v16u16========================================
static test_t TEST_v16u16(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint16_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  uint16_t *arr_b = malloc(33 * sizeof(*arr_b));
  uint16_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  uint16_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 16; ++i) {
    arr_a[i] = (uint16_t)rand();
    arr_b[i + 1] = (uint16_t)rand();
  }

  v16u16_t a;
  v16u16_t b;
  v16u16_t c;
  v16u16_t tmp;
  uint32_t mask;

  // test load
  a = v16u16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v16u16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v16u16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v16u16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v16u16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v16u16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                 arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                 arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v16u16_eq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v16u16_lt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v16u16_gt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v16u16_leq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v16u16_geq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v16u16_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v16u16_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] + b[i]), NULL);
  }

  c = v16u16_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] - b[i]), NULL);
  }

  c = v16u16_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v16u16_eq(b, v16u16_set1(0));
  tmp = v16u16_and(tmp, v16u16_set1(1));
  b = v16u16_or(b, tmp);

  c = v16u16_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] / b[i]), NULL);
  }

  c = v16u16_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v16u16_and(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v16u16_or(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v16u16_xor(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v16u16_not(a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = v16u16_sl(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] << shift_val), NULL);
  }

  c = v16u16_sr(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v16i16========================================
static test_t TEST_v16i16(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int16_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  int16_t *arr_b = malloc(33 * sizeof(*arr_b));
  int16_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  int16_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 16; ++i) {
    arr_a[i] = (int16_t)rand();
    arr_b[i + 1] = (int16_t)rand();
  }

  v16i16_t a;
  v16i16_t b;
  v16i16_t c;
  v16i16_t tmp;
  uint32_t mask;

  // test load
  a = v16i16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v16i16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v16i16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v16i16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v16i16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v16i16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                 arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11],
                 arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v16i16_eq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v16i16_lt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v16i16_gt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v16i16_leq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v16i16_geq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v16i16_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = v16i16_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (u16)(a[i] + (u16)b[i]), NULL);
  }

  c = v16i16_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], ((u16)a[i] - (u16)b[i]), NULL);
  }

  c = v16i16_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], ((u16)a[i] * (u16)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v16i16_eq(b, v16i16_set1(0));
  tmp = v16u16_and(tmp, v16i16_set1(1));
  b = v16u16_or(b, tmp);

  c = v16i16_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], ((u16)a[i] / (u16)b[i]), NULL);
  }

  c = v16i16_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], ((u16)a[i] % (u16)b[i]), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v8u32========================================
static test_t TEST_v8u32(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint32_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  uint32_t *arr_b = malloc(33 * sizeof(*arr_b));
  uint32_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  uint32_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint32_t i = 0; i < 8; ++i) {
    arr_a[i] = (uint32_t)rand();
    arr_b[i + 1] = (uint32_t)rand();
  }

  v8u32_t a;
  v8u32_t b;
  v8u32_t c;
  v8u32_t tmp;
  uint32_t mask;

  // test load
  a = v8u32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v8u32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v8u32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v8u32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v8u32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v8u32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v8u32_eq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v8u32_lt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v8u32_gt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v8u32_leq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v8u32_geq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v8u32_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = v8u32_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] + b[i]), NULL);
  }

  c = v8u32_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] - b[i]), NULL);
  }

  c = v8u32_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v8u32_eq(b, v8u32_set1(0));
  tmp = v8u32_and(tmp, v8u32_set1(1));
  b = v8u32_or(b, tmp);

  c = v8u32_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] / b[i]), NULL);
  }

  c = v8u32_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v8u32_and(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v8u32_or(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v8u32_xor(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v8u32_not(a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = v8u32_sl(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] << shift_val), NULL);
  }

  c = v8u32_sr(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v8i32========================================
static test_t TEST_v8i32(void) {
  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int32_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  int32_t *arr_b = malloc(33 * sizeof(*arr_b));
  int32_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  int32_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (int32_t i = 0; i < 8; ++i) {
    arr_a[i] = (int32_t)rand();
    arr_b[i + 1] = (int32_t)rand();
  }

  v8i32_t a;
  v8i32_t b;
  v8i32_t c;
  v8i32_t tmp;
  int32_t mask;

  // test load
  a = v8i32_load_aligned((void *)arr_a);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v8i32_load_unaligned((void *)(&arr_b[1]));
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v8i32_store_aligned((void *)arr_a_store, a);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v8i32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v8i32_set1(arr_a[0]);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v8i32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                arr_a[6], arr_a[7]);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v8i32_eq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v8i32_lt(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v8i32_gt(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v8i32_leq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v8i32_geq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v8i32_mask(c);
  for (int32_t i = 0; i < 8; ++i) {
    if (mask & (((int32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = v8i32_add(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] + (u32)b[i], NULL);
  }

  c = v8i32_sub(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] - (u32)b[i], NULL);
  }

  c = v8i32_mul(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] * (u32)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v8i32_eq(b, v8i32_set1(0));
  tmp = v8u32_and(tmp, v8i32_set1(1));
  b = v8u32_or(b, tmp);

  c = v8i32_div(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] / (u32)b[i], NULL);
  }

  c = v8i32_mod(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (u32)a[i] % (u32)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v4u64========================================
static test_t TEST_v4u64(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  uint64_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  uint64_t *arr_b = malloc(33 * sizeof(*arr_b));
  uint64_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  uint64_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (uint64_t i = 0; i < 4; ++i) {
    arr_a[i] = (uint64_t)rand();
    arr_b[i + 1] = (uint64_t)rand();
  }

  v4u64_t a;
  v4u64_t b;
  v4u64_t c;
  v4u64_t tmp;
  uint64_t mask;

  // test load
  a = v4u64_load_aligned((void *)arr_a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v4u64_load_unaligned((void *)(&arr_b[1]));
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v4u64_store_aligned((void *)arr_a_store, a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v4u64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v4u64_set1(arr_a[0]);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v4u64_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v4u64_eq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v4u64_lt(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v4u64_gt(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v4u64_leq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v4u64_geq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v4u64_mask(c);
  for (uint64_t i = 0; i < 4; ++i) {
    if (mask & (((uint64_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = v4u64_add(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] + b[i]), NULL);
  }

  c = v4u64_sub(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] - b[i]), NULL);
  }

  c = v4u64_mul(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v4u64_eq(b, v4u64_set1(0));
  tmp = v4u64_and(tmp, v4u64_set1(1));
  b = v4u64_or(b, tmp);

  c = v4u64_div(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] / b[i]), NULL);
  }

  c = v4u64_mod(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = v4u64_and(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = v4u64_or(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = v4u64_xor(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = v4u64_not(a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(~a[i]), NULL);
  }

  const uint64_t shift_val = rand() % 8;
  c = v4u64_sl(a, shift_val);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] << shift_val), NULL);
  }

  c = v4u64_sr(a, shift_val);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] >> shift_val), NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

  return test;
}

//=================================v4i64========================================
static test_t TEST_v4i64(void) {
  // time_t t = time(NULL);
  time_t t = 1714331144;
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", __func__, t));
  srand(t);

  int64_t *arr_a = aligned_alloc(32, 32 * sizeof(*arr_a));
  int64_t *arr_b = malloc(33 * sizeof(*arr_b));
  int64_t *arr_a_store = aligned_alloc(32, 32 * sizeof(*arr_a_store));
  int64_t *arr_b_store = malloc(33 * sizeof(*arr_b_store));

  for (int64_t i = 0; i < 4; ++i) {
    arr_a[i] = (int64_t)rand();
    arr_b[i + 1] = (int64_t)rand();
  }

  v4i64_t a;
  v4i64_t b;
  v4i64_t c;
  v4i64_t tmp;
  int64_t mask;

  // test load
  a = v4i64_load_aligned((void *)arr_a);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = v4i64_load_unaligned((void *)(&arr_b[1]));
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  v4i64_store_aligned((void *)arr_a_store, a);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  v4i64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = v4i64_set1(arr_a[0]);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = v4i64_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = v4i64_eq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = v4i64_lt(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = v4i64_gt(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = v4i64_leq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = v4i64_geq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous eq data
  mask = v4i64_mask(c);
  for (int64_t i = 0; i < 4; ++i) {
    if (mask & (((int64_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = v4i64_add(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] + (u64)b[i], NULL);
  }

  c = v4i64_sub(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] - (u64)b[i], NULL);
  }

  c = v4i64_mul(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] * (u64)b[i], NULL);
  }

  // some magic require so we avoid div by 0
  tmp = v4i64_eq(b, v4i64_set1(0));
  tmp = v4u64_and(tmp, v4i64_set1(1));
  b = v4u64_or(b, tmp);

  c = v4i64_div(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] / (u64)b[i], NULL);
  }

  c = v4i64_mod(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (u64)a[i] % (u64)b[i], NULL);
  }

  free(arr_a);
  free(arr_b);
  free(arr_a_store);
  free(arr_b_store);

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
