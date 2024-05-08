#include "gmap/gmap.h"
#include <benchmark/benchmark.h>
#include <merlin/aloctr.h>
#include <stdio.h>

static mrln_aloctr_t *a;

__attribute__((constructor)) static void setup(void) {
  a = mrln_aloctr_global();
}

static bool uptr_iseq(const void *a, const void *b) {
  return *(uintptr_t *)a == *(uintptr_t *)b;
}

CONST_FUNC NODISCARD static u64 uptr_hash(const void *p) {
  uintptr_t x = *(uintptr_t *)p;
  x ^= x >> 30;
  x *= 0xbf58476d1ce4e5b9U;
  x ^= x >> 27;
  x *= 0x94d049bb133111ebU;
  x ^= x >> 31;
  return x;
}

static void BM_insert_bl(benchmark::State &state) {
  const uintptr_t end = state.range(0);

  for (auto _ : state) {
    gmap_t map = {};
    for (uintptr_t i = 0; i < end; ++i) {
      benchmark::DoNotOptimize(i);
    }
  }
}
BENCHMARK(BM_insert_bl)->Range(4, 1 << 20);

static void BM_insert(benchmark::State &state) {
  const uintptr_t end = state.range(0);

  for (auto _ : state) {
    gmap_t map = {};
    for (uintptr_t i = 0; i < end; ++i) {
      gmap_iter_t iter = gmap_iter(&map, uptr_hash(&i));
      for (; iter.isset; iter = gmap_next(iter)) {
      }
      gmap_insert(&map, &iter);
      *gmap_key(iter) = i;
      *gmap_val(iter) = i;
    }
  }
}
BENCHMARK(BM_insert)->Range(4, 1 << 20);

static void BM_find_bl(benchmark::State &state) {
  const uintptr_t end = state.range(0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(rand() % end);
  }
}
BENCHMARK(BM_find_bl)->Range(4, 1 << 20);

static void BM_find(benchmark::State &state) {
  const uintptr_t end = state.range(0);
  gmap_t map = {};
  for (uintptr_t i = 0; i < end; ++i) {
    gmap_iter_t iter = gmap_iter(&map, uptr_hash(&i));
    for (; iter.isset; iter = gmap_next(iter)) {
    }
    gmap_insert(&map, &iter);
    *gmap_key(iter) = i;
    *gmap_val(iter) = i;
  }

  for (auto _ : state) {
    uintptr_t k = rand() % end;
    gmap_iter_t iter = gmap_iter(&map, uptr_hash(&k));
    for (; iter.isset && *gmap_key(iter) != k; iter = gmap_next(iter)) {
    }
  }
}
BENCHMARK(BM_find)->Range(4, 1 << 20);

BENCHMARK_MAIN();
