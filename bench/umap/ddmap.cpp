#include "ddmap/ddmap.h"
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

constexpr static struct ddmap_vt_t vt = {
    .iseq = uptr_iseq,
    .hash = uptr_hash,
    .keysz = 8,
    .valsz = 8,
};

static void BM_insert_bl(benchmark::State &state) {
  const uintptr_t end = state.range(0);

  for (auto _ : state) {
    ddmap_t map = {};
    (void)ddmap(&map, &vt, end, a);
    for (uintptr_t i = 0; i < end; ++i) {
      benchmark::DoNotOptimize(i);
    }
  }
}
BENCHMARK(BM_insert_bl)->Range(4, 1 << 20);

static void BM_insert(benchmark::State &state) {
  const uintptr_t end = state.range(0);

  for (auto _ : state) {
    ddmap_t map = {};
    (void)ddmap(&map, &vt, end, a);
    for (uintptr_t i = 0; i < end; ++i) {
      (void)ddmap_insert(&map, &vt, &i, &i, a);
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
  ddmap_t map = {};
  (void)ddmap(&map, &vt, end, a);
  for (uintptr_t i = 0; i < end; ++i) {
    (void)ddmap_insert(&map, &vt, &i, &i, a);
  }
  for (auto _ : state) {
    uintptr_t k = rand() % end;
    auto i = ddmap_find(&map, &vt, &k);
    benchmark::DoNotOptimize(i);
  }
}
BENCHMARK(BM_find)->Range(4, 1 << 20);

BENCHMARK_MAIN();
