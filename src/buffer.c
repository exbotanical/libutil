#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

int buffer_size(buffer_t *buf) { return ((__buffer_t *)buf)->len; }

char *buffer_state(buffer_t *buf) { return ((__buffer_t *)buf)->state; }

buffer_t *buffer_init(const char *init) {
  __buffer_t *buf = malloc(sizeof(__buffer_t));
  if (!buf) {
    free(buf);
    return NULL;
  }

  buf->state = NULL;
  buf->len = 0;

  if (init != NULL) {
    buffer_append((buffer_t *)buf, init);
  }

  return (buffer_t *)buf;
}

bool buffer_append(buffer_t *buf, const char *s) {
  if (!s) {
    return false;
  }

  __buffer_t *internal = (__buffer_t *)buf;
  unsigned int len = strlen(s);

  // get mem sizeof current str + sizeof append str
  char *next = realloc(internal->state, internal->len + len + 1);
  if (!next) {
    free(next);
    return false;
  }

  memcpy(&next[internal->len], s, len);
  next[internal->len + len] = '\0';
  internal->state = next;
  internal->len += len;

  return true;
}

bool buffer_append_with(buffer_t *buf, const char *s, unsigned int len) {
  __buffer_t *internal = (__buffer_t *)buf;

  char *next = realloc(internal->state, internal->len + len + 1);
  if (!next) {
    return false;
  }

  memcpy(&next[internal->len], s, len);
  next[internal->len + len] = '\0';
  internal->state = next;
  internal->len += len;

  return true;
}

buffer_t *buffer_concat(buffer_t *buf_a, buffer_t *buf_b) {
  buffer_t *buf = buffer_init(((__buffer_t *)buf_a)->state);
  if (!buf) {
    buffer_free(buf);
    return NULL;
  }

  if (!buffer_append(buf, ((__buffer_t *)buf_b)->state)) {
    buffer_free(buf);
    return NULL;
  }

  return buf;
}

void buffer_free(buffer_t *buf) {
  __buffer_t *internal = (__buffer_t *)buf;

  // Because buffer_t's state member is initialized lazily, we need only dealloc
  // if it was actually allocated to begin with
  if (internal->len) {
    free(internal->state);
    internal->state = NULL;
  }

  free(internal);
}

// TODO: error enums
// TODO: rename all to `self`
// TODO: remove inclusivity flags - user can just pass the end index they want
int buffer_slice(buffer_t *self, unsigned int start, unsigned int end_inclusive,
                 char *dest) {
  __buffer_t *internal = (__buffer_t *)self;

  if (internal->len < end_inclusive || internal->state == NULL) {
    return -1;
  }

  unsigned int x = 0;
  unsigned int i = start;
  for (; i <= end_inclusive; i++, x++) {
    memcpy(&dest[x], &internal->state[i], 1);
  }

  dest[x] = '\0';

  return 0;
}
