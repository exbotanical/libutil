#include "str.h"

#include <stdlib.h>
#include <string.h>

char *str_truncate(const char *s, int len) {
  unsigned int full_len = strlen(s);

  // Simply return a copy if invalid len
  if (len <= 0 || len >= (unsigned int)strlen(s)) {
    char *sc = malloc(full_len);
    strncpy(sc, s, full_len);

    return sc;
  }

  char *sc = malloc(len + 1);
  strncpy(sc, s, len + 1);
  sc[len] = '\0';

  return sc;
}
