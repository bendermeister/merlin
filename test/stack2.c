#include <merlin/stack2.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int *ptr = mrln_stack2();

  for (int i = 0; i < 100; ++i) {
    ptr[i] = rand();
  }

  for (int i = 0; i < 100; ++i) {
    printf("ptr[%d] = %d", i, ptr[i]);
  }

  return 0;
}
