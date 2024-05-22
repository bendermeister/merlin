#include <errno.h>
#include <merlin/aloctr.h>
#include <stddef.h> // for NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  // get the global allocate, which is thread safe and can be thought of as a
  // malloc/realloc/free replacement
  aloctr_t *a = aloctr_global();

  // now we can use it to allocate an array of ints
  int *array = allocate(a, 10 * sizeof(*array));

  // check if an error occurred
  if (array == NULL) {
    // NOTE: if `size` passed to allocate is `0` it will return `NULL`, so this
    // might not always be an error, if you might have `0` as a valid size check
    // errno too!
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    exit(1);
  }

  // now we fill the array
  for (int i = 0; i < 10; ++i) {
    array[i] = i;
  }

  // we can also reallocate the array

  {
    void *new = reallocate(a, array, 11 * sizeof(*array));
    if (new == NULL) {
      // NOTE: if `size` passed to reallocate is `0` it will return `NULL`, so
      // this might not always be an error, if you might have `0` as a valid
      // size check errno too!
      fprintf(stderr, "ERROR: %s\n", strerror(errno));
      exit(1);
    }

    // in case of an error `array` will not be altered so we have to asign to a
    // tmp variable before assigning it to `array`
    array = new;
  }

  array[10] = 69;

  deallocate(a, array);

  return 0;
}
