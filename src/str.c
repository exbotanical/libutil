#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

size_t sizeof_char = sizeof(char);

char *str_truncate(const char *s, int len) {
  unsigned int full_len = strlen(s);

  // Simply return a copy if invalid len
  if (len <= 0 || len >= (int)strlen(s)) {
    char *ret = malloc(full_len);
    strncpy(ret, s, full_len);

    return ret;
  }

  char *ret = malloc(len + 1);
  strncpy(ret, s, len + 1);
  ret[len] = '\0';

  return ret;
}

char *str_concat(const char *s1, const char *s2) {
  unsigned int size = sizeof(s1) + sizeof(s1);
  char *ret = malloc(size);

  snprintf(ret, strlen(s1) + strlen(s2) + 1, "%s%s", s1, s2);

  return ret;
}
