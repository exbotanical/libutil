#include "str.h"

#include <string.h>

char *str_truncate(const char *s, int len) {
  if (len <= 0 || len >= strlen(s)) {
    return s;
  }

  char *sc = malloc(len + 1);
  strncpy(sc, s, len + 1);
  sc[len] = '\0';

  return sc;
}
