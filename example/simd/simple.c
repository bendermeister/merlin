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
