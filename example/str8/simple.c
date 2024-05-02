#include <merlin/str8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char cstr[] = "Oh  no  we  used  2  spaces.\n";

  // get the global allocator used to allocate the strings
  mrln_aloctr_t *a = mrln_aloctr_global();

  int err;

  // str8 has to be zeroed
  mrln_str8_t str8 = {};

  // allocate string and fill it with cstr
  err = mrln_str8(&str8, cstr, a);

  // check for errors
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  // replace "  " with " "
  mrln_str8view_t target = mrln_str8view("  ");
  mrln_str8view_t replace = mrln_str8view(" ");
  err = mrln_str8_replace(&str8, &target, &replace, a);

  // check for errors
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  printf("%.*s\n", (int)str8.length, str8.buffer);

  // deallocate str8
  mrln_str8_destroy(&str8, a);

  return 0;
}
