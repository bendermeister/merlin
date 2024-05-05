#include <benchmark/benchmark.h>
#include <merlin/aloctr.h>
#include <merlin/umap.h>
#include <stdio.h>

static mrln_aloctr_t *a;

static void BM_bench(benchmark::State &state) {
  for (auto _ : state) {
    a = mrln_aloctr_global();
    mrln_umap_t map = {};
    (void)mrln_umap(&map, state.range(0), a);

    uintptr_t i = 0;
    for (; i < state.range(0); ++i) {
      (void)mrln_umap_insert(&map, i, i, a);
    }

    mrln_umap_destroy(&map, a);
  }
}
BENCHMARK(BM_bench)
    ->Arg(1 << 1)
    ->Arg(1 << 2)
    ->Arg(1 << 3)
    ->Arg(1 << 4)
    ->Arg(1 << 5)
    ->Arg(1 << 6)
    ->Arg(1 << 7)
    ->Arg(1 << 8)
    ->Arg(1 << 9)
    ->Arg(1 << 10)
    ->Arg(1 << 11)
    ->Arg(1 << 12)
    ->Arg(1 << 13)
    ->Arg(1 << 14)
    ->Arg(1 << 15)
    ->Arg(1 << 16)
    ->Arg(1 << 17)
    ->Arg(1 << 18)
    ->Arg(1 << 19);

BENCHMARK_MAIN();
