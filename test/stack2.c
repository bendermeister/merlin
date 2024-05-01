#include <broken/unit.h>
#include <merlin/stack2.h>
#include <stdio.h>
#include <stdlib.h>

static test_t TEST_stack2(void) {
  test_t test = TEST_MAKE();

  int *ptr = mrln_stack2();
  int *base_ptr = ptr;

  for (int i = 0; i < 1024; ++i) {
    ptr[i] = i;
  }

  for (int i = 0; i < 1024; ++i) {
    TEST_INT(&test, ptr[i], i, NULL);
  }

  mrln_stack2_set(ptr + 1024);

  ptr = mrln_stack2();

  for (int i = 0; i < 2048; ++i) {
    ptr[i] = -1;
  }

  for (int i = 0; i < 2048; ++i) {
    TEST_INT(&test, ptr[i], -1, NULL);
  }

  mrln_stack2_set(base_ptr);

  ptr = mrln_stack2();

  for (int i = 0; i < 1024; ++i) {
    TEST_INT(&test, ptr[i], i, NULL);
  }

  return test;
}

int main(void) {
  TEST_RUN(TEST_stack2());
  return 0;
}
