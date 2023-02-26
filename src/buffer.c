#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

Buffer *buffer_init(const char *init) {
  Buffer *buf = malloc(sizeof(Buffer));
  if (!buf) {
    free(buf);
    return NULL;
  }

  ((buffer_t *)buf)->state = NULL;
  ((buffer_t *)buf)->len = 0;

  if (init != NULL) {
    buffer_append(buf, init);
  }

  return buf;
}

bool buffer_append(Buffer *buf, const char *s) {
  if (!s) {
    return false;
  }

  buffer_t *internal = (buffer_t *)buf;
  unsigned int len = strlen(s);

  // get mem sizeof current str + sizeof append str
  char *next = realloc(internal->state, internal->len + len + 1);
  if (!next) {
    free(next);
    return false;
  }

  memcpy(&next[internal->len], s, len + 1);
  internal->state = next;
  internal->len += len;

  return true;
}

bool buffer_append_with(Buffer *buf, const char *s, unsigned int len) {
  buffer_t *internal = (buffer_t *)buf;

  char *next = realloc(internal->state, internal->len + len);
  if (!next) {
    return false;
  }

  memcpy(&next[internal->len], s, len);
  internal->state = next;
  internal->len += len;

  return true;
}

Buffer *buffer_concat(Buffer *buf_a, Buffer *buf_b) {
  Buffer *buf = buffer_init(((buffer_t *)buf_a)->state);
  if (!buf) {
    buffer_free(buf);
    return NULL;
  }

  if (!buffer_append(buf, ((buffer_t *)buf_b)->state)) {
    buffer_free(buf);
    return NULL;
  }

  return buf;
}

void buffer_free(Buffer *buf) {
  buffer_t *internal = (buffer_t *)buf;

  // Because Buffer's state member is initialized lazily, we need only dealloc
  // if it was actually allocated to begin with
  if (internal->len) {
    free(internal->state);
    internal->state = NULL;
  }

  free(internal);
}
