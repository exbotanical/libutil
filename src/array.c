#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

#include "libutil.h"

unsigned int array_size(Array *array) { return ((array_t *)array)->len; }

void *array_get(Array *array, int index) {
  array_t *internal = (array_t *)array;

  if (index == -1) {
    if (internal->len == 0) {
      return NULL;
    }
    return internal->state[internal->len - 1];
  }

  if (index > internal->len) {
    return NULL;
  }

  return internal->state[index];
}

Array *array_init() {
  Array *array = malloc(sizeof(Array));
  if (!array) {
    errno = ENOMEM;
    return NULL;
  }

  array_t *internal = (array_t *)array;

  internal->state = malloc(sizeof(void *));
  if (!internal->state) {
    errno = ENOMEM;
    return NULL;
  }

  internal->len = 0;

  return array;
}

bool array_includes(Array *array, ComparatorFunction *comparator,
                    void *compare_to) {
  array_t *internal = (array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return true;
    }
  }

  return false;
}

int array_find(Array *array, ComparatorFunction *comparator, void *compare_to) {
  array_t *internal = (array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return i;
    }
  }

  return -1;
}

bool array_push(Array *array, void *el) {
  array_t *internal = (array_t *)array;

  void **next_state =
      realloc(internal->state, (internal->len + 1) * sizeof(void *));
  if (!next_state) {
    free(next_state);
    errno = ENOMEM;

    return false;
  }

  internal->state = next_state;
  internal->state[internal->len++] = el;

  return true;
}

void *array_pop(Array *array) {
  array_t *internal = (array_t *)array;

  unsigned int len = internal->len;

  if (len > 0) {
    unsigned int next_len = len - 1;

    void *el = internal->state[next_len];
    internal->state[next_len] = NULL;
    internal->len = next_len;

    return el;
  }

  return NULL;
}

void *array_shift(Array *array) {
  array_t *internal = (array_t *)array;

  if (internal->len == 0) {
    return false;
  }

  void *el = internal->state[0];
  Array *new = array_init();

  for (unsigned int i = 1; i < internal->len; i++) {
    array_push(new, internal->state[i]);
  }

  internal->len--;
  *array = *new;
  return el;
}

Array *array_slice(Array *array, unsigned start, int end) {
  array_t *internal = (array_t *)array;
  Array *slice = array_init();

  // TODO: test -1
  unsigned int normalized_end = end == -1 ? (int)internal->len : end;
  for (unsigned int i = start; i < normalized_end; i++) {
    array_push(slice, internal->state[i]);
  }

  return slice;
}

bool array_remove(Array *array, unsigned int index) {
  array_t *internal = (array_t *)array;

  if (internal->len < index) {
    return false;
  }

  for (unsigned int i = 0; i < internal->len - 1; i++) {
    if (i >= index) {
      internal->state[i] = internal->state[i + 1];
    }
  }

  internal->len--;
  return true;
}

Array *array_map(Array *array, CallbackFunction *callback) {
  array_t *internal = (array_t *)array;

  Array *ret = array_init();
  for (unsigned int i = 0; i < internal->len; i++) {
    array_push(ret, callback(internal->state[i], i, array));
  }

  return ret;
}

Array *array_filter(Array *array, PredicateFunction *predicate,
                    void *compare_to) {
  array_t *internal = (array_t *)array;

  Array *ret = array_init();
  for (unsigned int i = 0; i < internal->len; i++) {
    void *el = internal->state[i];
    if (predicate(el, i, array, compare_to)) {
      array_push(ret, el);
    }
  }

  return ret;
}

void array_foreach(Array *array, CallbackFunction *callback) {
  array_t *internal = (array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    callback(internal->state[i], i, array);
  }
}

void array_free(Array *array) {
  array_t *internal = (array_t *)array;

  free(internal->state);
  internal->state = NULL;
  free(internal);
}
