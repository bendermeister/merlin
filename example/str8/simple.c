#include <merlin/str8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  const char cstr[] = "Oh  no  we  used  2  spaces.\n";

  mrln_str8view_t cstr_view = mrln_str8view(cstr);

  int err;

  // concatonate view with empty string
  mrln_str8_t str8 = {};
  err = mrln_str8_concat(&str8, &cstr_view);
  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  mrln_str8view_t target = mrln_str8view("  ");
  mrln_str8view_t replace = mrln_str8view(" ");
  err = mrln_str8_replace(&str8, &target, &replace);

  if (err) {
    fprintf(stderr, "ERROR: %s\n", strerror(err));
    exit(1);
  }

  printf("%.*s\n", (int)str8.length, str8.buffer);

  mrln_str8_destroy(&str8);

  return 0;
}
