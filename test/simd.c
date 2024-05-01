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

  mrln_v16u8_t a;
  mrln_v16u8_t b;
  mrln_v16u8_t c;
  mrln_v16u8_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v16u8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v16u8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v16u8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v16u8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v16u8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v16u8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10],
                     arr_a[11], arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v16u8_cmpeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v16u8_cmplt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v16u8_cmpgt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v16u8_cmpleq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v16u8_cmpgeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v16u8_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v16u8_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v16u8_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v16u8_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v16u8_cmpeq(b, mrln_v16u8_set1(0));
  tmp = mrln_v16u8_and(tmp, mrln_v16u8_set1(1));
  b = mrln_v16u8_or(b, tmp);

  c = mrln_v16u8_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v16u8_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v16u8_and(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v16u8_or(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v16u8_xor(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v16u8_not(a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = mrln_v16u8_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v16u8_shift_right(a, shift_val);
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

  mrln_v16i8_t a;
  mrln_v16i8_t b;
  mrln_v16i8_t c;
  mrln_v16i8_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v16i8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v16i8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v16i8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v16i8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v16i8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v16i8_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7], arr_a[8], arr_a[9], arr_a[10],
                     arr_a[11], arr_a[12], arr_a[13], arr_a[14], arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_INT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v16i8_cmpeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v16i8_cmplt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v16i8_cmpgt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v16i8_cmpleq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v16i8_cmpgeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v16i8_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_INT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v16i8_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (int8_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v16i8_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (int8_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v16i8_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (int8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v16i8_cmpeq(b, mrln_v16i8_set1(0));
  tmp = mrln_v16u8_and((mrln_v16u8_t)tmp, mrln_v16u8_set1(1));
  b = mrln_v16u8_or(b, tmp);

  c = mrln_v16i8_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (int8_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v16i8_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_INT(&test, c[i], (int8_t)(a[i] % b[i]), NULL);
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

  mrln_v8u16_t a;
  mrln_v8u16_t b;
  mrln_v8u16_t c;
  mrln_v8u16_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v8u16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v8u16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v8u16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v8u16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v8u16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v8u16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v8u16_cmpeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v8u16_cmplt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v8u16_cmpgt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v8u16_cmpleq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v8u16_cmpgeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v8u16_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v8u16_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v8u16_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v8u16_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v8u16_cmpeq(b, mrln_v8u16_set1(0));
  tmp = mrln_v8u16_and(tmp, mrln_v8u16_set1(1));
  b = mrln_v8u16_or(b, tmp);

  c = mrln_v8u16_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v8u16_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v8u16_and(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v8u16_or(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v8u16_xor(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v8u16_not(a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(~a[i]), NULL);
  }

  b = mrln_v8u16_set1(rand() % 16);

  const uint32_t shift_val = rand() % 16;
  c = mrln_v8u16_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v8u16_shift_right(a, shift_val);
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

  mrln_v8i16_t a;
  mrln_v8i16_t b;
  mrln_v8i16_t c;
  mrln_v8i16_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v8i16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v8i16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v8i16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v8i16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v8i16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v8i16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v8i16_cmpeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v8i16_cmplt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v8i16_cmpgt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v8i16_cmpleq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v8i16_cmpgeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v8i16_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v8i16_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v8i16_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v8i16_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)((int16_t)a[i] * (int16_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v8i16_cmpeq(b, mrln_v8i16_set1(0));
  tmp = mrln_v8u16_and(tmp, mrln_v8u16_set1(1));
  b = mrln_v8u16_or(b, tmp);

  c = mrln_v8i16_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v8i16_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] % b[i]), NULL);
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

  mrln_v4u32_t a;
  mrln_v4u32_t b;
  mrln_v4u32_t c;
  mrln_v4u32_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v4u32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v4u32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v4u32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v4u32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v4u32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v4u32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v4u32_cmpeq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v4u32_cmplt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v4u32_cmpgt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v4u32_cmpleq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v4u32_cmpgeq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v4u32_mask(c);
  for (uint32_t i = 0; i < 4; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v4u32_add(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v4u32_sub(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v4u32_mul(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v4u32_cmpeq(b, mrln_v4u32_set1(0));
  tmp = mrln_v4u32_and(tmp, mrln_v4u32_set1(1));
  b = mrln_v4u32_or(b, tmp);

  c = mrln_v4u32_div(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v4u32_mod(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v4u32_and(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v4u32_or(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v4u32_xor(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v4u32_not(a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(~a[i]), NULL);
  }

  b = mrln_v4u32_set1(rand() % 32);

  const uint32_t shift_val = rand() % 32;
  c = mrln_v4u32_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v4u32_shift_right(a, shift_val);
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

  mrln_v4i32_t a;
  mrln_v4i32_t b;
  mrln_v4i32_t c;
  mrln_v4i32_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v4i32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v4i32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v4i32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v4i32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v4i32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v4i32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v4i32_cmpeq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v4i32_cmplt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v4i32_cmpgt(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v4i32_cmpleq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v4i32_cmpgeq(a, b);
  for (uint32_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v4i32_mask(c);
  for (uint32_t i = 0; i < 4; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v4i32_add(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(((uint64_t)a[i] + (uint64_t)b[i])), NULL);
  }

  c = mrln_v4i32_sub(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v4i32_mul(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v4i32_cmpeq(b, mrln_v4i32_set1(0));
  tmp = mrln_v4u32_and(tmp, mrln_v4i32_set1(1));
  b = mrln_v4u32_or(b, tmp);

  c = mrln_v4i32_div(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v4i32_mod(a, b);
  for (uint32_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] % b[i]), NULL);
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

  mrln_v2u64_t a;
  mrln_v2u64_t b;
  mrln_v2u64_t c;
  mrln_v2u64_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v2u64_load_aligned((void *)arr_a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v2u64_load_unaligned((void *)(&arr_b[1]));
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v2u64_store_aligned((void *)arr_a_store, a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v2u64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v2u64_set1(arr_a[0]);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v2u64_set(arr_a[0], arr_a[1]);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v2u64_cmpeq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v2u64_cmplt(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v2u64_cmpgt(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v2u64_cmpleq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v2u64_cmpgeq(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v2u64_mask(c);
  for (uint64_t i = 0; i < 2; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v2u64_add(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v2u64_sub(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v2u64_mul(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v2u64_cmpeq(b, mrln_v2u64_set1(0));
  tmp = mrln_v2u64_and(tmp, mrln_v2u64_set1(1));
  b = mrln_v2u64_or(b, tmp);

  c = mrln_v2u64_div(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v2u64_mod(a, b);
  for (uint64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v2u64_and(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v2u64_or(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v2u64_xor(a, b);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v2u64_not(a);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(~a[i]), NULL);
  }

  b = mrln_v2u64_set1(rand() % 32);

  const uint32_t shift_val = rand() % 64;
  c = mrln_v2u64_shift_left(a, shift_val);
  for (uint64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v2u64_shift_right(a, shift_val);
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

  mrln_v2i64_t a;
  mrln_v2i64_t b;
  mrln_v2i64_t c;
  mrln_v2i64_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v2i64_load_aligned((void *)arr_a);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v2i64_load_unaligned((void *)(&arr_b[1]));
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v2i64_store_aligned((void *)arr_a_store, a);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v2i64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v2i64_set1(arr_a[0]);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v2i64_set(arr_a[0], arr_a[1]);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v2i64_cmpeq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v2i64_cmplt(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v2i64_cmpgt(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v2i64_cmpleq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v2i64_cmpgeq(a, b);
  for (int64_t i = 0; i < 2; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v2i64_mask(c);
  for (int64_t i = 0; i < 2; ++i) {
    if (mask & (1u << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], NULL);
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], NULL);
    }
  }
  TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v2i64_add(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v2i64_sub(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v2i64_mul(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v2i64_cmpeq(b, mrln_v2i64_set1(0));
  tmp = mrln_v2u64_and(tmp, mrln_v2i64_set1(1));
  b = mrln_v2u64_or(b, tmp);

  c = mrln_v2i64_div(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v2i64_mod(a, b);
  for (int64_t i = 0; i < 2; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] % b[i]), NULL);
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

  mrln_v32u8_t a;
  mrln_v32u8_t b;
  mrln_v32u8_t c;
  mrln_v32u8_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v32u8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v32u8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v32u8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v32u8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v32u8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v32u8_set(
      arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5], arr_a[6],
      arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11], arr_a[12], arr_a[13],
      arr_a[14], arr_a[15], arr_a[16], arr_a[17], arr_a[18], arr_a[19],
      arr_a[20], arr_a[21], arr_a[22], arr_a[23], arr_a[24], arr_a[25],
      arr_a[26], arr_a[27], arr_a[28], arr_a[29], arr_a[30], arr_a[31]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v32u8_cmpeq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v32u8_cmplt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v32u8_cmpgt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v32u8_cmpleq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v32u8_cmpgeq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v32u8_mask(c);
  for (uint32_t i = 0; i < 32; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v32u8_add(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v32u8_sub(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v32u8_mul(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v32u8_cmpeq(b, mrln_v32u8_set1(0));
  tmp = mrln_v32u8_and(tmp, mrln_v32u8_set1(1));
  b = mrln_v32u8_add(b, tmp);

  c = mrln_v32u8_div(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v32u8_mod(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v32u8_and(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v32u8_or(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v32u8_xor(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v32u8_not(a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = mrln_v32u8_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v32u8_shift_right(a, shift_val);
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

  mrln_v32i8_t a;
  mrln_v32i8_t b;
  mrln_v32i8_t c;
  mrln_v32i8_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v32i8_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v32i8_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v32i8_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v32i8_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v32i8_set1(arr_a[0]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v32i8_set(
      arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5], arr_a[6],
      arr_a[7], arr_a[8], arr_a[9], arr_a[10], arr_a[11], arr_a[12], arr_a[13],
      arr_a[14], arr_a[15], arr_a[16], arr_a[17], arr_a[18], arr_a[19],
      arr_a[20], arr_a[21], arr_a[22], arr_a[23], arr_a[24], arr_a[25],
      arr_a[26], arr_a[27], arr_a[28], arr_a[29], arr_a[30], arr_a[31]);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v32i8_cmpeq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v32i8_cmplt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v32i8_cmpgt(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v32i8_cmpleq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v32i8_cmpgeq(a, b);
  for (uint32_t i = 0; i < 32; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v32i8_mask(c);
  for (uint32_t i = 0; i < 32; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v32i8_add(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (int8_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v32i8_sub(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (int8_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v32i8_mul(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (int8_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v32i8_cmpeq(b, mrln_v32i8_set1(0));
  tmp = mrln_v32u8_and(tmp, mrln_v32u8_set1(1));
  b = mrln_v32u8_or(b, tmp);

  c = mrln_v32i8_div(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (int8_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v32i8_mod(a, b);
  for (uint32_t i = 0; i < 32; i += 1) {
    TEST_UINT(&test, c[i], (int8_t)(a[i] % b[i]), NULL);
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

  mrln_v16u16_t a;
  mrln_v16u16_t b;
  mrln_v16u16_t c;
  mrln_v16u16_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v16u16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v16u16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v16u16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v16u16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v16u16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v16u16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4],
                      arr_a[5], arr_a[6], arr_a[7], arr_a[8], arr_a[9],
                      arr_a[10], arr_a[11], arr_a[12], arr_a[13], arr_a[14],
                      arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v16u16_cmpeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v16u16_cmplt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v16u16_cmpgt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v16u16_cmpleq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v16u16_cmpgeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v16u16_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v16u16_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v16u16_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v16u16_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v16u16_cmpeq(b, mrln_v16u16_set1(0));
  tmp = mrln_v16u16_and(tmp, mrln_v16u16_set1(1));
  b = mrln_v16u16_or(b, tmp);

  c = mrln_v16u16_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v16u16_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v16u16_and(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v16u16_or(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v16u16_xor(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v16u16_not(a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = mrln_v16u16_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, c[i], (uint16_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v16u16_shift_right(a, shift_val);
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

  mrln_v16i16_t a;
  mrln_v16i16_t b;
  mrln_v16i16_t c;
  mrln_v16i16_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v16i16_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v16i16_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v16i16_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v16i16_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v16i16_set1(arr_a[0]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v16i16_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4],
                      arr_a[5], arr_a[6], arr_a[7], arr_a[8], arr_a[9],
                      arr_a[10], arr_a[11], arr_a[12], arr_a[13], arr_a[14],
                      arr_a[15]);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v16i16_cmpeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v16i16_cmplt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v16i16_cmpgt(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v16i16_cmpleq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v16i16_cmpgeq(a, b);
  for (uint32_t i = 0; i < 16; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v16i16_mask(c);
  for (uint32_t i = 0; i < 16; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 16, 0, NULL);

  // test arithmetic
  c = mrln_v16i16_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v16i16_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v16i16_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] * b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v16i16_cmpeq(b, mrln_v16i16_set1(0));
  tmp = mrln_v16u16_and(tmp, mrln_v16i16_set1(1));
  b = mrln_v16u16_or(b, tmp);

  c = mrln_v16i16_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v16i16_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (int16_t)(a[i] % b[i]), NULL);
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

  mrln_v8u32_t a;
  mrln_v8u32_t b;
  mrln_v8u32_t c;
  mrln_v8u32_t tmp;
  uint32_t mask;

  // test load
  a = mrln_v8u32_load_aligned((void *)arr_a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v8u32_load_unaligned((void *)(&arr_b[1]));
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v8u32_store_aligned((void *)arr_a_store, a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v8u32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v8u32_set1(arr_a[0]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v8u32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7]);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v8u32_cmpeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v8u32_cmplt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v8u32_cmpgt(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v8u32_cmpleq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v8u32_cmpgeq(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v8u32_mask(c);
  for (uint32_t i = 0; i < 8; ++i) {
    if (mask & (((uint32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = mrln_v8u32_add(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v8u32_sub(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v8u32_mul(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)((uint32_t)a[i] * (uint32_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v8u32_cmpeq(b, mrln_v8u32_set1(0));
  tmp = mrln_v8u32_and(tmp, mrln_v8u32_set1(1));
  b = mrln_v8u32_or(b, tmp);

  c = mrln_v8u32_div(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v8u32_mod(a, b);
  for (uint32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v8u32_and(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v8u32_or(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v8u32_xor(a, b);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v8u32_not(a);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(~a[i]), NULL);
  }

  const uint32_t shift_val = rand() % 8;
  c = mrln_v8u32_shift_left(a, shift_val);
  for (uint32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, c[i], (uint32_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v8u32_shift_right(a, shift_val);
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

  mrln_v8i32_t a;
  mrln_v8i32_t b;
  mrln_v8i32_t c;
  mrln_v8i32_t tmp;
  int32_t mask;

  // test load
  a = mrln_v8i32_load_aligned((void *)arr_a);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v8i32_load_unaligned((void *)(&arr_b[1]));
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v8i32_store_aligned((void *)arr_a_store, a);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v8i32_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v8i32_set1(arr_a[0]);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v8i32_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3], arr_a[4], arr_a[5],
                     arr_a[6], arr_a[7]);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v8i32_cmpeq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v8i32_cmplt(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v8i32_cmpgt(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v8i32_cmpleq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v8i32_cmpgeq(a, b);
  for (int32_t i = 0; i < 8; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v8i32_mask(c);
  for (int32_t i = 0; i < 8; ++i) {
    if (mask & (((int32_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %u", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = mrln_v8i32_add(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)((uint64_t)a[i] + (uint64_t)b[i]), NULL);
  }

  c = mrln_v8i32_sub(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v8i32_mul(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v8i32_cmpeq(b, mrln_v8i32_set1(0));
  tmp = mrln_v8u32_and(tmp, mrln_v8i32_set1(1));
  b = mrln_v8u32_or(b, tmp);

  c = mrln_v8i32_div(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v8i32_mod(a, b);
  for (int32_t i = 0; i < 8; i += 1) {
    TEST_UINT(&test, c[i], (int32_t)(a[i] % b[i]), NULL);
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

  mrln_v4u64_t a;
  mrln_v4u64_t b;
  mrln_v4u64_t c;
  mrln_v4u64_t tmp;
  uint64_t mask;

  // test load
  a = mrln_v4u64_load_aligned((void *)arr_a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v4u64_load_unaligned((void *)(&arr_b[1]));
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v4u64_store_aligned((void *)arr_a_store, a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v4u64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v4u64_set1(arr_a[0]);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v4u64_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v4u64_cmpeq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v4u64_cmplt(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v4u64_cmpgt(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v4u64_cmpleq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v4u64_cmpgeq(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v4u64_mask(c);
  for (uint64_t i = 0; i < 4; ++i) {
    if (mask & (((uint64_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = mrln_v4u64_add(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v4u64_sub(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v4u64_mul(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)((uint64_t)a[i] * (uint64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v4u64_cmpeq(b, mrln_v4u64_set1(0));
  tmp = mrln_v4u64_and(tmp, mrln_v4u64_set1(1));
  b = mrln_v4u64_or(b, tmp);

  c = mrln_v4u64_div(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v4u64_mod(a, b);
  for (uint64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] % b[i]), NULL);
  }

  // test bitwise
  c = mrln_v4u64_and(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] & b[i], NULL);
  }

  c = mrln_v4u64_or(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] | b[i], NULL);
  }

  c = mrln_v4u64_xor(a, b);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], a[i] ^ b[i], NULL);
  }

  c = mrln_v4u64_not(a);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(~a[i]), NULL);
  }

  const uint64_t shift_val = rand() % 8;
  c = mrln_v4u64_shift_left(a, shift_val);
  for (uint64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, c[i], (uint64_t)(a[i] << shift_val), NULL);
  }

  c = mrln_v4u64_shift_right(a, shift_val);
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

  mrln_v4i64_t a;
  mrln_v4i64_t b;
  mrln_v4i64_t c;
  mrln_v4i64_t tmp;
  int64_t mask;

  // test load
  a = mrln_v4i64_load_aligned((void *)arr_a);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }
  b = mrln_v4i64_load_unaligned((void *)(&arr_b[1]));
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, b[i], arr_b[i + 1], NULL);
  }

  // test store
  mrln_v4i64_store_aligned((void *)arr_a_store, a);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_a_store[i], arr_a[i], NULL);
  }
  mrln_v4i64_store_unaligned((void *)(&arr_b_store[1]), b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, arr_b_store[i + 1], arr_b[i + 1], NULL);
  }

  // test set
  a = mrln_v4i64_set1(arr_a[0]);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[0], NULL);
  }
  a = mrln_v4i64_set(arr_a[0], arr_a[1], arr_a[2], arr_a[3]);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_UINT(&test, a[i], arr_a[i], NULL);
  }

  // test cmp
  c = mrln_v4i64_cmpeq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] == b[i], NULL);
  }

  c = mrln_v4i64_cmplt(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] < b[i], NULL);
  }

  c = mrln_v4i64_cmpgt(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] > b[i], NULL);
  }

  c = mrln_v4i64_cmpleq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] <= b[i], NULL);
  }

  c = mrln_v4i64_cmpgeq(a, b);
  for (int64_t i = 0; i < 4; ++i) {
    TEST_BOOL(&test, (bool)c[i], a[i] >= b[i], NULL);
  }

  // test mask
  // testing with previous cmpeq data
  mask = mrln_v4i64_mask(c);
  for (int64_t i = 0; i < 4; ++i) {
    if (mask & (((int64_t)1u) << i)) {
      TEST_BOOL(&test, true, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    } else {
      TEST_BOOL(&test, false, a[i] >= b[i], TEST_MAKE_STR("i: %lu", i));
    }
  }
  // TEST_UINT(&test, mask >> 8, 0, NULL);

  // test arithmetic
  c = mrln_v4i64_add(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] + b[i]), NULL);
  }

  c = mrln_v4i64_sub(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] - b[i]), NULL);
  }

  c = mrln_v4i64_mul(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)((int64_t)a[i] * (int64_t)b[i]), NULL);
  }

  // some magic require so we avoid div by 0
  tmp = mrln_v4i64_cmpeq(b, mrln_v4i64_set1(0));
  tmp = mrln_v4u64_and(tmp, mrln_v4i64_set1(1));
  b = mrln_v4u64_or(b, tmp);

  c = mrln_v4i64_div(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] / b[i]), NULL);
  }

  c = mrln_v4i64_mod(a, b);
  for (int64_t i = 0; i < 4; i += 1) {
    TEST_UINT(&test, c[i], (int64_t)(a[i] % b[i]), NULL);
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
