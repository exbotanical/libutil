#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Buffer *buffer_init(const char *init) {
  Buffer *buf = malloc(sizeof(Buffer));
  if (!buf) {
    buffer_free(buf);
    return NULL;
  }

  buf->state = NULL;
  buf->len = 0;
  if (init != NULL) {
    buffer_append(buf, init);
  }

  return buf;
}

bool buffer_append(Buffer *buf, const char *s) {
  if (!s) {
    return false;
  }

  int len = strlen(s);

  // get mem sizeof current str + sizeof append str
  char *next = realloc(buf->state, buf->len + len);
  if (!next) {
    buffer_free(next);
    return false;
  }

  memcpy(&next[buf->len], s, len);
  buf->state = next;
  buf->len += len;

  return true;
}

Buffer *buffer_concat(Buffer *buf_a, Buffer *buf_b) {
  Buffer *buf = buffer_init(buf_a->state);
  if (!buf) {
    buffer_free(buf);
    return NULL;
  }

  if (!buffer_append(buf, buf_b->state)) {
    buffer_free(buf);
    return NULL;
  }

  return buf;
}

void buffer_free(Buffer *buf) {
  // Because Buffer's state member is initialized lazily, we need only dealloc
  // if it was actually allocated to begin with
  if (buf->len) {
    free(buf->state);
    buf->state = NULL;
  }

  free(buf);
}
