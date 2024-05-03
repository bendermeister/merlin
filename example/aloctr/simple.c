#include <merlin/aloctr.h>
#include <stddef.h> // for NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  // get the global allocator

  mrln_aloctr_t *a = mrln_aloctr_global();

  // now we can use it to allocate an array

  int *array = NULL;
  intptr_t capacity = 0;

  // Explanation:
  // 1. parameter is the allocator we want to use
  // 2. paramter is a pointer to the current buffer, and will be set to
  //    the new buffer want to allocate
  // 3. paramter is the current capacity (in bytes) of the buffer, and will
  //    be set to the new capacity of the buffer
  // 4. paramter is the alignment of the buffer
  // 5. paramter is the new capcity of the buffer
  // -  return value is an error value
  int err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int),
                       10 * sizeof(*array));

  // check the error value
  if (err) {
    // in most cases the error is an ENOMEM
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  // now we can use our array
  for (int i = 0; i < 10; ++i) {
    array[i] = i;
  }

  // OH NO there is no space left in the array but I really need to put this
  // `69` in there
  //
  // no problem we can use the same function to reallocate the array

  err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int),
                   11 * sizeof(*array));

  // this reallocates the array, please keep in mind that capacity needs to be
  // the capacity in bytes of the current buffer, a mrln_aloctr is not required
  // to keep track of the buffer sizes

  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  array[10] = 69;

  // we use the same function again to deallocate the buffer, by reallocating it
  // to size zero, this will set `capacity` to `0` and `array` to NULL
  err = mrln_alloc(a, (void **)&array, &capacity, _Alignof(int), 0);

  if (err) {
    // checking the error value of the global allocator is not necessary, this
    // might not be true for custom, or special allocators
  }

  return 0;
}
