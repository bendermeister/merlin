#include <merlin/simd.h>

#include <stdio.h>

int main(void) {
  const v4u32_t b = v4u32_set1(35);
  const v4u32_t a = v4u32_set1(34);

  v4u32_t c = v4u32_add(a, b);

  uint32_t array[4];

  v4u32_store_unaligned((void *)array, c);

  for (int i = 0; i < 4; ++i) {
    printf("%u\n", array[i]);
  }

  return 0;
}
