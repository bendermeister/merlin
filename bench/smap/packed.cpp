#include "packed/packed.h"
#include <benchmark/benchmark.h>
#include <merlin/aloctr.h>
#include <merlin/smap.h>
#include <merlin/str8.h>
#include <stdlib.h>

mrln_aloctr_t *a;

#define MAX_ENTRIES 20
#define MAX_KEY_LEN 32

struct {
  uint8_t *key_buf;
  mrln_str8view_t *key;
} data = {};

__attribute__((constructor)) static void setup_data(void) {
  data.key_buf = (uint8_t *)malloc((1 << MAX_ENTRIES) * MAX_KEY_LEN);
  data.key = (mrln_str8view_t *)malloc((1 << MAX_ENTRIES) * sizeof(*data.key));

  for (intptr_t i = 0; i < (1 << MAX_ENTRIES) * MAX_KEY_LEN; ++i) {
    data.key_buf[i] = rand() & 255;
  }
}

__attribute__((destructor)) static void teardown_data(void) {
  free(data.key_buf);
  free(data.key);
}

static void generate_fixlen_keys(const intptr_t vec_len,
                                 const intptr_t key_len) {
  intptr_t key_buf_len = 0;
  for (intptr_t i = 0; i < vec_len; ++i) {
    data.key[i].length = key_len;
    data.key[i].buffer = &data.key_buf[key_buf_len];
    key_buf_len += key_len;
  }
}

static void generate_randlen_keys(const intptr_t vec_len,
                                  const intptr_t key_len) {
  intptr_t key_buf_len = 0;
  for (intptr_t i = 0; i < vec_len; ++i) {
    intptr_t len = (rand() % key_len) + 1;
    data.key[i].length = len;
    data.key[i].buffer = &data.key_buf[key_buf_len];
    key_buf_len += len;
  }
}

static void BM_insert_fixlen_bl(benchmark::State &state) {
  a = mrln_aloctr_global();

  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);

  generate_fixlen_keys(num_elements, key_len);

  for (auto _ : state) {
    packed_t map = {};
    (void)packed(&map, num_elements, a);
    for (intptr_t i = 0; i < num_elements; ++i) {
      auto x = data.key[i];
      benchmark::DoNotOptimize(x);
    }
    packed_destroy(&map, a);
  }
}

BENCHMARK(BM_insert_fixlen_bl)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_insert_fixlen(benchmark::State &state) {
  a = mrln_aloctr_global();

  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);
  generate_fixlen_keys(num_elements, key_len);

  for (auto _ : state) {
    packed_t map = {};
    (void)packed(&map, num_elements, a);
    for (intptr_t i = 0; i < num_elements; ++i) {
      (void)packed_insert(&map, &data.key[i], (uintptr_t)i, a);
    }
    packed_destroy(&map, a);
  }
}

BENCHMARK(BM_insert_fixlen)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_find_fixlen_bl(benchmark::State &state) {
  const intptr_t num_elements = 1 << state.range(0);
  for (auto _ : state) {
    auto x = data.key[rand() % num_elements];
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_find_fixlen_bl)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_find_fixlen(benchmark::State &state) {
  a = mrln_aloctr_global();

  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);

  generate_fixlen_keys(num_elements, key_len);

  packed_t map = {};
  (void)packed(&map, num_elements, a);
  for (intptr_t i = 0; i < num_elements; ++i) {
    (void)packed_insert(&map, &data.key[i], (uintptr_t)i, a);
  }

  for (auto _ : state) {
    auto i = packed_find(&map, &data.key[rand() % num_elements]);
    benchmark::DoNotOptimize(i);
  }

  packed_destroy(&map, a);
}

BENCHMARK(BM_find_fixlen)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_insert_randlen_bl(benchmark::State &state) {
  a = mrln_aloctr_global();
  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);
  generate_randlen_keys(num_elements, key_len);

  for (auto _ : state) {
    packed_t map = {};
    (void)packed(&map, num_elements, a);
    for (intptr_t i = 0; i < num_elements; ++i) {
      auto x = data.key[i];
      benchmark::DoNotOptimize(x);
    }
    packed_destroy(&map, a);
  }
}

BENCHMARK(BM_insert_randlen_bl)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_insert_randlen(benchmark::State &state) {
  a = mrln_aloctr_global();
  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);
  generate_randlen_keys(num_elements, key_len);
  for (auto _ : state) {
    packed_t map = {};
    (void)packed(&map, num_elements, a);
    for (intptr_t i = 0; i < num_elements; ++i) {
      (void)packed_insert(&map, &data.key[i], (uintptr_t)i, a);
    }
    packed_destroy(&map, a);
  }
}

BENCHMARK(BM_insert_randlen)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_find_randlen_bl(benchmark::State &state) {
  const intptr_t num_elements = 1 << state.range(0);
  for (auto _ : state) {
    auto x = data.key[rand() % num_elements];
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_find_randlen_bl)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

static void BM_find_randlen(benchmark::State &state) {
  a = mrln_aloctr_global();
  const intptr_t num_elements = 1 << state.range(0);
  const intptr_t key_len = state.range(1);
  generate_randlen_keys(num_elements, key_len);

  packed_t map = {};
  (void)packed(&map, num_elements, a);
  for (intptr_t i = 0; i < num_elements; ++i) {
    (void)packed_insert(&map, &data.key[i], (uintptr_t)i, a);
  }

  for (auto _ : state) {
    auto i = packed_find(&map, &data.key[rand() % num_elements]);
    benchmark::DoNotOptimize(i);
  }

  packed_destroy(&map, a);
}

BENCHMARK(BM_find_randlen)
    ->ArgsProduct({{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
                   {4, 8, 16, 32}});

BENCHMARK_MAIN();
