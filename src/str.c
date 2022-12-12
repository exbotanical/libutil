#include "str.h"

#include <stdlib.h>
#include <string.h>

char *str_truncate(const char *s, int len) {
  int full_len = strlen(s);

  if (len <= 0 || len >= (int)strlen(s)) {
    char *sc = malloc(full_len);
    strncpy(sc, s, full_len);

    return sc;
  }

  char *sc = malloc(len + 1);
  strncpy(sc, s, len + 1);
  sc[len] = '\0';

  return sc;
}
