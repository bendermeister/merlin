#include <merlin/str8.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char cstr[] = "Oh  no  we  used  2  spaces.\n";

  // get view from cstr
  const merlin_str8_view_t cstr_view = merlin_str8_view_from_static_cstr(cstr);

  int err;

  // concatonate view with empty string
  merlin_str8_t str8 = {};
  err = merlin_str8_concat(&str8, &cstr_view);
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  err = merlin_str8_replace(&str8, &merlin_str8_view_from_static_cstr("  "),
                            &merlin_str8_view_from_static_cstr(" "));
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  printf("%.*s\n", (int)str8.length, str8.buffer);

  return 0;
}
