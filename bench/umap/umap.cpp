#include <benchmark/benchmark.h>
#include <merlin/aloctr.h>
#include <merlin/umap.h>
#include <stdio.h>

static mrln_aloctr_t *a;

__attribute__((constructor)) static void setup(void) {
  a = mrln_aloctr_global();
}

static void BM_insert_bl(benchmark::State &state) {
  const size_t end = state.range(0);

  for (auto _ : state) {
    mrln_umap_t map = {};
    (void)mrln_umap(&map, end, a);
    for (size_t i = 0; i < end; ++i) {
      benchmark::DoNotOptimize(i);
    }
  }
}
BENCHMARK(BM_insert_bl)->Range(4, 1 << 20);

static void BM_insert(benchmark::State &state) {
  const size_t end = state.range(0);

  for (auto _ : state) {
    mrln_umap_t map = {};
    (void)mrln_umap(&map, end, a);
    for (size_t i = 0; i < end; ++i) {
      (void)mrln_umap_insert(&map, i, i, a);
    }
  }
}
BENCHMARK(BM_insert)->Range(4, 1 << 20);

static void BM_find_bl(benchmark::State &state) {
  const size_t end = state.range(0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(rand() % end);
  }
}
BENCHMARK(BM_find_bl)->Range(4, 1 << 20);

static void BM_find(benchmark::State &state) {
  const size_t end = state.range(0);
  mrln_umap_t map = {};
  (void)mrln_umap(&map, end, a);
  for (size_t i = 0; i < end; ++i) {
    (void)mrln_umap_insert(&map, i, i, a);
  }
  for (auto _ : state) {
    auto i = mrln_umap_find(&map, rand() % end);
    benchmark::DoNotOptimize(i);
  }
}
BENCHMARK(BM_find)->Range(4, 1 << 20);

BENCHMARK_MAIN();
