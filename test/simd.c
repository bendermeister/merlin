#include <broken/unit.h>
#include <merlin/simd.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <time.h>

//=================================v16u8========================================

static test_t TEST_v16u8_load_store(void) {
  test_t test = TEST_MAKE();

  // aligned load
  uint8_t *arr = malloc(17 * sizeof(*arr));
  uint8_t *store_arr = malloc(17 * sizeof(*store_arr));

  (void)memset(store_arr, 0, 17 * sizeof(*store_arr));

  for (uint8_t i = 0; i < 17; i += 1) {
    arr[i] = i;
  }

  merlin_v16u8_t vec = merlin_v16u8_load_aligned((void *)arr);

  for (uint8_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, vec[i], arr[i], TEST_MAKE_STR("i: %u", i));
  }

  merlin_v16u8_store_aligned((void *)store_arr, vec);

  for (uint8_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, store_arr[i], arr[i], TEST_MAKE_STR("i: %u", i));
  }
  (void)memset(store_arr, 0, 17 * sizeof(*store_arr));

  vec = merlin_v16u8_load_unaligned((void *)(&arr[1]));

  for (uint8_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, vec[i], arr[i + 1], TEST_MAKE_STR("i: %u", i));
  }

  merlin_v16u8_store_unaligned((void *)(&store_arr[1]), vec);

  for (uint8_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, store_arr[i + 1], arr[i + 1], TEST_MAKE_STR("i: %u", i));
  }
  (void)memset(store_arr, 0, 17 * sizeof(*store_arr));

  free(arr);
  free(store_arr);

  return test;
}

static test_t TEST_v16u8_set(void) {
  test_t test = TEST_MAKE();

  merlin_v16u8_t vec = merlin_v16u8_set1(69);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, vec[i], 69, TEST_MAKE_STR("i: %u", i));
  }

  vec = merlin_v16u8_set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, vec[i], i, TEST_MAKE_STR("i: %d", i));
  }

  return test;
}

static test_t TEST_v16u8_arithmetic(void) {

  time_t t = time(NULL);
  test_t test = TEST_MAKE(TEST_MAKE_STR("%s(%ld)", (char *)__func__, t));
  srand(t);

  merlin_v16u8_t a = merlin_v16u8_set(
      rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(),
      rand(), rand(), rand(), rand(), rand(), rand(), rand());
  merlin_v16u8_t b = merlin_v16u8_set(
      rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(),
      rand(), rand(), rand(), rand(), rand(), rand(), rand());
  merlin_v16u8_t c;

  c = merlin_v16u8_add(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] + b[i]), TEST_MAKE_STR("i: %u", i));
  }

  c = merlin_v16u8_sub(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] - b[i]), TEST_MAKE_STR("i: %u", i));
  }

  c = merlin_v16u8_mul(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] * b[i]), TEST_MAKE_STR("i: %u", i));
  }

  c = merlin_v16u8_div(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] / b[i]), TEST_MAKE_STR("i: %u", i));
  }

  c = merlin_v16u8_mod(a, b);
  for (uint32_t i = 0; i < 16; i += 1) {
    TEST_UINT(&test, c[i], (uint8_t)(a[i] % b[i]), TEST_MAKE_STR("i: %u", i));
  }

  return test;
}

int main(void) {
  TEST_RUN(TEST_v16u8_load_store());
  TEST_RUN(TEST_v16u8_set());
  TEST_RUN(TEST_v16u8_arithmetic());
  TEST_CLEANUP();
  return 0;
}
